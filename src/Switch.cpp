#include "Switch.h"

#include <AsyncMqttClient.h>
#include <AsyncWiFiSettings.h>

#include "GUI.h"
#include "defaults.h"
#include "globals.h"
#include "mqtt.h"
#include "string_utils.h"

namespace Switch {
int8_t lastSwitchValue = -1;

int8_t switch_1Type, switch_1Pin, switch_1Detected = -1;
float switch_1Timeout = 0;
int8_t lastswitch_1Value = -1;
unsigned long lastswitch_1Milli = 0;

int8_t switch_2Type, switch_2Pin, switch_2Detected = -1;
float switch_2Timeout = 0;
int8_t lastswitch_2Value = -1;
unsigned long lastswitch_2Milli = 0;
bool online;

void Setup() {
    std::vector<uint8_t> pinTypes = {INPUT_PULLUP, INPUT_PULLUP, INPUT_PULLDOWN, INPUT_PULLDOWN, INPUT, INPUT};
    if (switch_1Pin >= 0) pinMode(switch_1Pin, pinTypes[switch_1Type]);
    if (switch_2Pin >= 0) pinMode(switch_2Pin, pinTypes[switch_2Type]);
}

void ConnectToWifi() {
    std::vector<String> pinTypes = {"Pullup", "Pullup Inverted", "Pulldown", "Pulldown Inverted", "Floating", "Floating Inverted"};
    switch_1Type = AsyncWiFiSettings.dropdown("switch_1_type", pinTypes, 0, "Switch One pin type");
    switch_1Pin = AsyncWiFiSettings.integer("switch_1_pin", -1, "Switch One pin (-1 for disable)");
    switch_1Timeout = AsyncWiFiSettings.floating("switch_1_timeout", 0, 300, DEFAULT_DEBOUNCE_TIMEOUT, "Switch One timeout (in seconds)");
    switch_1Detected = switch_1Type & 0x01 ? LOW : HIGH;

    switch_2Type = AsyncWiFiSettings.dropdown("switch_2_type", pinTypes, 0, "Switch Two pin type");
    switch_2Pin = AsyncWiFiSettings.integer("switch_2_pin", -1, "Switch Two pin (-1 for disable)");
    switch_2Timeout = AsyncWiFiSettings.floating("switch_2_timeout", 0, 300, DEFAULT_DEBOUNCE_TIMEOUT, "Switch Two timeout (in seconds)");
    switch_2Detected = switch_2Type & 0x01 ? LOW : HIGH;
}

void SerialReport() {
    Serial.print("Switch One:   ");
    Serial.println(switch_1Pin >= 0 ? "enabled" : "disabled");
    Serial.print("Switch Two:   ");
    Serial.println(switch_2Pin >= 0 ? "enabled" : "disabled");
}

static void switch_1Loop() {
    if (switch_1Pin < 0) return;
    bool detected = digitalRead(switch_1Pin) == switch_1Detected;
    if (detected) lastswitch_1Milli = millis();
    unsigned long since = millis() - lastswitch_1Milli;
    int switch_1Value = (detected || since < (switch_1Timeout * 1000)) ? HIGH : LOW;

    if (lastswitch_1Value == switch_1Value) return;
    pub((roomsTopic + "/switch_1").c_str(), 0, true, switch_1Value == HIGH ? "ON" : "OFF");
    lastswitch_1Value = switch_1Value;
}

static void switch_2Loop() {
    if (switch_2Pin < 0) return;
    bool detected = digitalRead(switch_2Pin) == switch_2Detected;
    if (detected) lastswitch_2Milli = millis();
    unsigned long since = millis() - lastswitch_2Milli;
    int switch_2Value = (detected || since < (switch_2Timeout * 1000)) ? HIGH : LOW;

    if (lastswitch_2Value == switch_2Value) return;
    pub((roomsTopic + "/switch_2").c_str(), 0, true, switch_2Value == HIGH ? "ON" : "OFF");
    lastswitch_2Value = switch_2Value;
}

void Loop() {
    switch_1Loop();
    switch_2Loop();
    int SwitchValue = (lastswitch_2Value == HIGH || lastswitch_1Value == HIGH) ? HIGH : LOW;
    if (lastSwitchValue == SwitchValue) return;
    GUI::Switch(lastswitch_1Value == HIGH, lastswitch_2Value == HIGH);
    pub((roomsTopic + "/switch").c_str(), 0, true, SwitchValue == HIGH ? "ON" : "OFF");
    lastSwitchValue = SwitchValue;
}

bool SendDiscovery() {
    if (switch_1Pin < 0 && switch_2Pin < 0) return true;

    if (switch_1Pin >= 0){
        if (!sendNumberDiscovery("switch_1 Timeout", EC_CONFIG)) return false;
        sendSensorDiscovery("switch_1", EC_NONE);
    }
    if (switch_2Pin >= 0){
        if (!sendNumberDiscovery("switch_2 Timeout", EC_CONFIG)) return false;
        sendSensorDiscovery("switch_2", EC_NONE);
    }
    return sendSensorDiscovery("switch", EC_NONE);
}

bool Command(String& command, String& pay) {
    if (command == "switch_1_timeout") {
        switch_1Timeout = pay.toInt();
        spurt("/switch_1_timeout", pay);
    } else if (command == "switch_2_timeout") {
        switch_2Timeout = pay.toInt();
        spurt("/switch_2_timeout", pay);
    } else
        return false;
    return true;
}

bool SendOnline() {
    if (online) return true;
    if (!pub((roomsTopic + "/switch_1_timeout").c_str(), 0, true, String(switch_1Timeout).c_str())) return false;
    if (!pub((roomsTopic + "/switch_2_timeout").c_str(), 0, true, String(switch_2Timeout).c_str())) return false;
    online = true;
    return true;
}
}  // namespace Switch
