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

int8_t switchoneType, switchonePin, switchoneDetected = -1;
float switchoneTimeout = 0;
int8_t lastswitchoneValue = -1;
unsigned long lastswitchoneMilli = 0;

int8_t switchtwoType, switchtwoPin, switchtwoDetected = -1;
float switchtwoTimeout = 0;
int8_t lastswitchtwoValue = -1;
unsigned long lastswitchtwoMilli = 0;
bool online;

void Setup() {
    std::vector<uint8_t> pinTypes = {INPUT_PULLUP, INPUT_PULLUP, INPUT_PULLDOWN, INPUT_PULLDOWN, INPUT, INPUT};
    if (switchonePin >= 0) pinMode(switchonePin, pinTypes[switchoneType]);
    if (switchtwoPin >= 0) pinMode(switchtwoPin, pinTypes[switchtwoType]);
}

void ConnectToWifi() {
    AsyncWiFiSettings.html("h4", "Switch One:");
    std::vector<String> pinTypes = {"Pullup", "Pullup Inverted", "Pulldown", "Pulldown Inverted", "Floating", "Floating Inverted"};
    switchoneType = AsyncWiFiSettings.dropdown("switchone_type", pinTypes, 0, "Switch One pin type");
    switchonePin = AsyncWiFiSettings.integer("switchone_pin", -1, "Switch One pin (-1 for disable)");
    switchoneTimeout = AsyncWiFiSettings.floating("switchone_timeout", 0, 300, DEFAULT_DEBOUNCE_TIMEOUT, "Switch One timeout (in seconds)");
    switchoneDetected = switchoneType & 0x01 ? LOW : HIGH;

    AsyncWiFiSettings.html("h4", "Switch Two:");
    switchtwoType = AsyncWiFiSettings.dropdown("switchtwo_type", pinTypes, 0, "Switch Two pin type");
    switchtwoPin = AsyncWiFiSettings.integer("switchtwo_pin", -1, "Switch Two pin (-1 for disable)");
    switchtwoTimeout = AsyncWiFiSettings.floating("switchtwo_timeout", 0, 300, DEFAULT_DEBOUNCE_TIMEOUT, "Switch Two timeout (in seconds)");
    switchtwoDetected = switchtwoType & 0x01 ? LOW : HIGH;
}

void SerialReport() {
    Serial.print("Switch One Sensor:   ");
    Serial.println(switchonePin >= 0 ? "enabled" : "disabled");
    Serial.print("Switch Two Sensor: ");
    Serial.println(switchtwoPin >= 0 ? "enabled" : "disabled");
}

static void switchoneLoop() {
    if (switchonePin < 0) return;
    bool detected = digitalRead(switchonePin) == switchoneDetected;
    if (detected) lastswitchoneMilli = millis();
    unsigned long since = millis() - lastswitchoneMilli;
    int switchoneValue = (detected || since < (switchoneTimeout * 1000)) ? HIGH : LOW;

    if (lastswitchoneValue == switchoneValue) return;
    pub((roomsTopic + "/switchone").c_str(), 0, true, switchoneValue == HIGH ? "ON" : "OFF");
    lastswitchoneValue = switchoneValue;
}

static void switchtwoLoop() {
    if (switchtwoPin < 0) return;
    bool detected = digitalRead(switchtwoPin) == switchtwoDetected;
    if (detected) lastswitchtwoMilli = millis();
    unsigned long since = millis() - lastswitchtwoMilli;
    int switchtwoValue = (detected || since < (switchtwoTimeout * 1000)) ? HIGH : LOW;

    if (lastswitchtwoValue == switchtwoValue) return;
    pub((roomsTopic + "/switchtwo").c_str(), 0, true, switchtwoValue == HIGH ? "ON" : "OFF");
    lastswitchtwoValue = switchtwoValue;
}

void Loop() {
    switchoneLoop();
    switchtwoLoop();
    int SwitchValue = (lastswitchtwoValue == HIGH || lastswitchoneValue == HIGH) ? HIGH : LOW;
    if (lastSwitchValue == SwitchValue) return;
    GUI::Switch(lastswitchoneValue == HIGH, lastswitchtwoValue == HIGH);
    pub((roomsTopic + "/switch").c_str(), 0, true, SwitchValue == HIGH ? "ON" : "OFF");
    lastSwitchValue = SwitchValue;
}

bool SendDiscovery() {
    if (switchonePin < 0 && switchtwoPin < 0) return true;

    if (switchonePin >= 0)
        if (!sendNumberDiscovery("switchone Timeout", EC_CONFIG)) return false;
    if (switchtwoPin >= 0)
        if (!sendNumberDiscovery("switchtwo Timeout", EC_CONFIG)) return false;
    return sendBinarySensorDiscovery("switch", EC_NONE, "switch");
}

bool Command(String& command, String& pay) {
    if (command == "switchone_timeout") {
        switchoneTimeout = pay.toInt();
        spurt("/switchone_timeout", pay);
    } else if (command == "switchtwo_timeout") {
        switchtwoTimeout = pay.toInt();
        spurt("/switchtwo_timeout", pay);
    } else
        return false;
    return true;
}

bool SendOnline() {
    if (online) return true;
    if (!pub((roomsTopic + "/switchone_timeout").c_str(), 0, true, String(switchoneTimeout).c_str())) return false;
    if (!pub((roomsTopic + "/switchtwo_timeout").c_str(), 0, true, String(switchtwoTimeout).c_str())) return false;
    online = true;
    return true;
}
}  // namespace Switch