#include "Switch.h"

#include <AsyncMqttClient.h>
#include <HeadlessWiFiSettings.h>

#include "GUI.h"
#include "defaults.h"
#include "globals.h"
#include "mqtt.h"
#include "string_utils.h"

namespace Switch {
int8_t lastSwitchValue = -1;

int8_t switch_1Type, switch_1Pin, switch_1Detected = -1;
int8_t switch_1Mode = 0;  // 0 = Input (binary sensor), 1 = Output (switch)
float switch_1Timeout = 0;
int8_t lastswitch_1Value = -1;
unsigned long lastswitch_1Milli = 0;

int8_t switch_2Type, switch_2Pin, switch_2Detected = -1;
int8_t switch_2Mode = 0;  // 0 = Input (binary sensor), 1 = Output (switch)
float switch_2Timeout = 0;
int8_t lastswitch_2Value = -1;
unsigned long lastswitch_2Milli = 0;
bool online;

void Setup() {
    std::vector<uint8_t> pinTypes = {INPUT_PULLUP, INPUT_PULLUP, INPUT_PULLDOWN, INPUT_PULLDOWN, INPUT, INPUT};
    if (switch_1Pin >= 0) {
        if (switch_1Mode == 1) {
            pinMode(switch_1Pin, OUTPUT);
            digitalWrite(switch_1Pin, lastswitch_1Value == HIGH ? HIGH : LOW);
        } else {
            pinMode(switch_1Pin, pinTypes[switch_1Type]);
        }
    }
    if (switch_2Pin >= 0) {
        if (switch_2Mode == 1) {
            pinMode(switch_2Pin, OUTPUT);
            digitalWrite(switch_2Pin, lastswitch_2Value == HIGH ? HIGH : LOW);
        } else {
            pinMode(switch_2Pin, pinTypes[switch_2Type]);
        }
    }
}

/**
 * @brief Load switch configuration from persisted Wi-Fi settings and derive detection levels.
 *
 * Reads configured pin type, pin number, and timeout for both Switch One and Switch Two from
 * HeadlessWiFiSettings, storing values into the corresponding module variables:
 * `switch_1Type`, `switch_1Pin`, `switch_1Timeout`, `switch_1Detected` and
 * `switch_2Type`, `switch_2Pin`, `switch_2Timeout`, `switch_2Detected`.
 *
 * The available pin type options presented are: "Pullup", "Pullup Inverted", "Pulldown",
 * "Pulldown Inverted", "Floating", and "Floating Inverted". A pin value of -1 disables the
 * corresponding switch. Timeouts are specified in seconds and default to DEFAULT_DEBOUNCE_TIMEOUT.
 *
 * The detected logic level for each switch is derived from the selected type: if the type's
 * least-significant bit is set, the detection level is `LOW`; otherwise it is `HIGH`.
 */
void ConnectToWifi() {
    std::vector<String> pinTypes = {"Pullup", "Pullup Inverted", "Pulldown", "Pulldown Inverted", "Floating", "Floating Inverted"};
    std::vector<String> switchModes = {"Input (Binary Sensor)", "Output (Switch)"};
    
    switch_1Type = HeadlessWiFiSettings.dropdown("switch_1_type", pinTypes, 0, "Switch One pin type");
    switch_1Pin = HeadlessWiFiSettings.integer("switch_1_pin", -1, "Switch One pin (-1 for disable)");
    switch_1Mode = HeadlessWiFiSettings.dropdown("switch_1_mode", switchModes, 0, "Switch One mode");
    switch_1Timeout = HeadlessWiFiSettings.floating("switch_1_timeout", 0, 300, DEFAULT_DEBOUNCE_TIMEOUT, "Switch One timeout (in seconds)");
    switch_1Detected = switch_1Type & 0x01 ? LOW : HIGH;
    
    // Load persisted state for output mode
    if (switch_1Mode == 1) {
        String switch_1State = HeadlessWiFiSettings.string("switch_1_state", true, "Switch One state");
        lastswitch_1Value = (switch_1State == "ON") ? HIGH : LOW;
    }

    switch_2Type = HeadlessWiFiSettings.dropdown("switch_2_type", pinTypes, 0, "Switch Two pin type");
    switch_2Pin = HeadlessWiFiSettings.integer("switch_2_pin", -1, "Switch Two pin (-1 for disable)");
    switch_2Mode = HeadlessWiFiSettings.dropdown("switch_2_mode", switchModes, 0, "Switch Two mode");
    switch_2Timeout = HeadlessWiFiSettings.floating("switch_2_timeout", 0, 300, DEFAULT_DEBOUNCE_TIMEOUT, "Switch Two timeout (in seconds)");
    switch_2Detected = switch_2Type & 0x01 ? LOW : HIGH;
    
    // Load persisted state for output mode
    if (switch_2Mode == 1) {
        String switch_2State = HeadlessWiFiSettings.string("switch_2_state", true, "Switch Two state");
        lastswitch_2Value = (switch_2State == "ON") ? HIGH : LOW;
    }
}

/**
 * @brief Logs the enabled/disabled status of both configured switches.
 *
 * Writes a status line for Switch One and Switch Two to the logging facility,
 * indicating "enabled" when the configured pin is >= 0 and "disabled" otherwise.
 */
void SerialReport() {
    Log.print("Switch One:   ");
    Log.println(switch_1Pin >= 0 ? "enabled" : "disabled");
    Log.print("Switch Two:   ");
    Log.println(switch_2Pin >= 0 ? "enabled" : "disabled");
}

/**
 * @brief Monitors Switch 1 input and publishes state changes.
 *
 * Reads the configured input pin for Switch 1 and treats the configured detection
 * level as the active state. When the input becomes active, updates the last
 * change timestamp and considers the switch ON while the hold window defined by
 * switch_1Timeout (seconds) has not elapsed. When the computed switch state
 * differs from the previously published state, publishes "ON" or "OFF" to
 * roomsTopic + "/switch_1" and updates lastswitch_1Value.
 *
 * If switch_1Pin is negative, the function returns without action.
 */
static void switch_1Loop() {
    if (switch_1Pin < 0) return;
    
    // Output mode: GPIO is controlled by MQTT commands, just publish current state
    if (switch_1Mode == 1) {
        // No need to read input or update state here
        return;
    }
    
    // Input mode: Read GPIO and publish state changes
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
    
    // Output mode: GPIO is controlled by MQTT commands, just publish current state
    if (switch_2Mode == 1) {
        // No need to read input or update state here
        return;
    }
    
    // Input mode: Read GPIO and publish state changes
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
        // Use switch discovery for output mode, binary sensor for input mode
        if (switch_1Mode == 1) {
            sendSwitchDiscovery("switch_1", EC_NONE);
        } else {
            sendBinarySensorDiscovery("switch_1", EC_NONE);
        }
    }
    if (switch_2Pin >= 0){
        if (!sendNumberDiscovery("switch_2 Timeout", EC_CONFIG)) return false;
        // Use switch discovery for output mode, binary sensor for input mode
        if (switch_2Mode == 1) {
            sendSwitchDiscovery("switch_2", EC_NONE);
        } else {
            sendBinarySensorDiscovery("switch_2", EC_NONE);
        }
    }
    // Combined switch entity - use binary sensor (read-only)
    return sendBinarySensorDiscovery("switch", EC_NONE);
}

