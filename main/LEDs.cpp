#include "LEDs.h"

#include <algorithm>
#include <cstring>
#include <vector>

#include "defaults.h"
#include "globals.h"
#include "led/Addressable.h"
#include "led/LED.h"
#include "led/SinglePWM.h"
#include "mqtt.h"
#include "Outputs.h"
#include "Settings.h"
#include "string_utils.h"
#include "util.h"

// Colours WS2812FX.h used to provide.
#define RED ((uint32_t)0xFF0000)
#define GREEN ((uint32_t)0x00FF00)
#define PURPLE ((uint32_t)0x400080)
#define PINK ((uint32_t)0xFF1493)

namespace LEDs {

std::vector<LED*> leds, statusLeds, countLeds, motionLeds;
bool online;
unsigned long lastSave = 0;

LED* newLed(uint8_t index, ControlType cntrl, int type, int pin, int cnt, const std::string& stateStr) {
    LED* led;
    if (pin == -1) {
        led = new LED(index, Control_Type_None);
    } else if (type >= 2) {
        led = new Addressable(index, cntrl, type - 2, pin, cnt);
    } else {
        led = new SinglePWM(index, cntrl, type == 1, pin);
    }
    led->setStateString(stateStr);
    return led;
}

namespace {
const char* const ledTypes[] = {"pwm", "pwm_inverted", "grb", "grbw", "rgb", "rgbw"};
const char* const ledControls[] = {"mqtt", "status", "motion", "count", "output"};
std::vector<std::pair<LED*, int>> outputLeds;  // control "output": LED, 1-based output number

template <size_t N>
int indexOf(const char* s, const char* const (&options)[N], int fallback) {
    for (size_t i = 0; i < N; i++)
        if (s && strcmp(s, options[i]) == 0) return (int)i;
    return fallback;
}
}  // namespace

/**
 * @brief Read the "leds" setting and build the LEDs.
 *
 * A JSON list such as [{"type": "grb", "pin": 27, "count": 25, "control": "status"}].
 * type: pwm | pwm_inverted | grb | grbw | rgb | rgbw (the last four addressable; count only
 * applies to those). control: mqtt | status | motion | count | output (mirrors output "output",
 * 1-based). max_brightness: 1-255 ceiling, default 100 for addressable, 255 for PWM. LED n (1-based list position) is
 * led_<n> in MQTT and keeps its colour in /led_<n>_state. The default is the board's LED.
 */
void ConnectToWifi(bool updating) {
    std::string def = DEFAULT_LED1_PIN < 0 ? "[]"
        : Sprintf(R"([{"type":"%s","pin":%d,"count":%d,"control":"%s"}])", ledTypes[DEFAULT_LED1_TYPE], DEFAULT_LED1_PIN, DEFAULT_LED1_CNT, ledControls[DEFAULT_LED1_CNTRL]);
    std::string text = Settings::json("leds", def, "LEDs");
    DynamicJsonDocument list(text.size() * 2 + 256);
    if (deserializeJson(list, text)) {
        Log.println("LEDs: invalid JSON, ignored");
        return;
    }
    for (JsonObject o : list.as<JsonArray>()) {
        if ((int)leds.size() >= MAX_LEDS) break;
        int n = leds.size() + 1;
        int type = indexOf(o["type"], ledTypes, 0);
        auto cntrl = (ControlType)indexOf(o["control"], ledControls, 0);
        int pin = o["pin"] | -1;
        int cnt = o["count"] | 1;
        leds.push_back(newLed(n, cntrl, type, pin, cnt, Settings::slurp(Sprintf("/led_%d_state", n))));
        // Addressable strips default to 100/255 so a dense matrix doesn't overheat.
        leds.back()->setMaxBrightness(o["max_brightness"] | (type >= 2 ? 100 : 255));
        if (cntrl == Control_Type_Output && pin >= 0) outputLeds.push_back({leds.back(), o["output"] | 1});
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
    for (auto& [led, n] : outputLeds) {
        bool on = Outputs::State(n);
        if (led->getState() != on) led->setState(on);
    }
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