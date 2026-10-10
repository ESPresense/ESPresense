#include "LEDs.h"

#include <algorithm>
#include <vector>

#include "defaults.h"
#include "globals.h"
#include "led/Addressable.h"
#include "led/LED.h"
#include "led/SinglePWM.h"
#include "mqtt.h"
#include "Settings.h"
#include "string_utils.h"
#include "util.h"

// Colours WS2812FX.h used to provide.
#define RED ((uint32_t)0xFF0000)
#define GREEN ((uint32_t)0x00FF00)
#define PURPLE ((uint32_t)0x400080)
#define PINK ((uint32_t)0xFF1493)

namespace LEDs {

int led_pwr_pin = -1;
std::vector<LED*> leds, statusLeds, countLeds, motionLeds;
bool online;
unsigned long lastSave = 0;

LED* newLed(uint8_t index, ControlType cntrl, int type, int pin, bool inverted, int cnt, const std::string& stateStr) {
    LED* led;
    if (pin == -1) {
        led = new LED(index, Control_Type_None);
    } else if (type >= 1) {
        led = new Addressable(index, cntrl, type - 1, pin, cnt);
    } else {
        led = new SinglePWM(index, cntrl, inverted, pin);
    }
    led->setStateString(stateStr);
    return led;
}

/**
 * @brief Register led_count and led_<n>_type/pin/cnt/cntrl/state for n = 1..led_count, and build the LEDs.
 *
 * LED 1 takes the board's DEFAULT_LED1_* values; the rest default to disabled.
 */
void ConnectToWifi(bool updating) {
    std::vector<std::string> ledTypes = {"PWM", "Addressable GRB", "Addressable GRBW", "Addressable RGB", "Addressable RGBW"};
    std::vector<std::string> ledControlTypes = {"MQTT", "Status", "Motion", "Count"};

    int count = Settings::integer("led_count", 0, MAX_LEDS, 3, "Number of LEDs");
    if (count < 0) count = 0;
    if (count > MAX_LEDS) count = MAX_LEDS;
    // led_<n>_state is the saved colour, not a form field, so it stays out of the group.
    Settings::group("led", MAX_LEDS, {"type", "pin", "inv", "cnt", "cntrl"});

    // Some boards (M5Stack NanoC6) only power their addressable LED while a GPIO is held high.
    led_pwr_pin = Settings::integer("led_pwr_pin", -1, 48, -1, "LED power pin (-1 to disable)");
    if (led_pwr_pin >= 0) {
        pinMode(led_pwr_pin, OUTPUT);
        digitalWrite(led_pwr_pin, HIGH);
    }

    for (int n = 1; n <= count; n++) {
        bool first = n == 1;
        int type = Settings::dropdown(Sprintf("led_%d_type", n), ledTypes, first ? DEFAULT_LED1_TYPE : 0, "LED Type");
        int pin = Settings::integer(Sprintf("led_%d_pin", n), -1, 48, first ? DEFAULT_LED1_PIN : -1, "Pin (-1 to disable)");
        bool inv = Settings::integer(Sprintf("led_%d_inv", n), 0, 1, first ? DEFAULT_LED1_INV : 0, "Inverted (PWM only)") == 1;
        int cnt = Settings::integer(Sprintf("led_%d_cnt", n), -1, 39, first ? DEFAULT_LED1_CNT : 1, "Count (only applies to Addressable LEDs)");
        auto cntrl = (ControlType)Settings::dropdown(Sprintf("led_%d_cntrl", n), ledControlTypes, first ? DEFAULT_LED1_CNTRL : 0, "LED Control");
        std::string const state = Settings::string(Sprintf("led_%d_state", n), "", "LED State");
        Settings::markState();
        leds.push_back(newLed(n, cntrl, type, pin, inv, cnt, state));
    }
    std::copy_if(leds.begin(), leds.end(), std::back_inserter(statusLeds), [](LED* a) { return a->getControlType() == Control_Type_Status; });
    std::copy_if(leds.begin(), leds.end(), std::back_inserter(countLeds), [](LED* a) { return a->getControlType() == Control_Type_Count; });
    std::copy_if(leds.begin(), leds.end(), std::back_inserter(motionLeds), [](LED* a) { return a->getControlType() == Control_Type_Motion; });
}

void SerialReport() {
}

bool sendState(LED* bulb) {
    DynamicJsonDocument doc(256);
    auto slug = slugify(bulb->getName());
    auto state = bulb->getState();
    doc["state"] = state ? MQTT_STATE_ON_PAYLOAD : MQTT_STATE_OFF_PAYLOAD;
    doc["color_mode"] = bulb->hasRgbw() ? "rgbw" : bulb->hasRgb() ? "rgb": "brightness";
    doc["brightness"] = bulb->getBrightness();
    if (bulb->hasRgbw()) {
        auto color = doc.createNestedObject("color");
        auto c = bulb->getColor();
        color["r"] = c.red;
        color["g"] = c.green;
        color["b"] = c.blue;
        color["w"] = c.white;
    } else if (bulb->hasRgb()) {
        auto color = doc.createNestedObject("color");
        auto c = bulb->getColor();
        color["r"] = c.red;
        color["g"] = c.green;
        color["b"] = c.blue;
    }
    std::string const setTopic = Sprintf("%s/%s", roomsTopic.c_str(), slug.c_str());
    return pub(setTopic.c_str(), 0, true, doc);
}

void Setup() {
    for (auto& led : leds)
        led->update();
}

/**
 * @brief Persists dirty state for MQTT-controlled LEDs to non-volatile storage.
 *
 * Iterates all LEDs and for each LED whose control type is MQTT and whose state is marked dirty,
 * clears the dirty flag, logs the save action including filename and state, and writes the LED's
 * state string to its associated state file.
 */
void Save() {
    for (auto& led : leds)
        if (led->getControlType() == Control_Type_MQTT && led->getDirty()) {
            led->setDirty(false);
            Log.printf("Saving %s: %s\n", led->getStateFilename().c_str(), led->getStateString().c_str());
            spurt(led->getStateFilename(), led->getStateString());
        }
}

void Loop() {
    for (auto& led : leds)
        led->service();
    if (millis() - lastSave > 15000) {
        lastSave = millis();
        Save();
    }
}

bool SendDiscovery() {
    for (auto& led : leds)
        if (led->getControlType() == Control_Type_MQTT && !sendLightDiscovery(led->getName(), EC_NONE, led->hasRgb(), led->hasRgbw()))
            return false;
    return true;
}

bool SendOnline() {
    if (online) return true;
    for (auto& led : leds)
        if (led->getControlType() > Control_Type_None && !sendState(led)) return false;
    online = true;
    return true;
}

void Connected(bool wifi, bool mqtt) {
    for (auto& led : statusLeds)
        led->setColor(wifi ? 128 : 0, 128, mqtt ? 128 : 0);
}

void Seen(bool inprogress) {
    for (auto& led : statusLeds)
        if (led->hasRgb()) {
            led->setColor(inprogress ? PURPLE : GREEN);
            led->setState(true);
        } else
            led->setState(inprogress);
}

void Wifi(unsigned int percent) {
    for (auto& led : statusLeds) {
        {
            led->setColor(RED);
            led->setState(percent % 2 == 0);
        }
    }
}

void Portal(unsigned int percent) {
    for (auto& led : statusLeds) {
        led->setColor(PINK);
        led->setState(percent % 2 == 0);
    }
}

void Update(unsigned int percent) {
    if (percent == UPDATE_STARTED) {
        for (auto& led : statusLeds)
            led->setColor(0, 128, 0);
    } else if (percent == UPDATE_COMPLETE) {
        for (auto& led : statusLeds)
            led->setColor(0, 128, 0);
    } else {
        for (auto& led : statusLeds)
            led->setState(percent % 2 == 0);
    }
}

LED* findBulb(std::string& command) {
    for (auto& led : leds) {
        if (led->getId() == command) {
            return led;
        }
    }
    return nullptr;
}

/**
 * @brief Apply a JSON command payload to the LED identified by command.
 *
 * Parses the provided JSON payload and updates the matched LED's color, brightness,
 * white value, color temperature, effect, and on/off state when those keys are present.
 *
 * @param command Identifier or slug used to locate the target LED.
 * @param pay JSON payload containing any of the supported keys: `color` (object with `r`, `g`, `b`),
 *            `brightness`, `white_value`, `color_temp`, `effect`, and `state` (compared to MQTT_STATE_ON_PAYLOAD).
 * @return true if a matching LED was found and the command was processed (note: returns `true` even if JSON deserialization fails);
 *         `false` if no LED matching `command` exists.
 */
bool Command(std::string& command, std::string& pay) {
    auto bulb = findBulb(command);
    if (bulb == nullptr) return false;
    DynamicJsonDocument root(pay.length() + 100);
    auto err = deserializeJson(root, pay);
    if (err) {
        Log.printf("LEDs::Command: deserializeJson: %s\n", err.c_str());
        return true;
    }
    bool sendNewState = false;
    if (root.containsKey("color")) {
        uint8_t r = root["color"]["r"];
        uint8_t g = root["color"]["g"];
        uint8_t b = root["color"]["b"];
        sendNewState = sendNewState || bulb->setColor(r, g, b);
    }

    if (root.containsKey("brightness")) {
        sendNewState = sendNewState || bulb->setBrightness(root["brightness"]);
    }

    if (root.containsKey("white_value")) {
        sendNewState = sendNewState || bulb->setWhite(root["white_value"]);
    }

    if (root.containsKey("color_temp")) {
        sendNewState = sendNewState || bulb->setColorTemperature(root["color_temp"]);
    }

    if (root.containsKey("effect")) {
        sendNewState = sendNewState || bulb->setEffect(root["effect"]);
    }

    if (root.containsKey("state"))
        sendNewState = sendNewState || bulb->setState(root["state"] == MQTT_STATE_ON_PAYLOAD);

    if (sendNewState) sendState(bulb);
    return true;
}

int count = 0, lastCount = 0;
void Counting(bool added) {
    if (added) {
        count++;
    } else {
        count--;
    }
    if (count != lastCount) {
        lastCount = count;
        for (auto& led : countLeds)
            led->setState(count > 0);
    }
}

void Count(unsigned int countVal) {
    count = countVal;
    for (auto& led : countLeds)
        led->setState(count > 0);
}

void Motion(bool motion) {
    for (auto& led : motionLeds)
        led->setState(motion);
}
}  // namespace LEDs