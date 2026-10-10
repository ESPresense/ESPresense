#include "InputGroup.h"

#include "Settings.h"
#include "defaults.h"
#include "globals.h"
#include "mqtt.h"
#include "string_utils.h"
#include "util.h"

InputGroup::InputGroup(const char* prefix, const char* label, const char* plural, int defaultCount)
    : prefix(prefix), label(label), plural(plural), defaultCount(defaultCount) {}

std::string InputGroup::key(int n, const char* field) const { return Sprintf("%s_%d_%s", prefix.c_str(), n, field); }

/**
 * @brief Register the count and the per-input settings, and derive each input's detection level.
 *
 * Pin types are "Pullup", "Pullup Inverted", "Pulldown", "Pulldown Inverted", "Floating" and
 * "Floating Inverted"; an odd type detects on LOW, an even one on HIGH. A pin of -1 disables
 * the input. Timeouts are 0-300 seconds.
 */
void InputGroup::connectToWifi() {
    std::vector<std::string> pinTypes = {"Pullup", "Pullup Inverted", "Pulldown", "Pulldown Inverted", "Floating", "Floating Inverted"};
    int n = Settings::integer(prefix + "_count", 0, MAX, defaultCount, "Number of " + plural);
    if (n < 0) n = 0;
    if (n > MAX) n = MAX;
    Settings::group(prefix, MAX, {"type", "pin", "timeout"});
    inputs.clear();
    for (int i = 1; i <= n; i++) {
        Input in;
        in.index = i;
        in.type = Settings::dropdown(key(i, "type"), pinTypes, 0, Sprintf("%s %d pin type", label.c_str(), i));
        in.pin = Settings::integer(key(i, "pin"), -1, 48, -1, Sprintf("%s %d pin (-1 for disable)", label.c_str(), i));
        in.timeout = Settings::floating(key(i, "timeout"), 0, 300, DEFAULT_DEBOUNCE_TIMEOUT, Sprintf("%s %d timeout (in seconds)", label.c_str(), i));
        in.detected = in.type & 0x01 ? LOW : HIGH;
        inputs.push_back(in);
    }
}

void InputGroup::setup() {
    static const PinMode modes[] = {INPUT_PULLUP, INPUT_PULLUP, INPUT_PULLDOWN, INPUT_PULLDOWN, INPUT, INPUT};
    for (auto& in : inputs)
        if (in.pin >= 0 && in.type >= 0 && in.type < 6) pinMode(in.pin, modes[in.type]);
}

void InputGroup::serialReport() {
    for (auto& in : inputs) {
        auto name = Sprintf("%s %d:", label.c_str(), in.index);
        Log.printf("%-14s%s\n", name.c_str(), in.pin >= 0 ? "enabled" : "disabled");
    }
}

bool InputGroup::loop() {
    for (auto& in : inputs) {
        if (in.pin < 0) continue;
        bool detected = digitalRead(in.pin) == in.detected;
        if (detected) in.lastMillis = millis();
        unsigned long since = millis() - in.lastMillis;
        int8_t value = (detected || since < (in.timeout * 1000)) ? HIGH : LOW;
        if (in.last == value) continue;
        pub((roomsTopic + "/" + Sprintf("%s_%d", prefix.c_str(), in.index)).c_str(), 0, true, value == HIGH ? "ON" : "OFF");
        in.last = value;
    }
    int8_t combined = mask() ? HIGH : LOW;
    if (lastCombined == combined) return false;
    pub((roomsTopic + "/" + prefix).c_str(), 0, true, combined == HIGH ? "ON" : "OFF");
    lastCombined = combined;
    return true;
}

bool InputGroup::sendDiscovery() {
    bool any = false;
    for (auto& in : inputs) {
        if (in.pin < 0) continue;
        any = true;
        auto name = Sprintf("%s_%d", prefix.c_str(), in.index);
        if (!sendNumberDiscovery(name + " Timeout", EC_CONFIG)) return false;
        sendSensorDiscovery(name, EC_NONE);
    }
    return !any || sendSensorDiscovery(prefix, EC_NONE);
}

bool InputGroup::sendOnline() {
    if (online) return true;
    for (auto& in : inputs)
        if (!pub((roomsTopic + "/" + key(in.index, "timeout")).c_str(), 0, true, toStr(in.timeout).c_str())) return false;
    online = true;
    return true;
}

bool InputGroup::command(const std::string& command, const std::string& pay) {
    for (auto& in : inputs)
        if (command == key(in.index, "timeout")) {
            in.timeout = toFloat(pay);
            spurt("/" + command, pay);
            return true;
        }
    return false;
}

int8_t InputGroup::value(int n) const {
    for (auto& in : inputs)
        if (in.index == n) return in.last;
    return -1;
}

uint32_t InputGroup::mask() const {
    uint32_t m = 0;
    for (auto& in : inputs)
        if (in.last == HIGH) m |= 1u << (in.index - 1);
    return m;
}
