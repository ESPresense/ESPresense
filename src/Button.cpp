#include "Button.h"

#include <AsyncMqttClient.h>
#include <AsyncWiFiSettings.h>

#include "GUI.h"
#include "defaults.h"
#include "globals.h"
#include "mqtt.h"
#include "string_utils.h"

namespace Button {
int8_t lastButtonValue = -1;

int8_t button_1Type, button_1Pin, button_1Detected = -1;
float button_1Timeout = 0;
int8_t lastbutton_1Value = -1;
unsigned long lastbutton_1Milli = 0;

int8_t button_2Type, button_2Pin, button_2Detected = -1;
float button_2Timeout = 0;
int8_t lastbutton_2Value = -1;
unsigned long lastbutton_2Milli = 0;
bool online;

void Setup() {
    std::vector<uint8_t> pinTypes = {INPUT_PULLUP, INPUT_PULLUP, INPUT_PULLDOWN, INPUT_PULLDOWN, INPUT, INPUT};
    if (button_1Pin >= 0) pinMode(button_1Pin, pinTypes[button_1Type]);
    if (button_2Pin >= 0) pinMode(button_2Pin, pinTypes[button_2Type]);
}

void ConnectToWifi() {
    std::vector<String> pinTypes = {"Pullup", "Pullup Inverted", "Pulldown", "Pulldown Inverted", "Floating", "Floating Inverted"};
    button_1Type = AsyncWiFiSettings.dropdown("button_1_type", pinTypes, 0, "Button One pin type");
    button_1Pin = AsyncWiFiSettings.integer("button_1_pin", -1, "Button One pin (-1 for disable)");
    button_1Timeout = AsyncWiFiSettings.floating("button_1_timeout", 0, 300, DEFAULT_DEBOUNCE_TIMEOUT, "Button One timeout (in seconds)");
    button_1Detected = button_1Type & 0x01 ? LOW : HIGH;

    button_2Type = AsyncWiFiSettings.dropdown("button_2_type", pinTypes, 0, "Button Two pin type");
    button_2Pin = AsyncWiFiSettings.integer("button_2_pin", -1, "Button Two pin (-1 for disable)");
    button_2Timeout = AsyncWiFiSettings.floating("button_2_timeout", 0, 300, DEFAULT_DEBOUNCE_TIMEOUT, "Button Two timeout (in seconds)");
    button_2Detected = button_2Type & 0x01 ? LOW : HIGH;
}

void SerialReport() {
    Serial.print("Button One:   ");
    Serial.println(button_1Pin >= 0 ? "enabled" : "disabled");
    Serial.print("Button Two:   ");
    Serial.println(button_2Pin >= 0 ? "enabled" : "disabled");
}

static void button_1Loop() {
    if (button_1Pin < 0) return;
    bool detected = digitalRead(button_1Pin) == button_1Detected;
    if (detected) lastbutton_1Milli = millis();
    unsigned long since = millis() - lastbutton_1Milli;
    int button_1Value = (detected || since < (button_1Timeout * 1000)) ? HIGH : LOW;

    if (lastbutton_1Value == button_1Value) return;
    pub((roomsTopic + "/button_1").c_str(), 0, true, button_1Value == HIGH ? "ON" : "OFF");
    lastbutton_1Value = button_1Value;
}

static void button_2Loop() {
    if (button_2Pin < 0) return;
    bool detected = digitalRead(button_2Pin) == button_2Detected;
    if (detected) lastbutton_2Milli = millis();
    unsigned long since = millis() - lastbutton_2Milli;
    int button_2Value = (detected || since < (button_2Timeout * 1000)) ? HIGH : LOW;

    if (lastbutton_2Value == button_2Value) return;
    pub((roomsTopic + "/button_2").c_str(), 0, true, button_2Value == HIGH ? "ON" : "OFF");
    lastbutton_2Value = button_2Value;
}

void Loop() {
    button_1Loop();
    button_2Loop();
    int ButtonValue = (lastbutton_2Value == HIGH || lastbutton_1Value == HIGH) ? HIGH : LOW;
    if (lastButtonValue == ButtonValue) return;
    GUI::Button(lastbutton_1Value == HIGH, lastbutton_2Value == HIGH);
    pub((roomsTopic + "/button").c_str(), 0, true, ButtonValue == HIGH ? "ON" : "OFF");
    lastButtonValue = ButtonValue;
}

bool SendDiscovery() {
    if (button_1Pin < 0 && button_2Pin < 0) return true;

    if (button_1Pin >= 0){
        if (!sendNumberDiscovery("button_1 Timeout", EC_CONFIG)) return false;
        sendSensorDiscovery("button_1", EC_NONE);
    }
    if (button_2Pin >= 0){
        if (!sendNumberDiscovery("button_2 Timeout", EC_CONFIG)) return false;
        sendSensorDiscovery("button_2", EC_NONE);
    }
    return sendSensorDiscovery("button", EC_NONE);
}

bool Command(String& command, String& pay) {
    if (command == "button_1_timeout") {
        button_1Timeout = pay.toInt();
        spurt("/button_1_timeout", pay);
    } else if (command == "button_2_timeout") {
        button_2Timeout = pay.toInt();
        spurt("/button_2_timeout", pay);
    } else
        return false;
    return true;
}

bool SendOnline() {
    if (online) return true;
    if (!pub((roomsTopic + "/button_1_timeout").c_str(), 0, true, String(button_1Timeout).c_str())) return false;
    if (!pub((roomsTopic + "/button_2_timeout").c_str(), 0, true, String(button_2Timeout).c_str())) return false;
    online = true;
    return true;
}
}  // namespace Button