bool Command(String& command, String& pay) {
    if (command == "switch_1_timeout") {
        switch_1Timeout = pay.toInt();
        spurt("/switch_1_timeout", pay);
    } else if (command == "switch_2_timeout") {
        switch_2Timeout = pay.toInt();
        spurt("/switch_2_timeout", pay);
    } else if (command == "switch_1" && switch_1Mode == 1 && switch_1Pin >= 0) {
        // Handle switch_1 ON/OFF commands for output mode
        int newValue = (pay == "ON") ? HIGH : LOW;
        digitalWrite(switch_1Pin, newValue);
        lastswitch_1Value = newValue;
        pub((roomsTopic + "/switch_1").c_str(), 0, true, newValue == HIGH ? "ON" : "OFF");
        spurt("/switch_1_state", pay);  // Persist state
    } else if (command == "switch_2" && switch_2Mode == 1 && switch_2Pin >= 0) {
        // Handle switch_2 ON/OFF commands for output mode
        int newValue = (pay == "ON") ? HIGH : LOW;
        digitalWrite(switch_2Pin, newValue);
        lastswitch_2Value = newValue;
        pub((roomsTopic + "/switch_2").c_str(), 0, true, newValue == HIGH ? "ON" : "OFF");
        spurt("/switch_2_state", pay);  // Persist state
    } else
        return false;
    return true;
}

bool SendOnline() {
    if (online) return true;
    if (!pub((roomsTopic + "/switch_1_timeout").c_str(), 0, true, String(switch_1Timeout).c_str())) return false;
    if (!pub((roomsTopic + "/switch_2_timeout").c_str(), 0, true, String(switch_2Timeout).c_str())) return false;
    
    // Publish initial state for output mode switches
    if (switch_1Mode == 1 && switch_1Pin >= 0) {
        if (!pub((roomsTopic + "/switch_1").c_str(), 0, true, lastswitch_1Value == HIGH ? "ON" : "OFF")) return false;
    }
    if (switch_2Mode == 1 && switch_2Pin >= 0) {
        if (!pub((roomsTopic + "/switch_2").c_str(), 0, true, lastswitch_2Value == HIGH ? "ON" : "OFF")) return false;
    }
    
    online = true;
    return true;
}
}  // namespace Switch