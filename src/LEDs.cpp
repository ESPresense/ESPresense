#include "LEDs.h"

#include <AsyncMqttClient.h>
#include <AsyncWiFiSettings.h>
#include <WS2812FX.h>

#include "LED.h"
#include "MotionSensors.h"
#include "defaults.h"
#include "globals.h"
#include "led/Addressable.h"
#include "led/SinglePWM.h"
#include "mqtt.h"
#include "string_utils.h"

namespace LEDs {

int led_1_type = DEFAULT_LED1_TYPE, led_2_type, led_3_type;
int led_1_pin = DEFAULT_LED1_PIN, led_2_pin, led_3_pin;
int led_1_cnt = DEFAULT_LED1_CNT, led_2_cnt, led_3_cnt;
ControlType led_1_cntrl = DEFAULT_LED1_CNTRL, led_2_cntrl, led_3_cntrl;
std::vector<LED*> leds, status, count, motion;
bool online;

void ConnectToWifi() {
    std::vector<String> ledTypes = {"Addressable GRB", "Addressable RGBW", "Addressable RGB", "Single PWM", "Single PWM Inverted"};
    std::vector<String> ledControlTypes = {"MQTT", "Status", "Motion", "Count"};

    AsyncWiFiSettings.heading("LEDs <a href='https://espresense.com/configuration/settings#leds' target='_blank'>ℹ️</a>", false);

    AsyncWiFiSettings.html("h4", "LED 1:");

    led_1_type = AsyncWiFiSettings.dropdown("led_1_type", ledTypes, DEFAULT_LED1_TYPE, "LED Type");
    led_1_pin = AsyncWiFiSettings.integer("led_1_pin", -1, 39, DEFAULT_LED1_PIN, "Pin (-1 to disable)");
    led_1_cnt = AsyncWiFiSettings.integer("led_1_cnt", -1, 39, DEFAULT_LED1_CNT, "Count (only applies to Addressable LEDs)");
    led_1_cntrl = (ControlType)AsyncWiFiSettings.dropdown("led_1_cntrl", ledControlTypes, DEFAULT_LED1_CNTRL, "LED Control");

    AsyncWiFiSettings.html("h4", "LED 2:");

    led_2_type = AsyncWiFiSettings.dropdown("led_2_type", ledTypes, 0, "LED Type");
    led_2_pin = AsyncWiFiSettings.integer("led_2_pin", -1, 39, -1, "Pin (-1 to disable)");
    led_2_cnt = AsyncWiFiSettings.integer("led_2_cnt", -1, 39, 1, "Count (only applies to Addressable LEDs)");
    led_2_cntrl = (ControlType)AsyncWiFiSettings.dropdown("led_2_cntrl", ledControlTypes, 0, "LED Control");

    AsyncWiFiSettings.html("h4", "LED 3:");

    led_3_type = AsyncWiFiSettings.dropdown("led_3_type", ledTypes, 0, "LED Type");
    led_3_pin = AsyncWiFiSettings.integer("led_3_pin", -1, 39, -1, "Pin (-1 to disable)");
    led_3_cnt = AsyncWiFiSettings.integer("led_3_cnt", -1, 39, 1, "Count (only applies to Addressable LEDs)");
    led_3_cntrl = (ControlType)AsyncWiFiSettings.dropdown("led_3_cntrl", ledControlTypes, 0, "LED Control");

    leds.push_back(led_1_type <= 2 ? (LED*)(new Addressable(1, led_1_cntrl, led_1_type, led_1_pin, led_1_cnt)) : new SinglePWM(1, led_1_cntrl, led_1_type, led_1_pin, led_1_cnt));
    leds.push_back(led_2_type <= 2 ? (LED*)(new Addressable(2, led_2_cntrl, led_2_type, led_2_pin, led_2_cnt)) : new SinglePWM(2, led_2_cntrl, led_2_type, led_2_pin, led_2_cnt));
    leds.push_back(led_3_type <= 2 ? (LED*)(new Addressable(3, led_3_cntrl, led_3_type, led_3_pin, led_3_cnt)) : new SinglePWM(3, led_3_cntrl, led_3_type, led_3_pin, led_3_cnt));
    std::copy_if(leds.begin(), leds.end(), std::back_inserter(status), [](LED* a) { return a->getControlType() == Control_Type_Status; });
    std::copy_if(leds.begin(), leds.end(), std::back_inserter(count), [](LED* a) { return a->getControlType() == Control_Type_Count; });
    std::copy_if(leds.begin(), leds.end(), std::back_inserter(motion), [](LED* a) { return a->getControlType() == Control_Type_Motion; });
}

void SerialReport() {
}

bool SendState(LED* bulb) {
    char buffer[512];
    DynamicJsonDocument doc(512);
    auto slug = slugify(bulb->getName());
    auto state = bulb->getState();
    doc["state"] = state ? MQTT_STATE_ON_PAYLOAD : MQTT_STATE_OFF_PAYLOAD;
    if (state) {
        doc["brightness"] = bulb->getBrightness();
        auto color = doc.createNestedObject("color");
        auto c = bulb->getColor();
        color["r"] = c.red;
        color["g"] = c.green;
        color["b"] = c.blue;
        // doc["white_value"] = bulb->getColor().white;
        // doc["color_temp"] = bulb->getColorTemperature();
    }
    serializeJson(doc, buffer);
    String setTopic = Sprintf("%s/%s", roomsTopic.c_str(), slug.c_str());
    return pub(setTopic.c_str(), 0, true, buffer);
}

void Setup() {
    for (auto& led : leds)
        led->Setup();
}

void Loop() {
    for (auto& led : leds)
        led->Loop();
}

bool SendDiscovery() {
    for (auto& led : leds)
        if (!sendLightDiscovery(led->getName(), EC_NONE))
            return false;
    return true;
}

bool SendOnline() {
    if (online) return true;
    for (auto& led : leds)
        if (!SendState(led)) return false;
    online = true;
    return true;
}

void Connected(bool wifi, bool mqtt) {
    for (auto& led : status)
        led->setColor(wifi ? 128 : 0, 128, mqtt ? 128 : 0);
}

void Seen(bool inprogress) {
    for (auto& led : status)
        led->setState(inprogress);
}

void ConnectProgress() {
    for (auto& led : status)
        led->setColor(128, 0, 0);
}

void Update(bool inprogress) {
    for (auto& led : status)
        led->setColor(0, 128, 0);
}

void UpdateProgress(int percent) {
    for (auto& led : status)
        led->setState(percent % 2 == 0);
}

LED* findBulb(String& command) {
    for (auto& led : leds) {
        if (led->getId() == command) {
            return led;
        }
    }
    return NULL;
}

bool Command(String& command, String& pay) {
    auto bulb = findBulb(command);
    if (bulb == NULL) return false;
    DynamicJsonDocument root(pay.length() + 100);
    auto err = deserializeJson(root, pay);
    if (err) {
        Serial.printf("LEDs::Command: deserializeJson: 0x%x\n", err);
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

    if (sendNewState) SendState(bulb);
    return true;
}

}  // namespace LEDs
