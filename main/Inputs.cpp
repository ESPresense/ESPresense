#include "Inputs.h"

#include <vector>

#include "GUI.h"
#include "Settings.h"
#include "defaults.h"
#include "globals.h"
#include "mqtt.h"
#include "string_utils.h"
#include "util.h"

namespace Inputs {
namespace {
struct Input {
    int index = 0;
    std::string name;
    Role role = Role::Motion;
    int8_t pin = -1, type = 0;
    bool inverted = false;
    float timeout = 0;
    int8_t last = -1;
    unsigned long lastMillis = 0;

    std::string id() const { return Sprintf("input_%d", index); }
    std::string timeoutId() const { return Sprintf("input_%d_timeout", index); }
};

std::vector<Input> inputs;

std::string key(int n, const char* field) { return Sprintf("input_%d_%s", n, field); }
}  // namespace

void ConnectToWifi(bool updating) {
    std::vector<std::string> roles = {"Motion", "Switch", "Button"};
    // Odd types are inverted: active LOW.
    std::vector<std::string> pinTypes = {"Pullup", "Pullup Inverted", "Pulldown", "Pulldown Inverted", "Floating", "Floating Inverted"};
    int count = Settings::integer("input_count", 0, MAX, 0, "Number of inputs");
    if (count < 0) count = 0;
    if (count > MAX) count = MAX;
    Settings::group("input", MAX, {"name", "role", "pin", "type", "timeout"});
    inputs.clear();
    for (int n = 1; n <= count; n++) {
        Input in;
        in.index = n;
        in.name = Settings::string(key(n, "name"), Sprintf("Input %d", n), "Name");
        in.role = (Role)Settings::dropdown(key(n, "role"), roles, 0, "Role");
        in.pin = Settings::integer(key(n, "pin"), -1, 48, -1, "Pin (-1 to disable)");
        in.type = Settings::dropdown(key(n, "type"), pinTypes, 0, "Pin type");
        in.inverted = in.type & 1;
        in.timeout = Settings::floating(key(n, "timeout"), 0, 300, DEFAULT_DEBOUNCE_TIMEOUT, "Timeout (in seconds)");
        if (in.name.empty()) in.name = Sprintf("Input %d", n);
        inputs.push_back(in);
    }
}

void Setup() {
    static const PinMode modes[] = {INPUT_PULLUP, INPUT_PULLDOWN, INPUT};
    for (auto& in : inputs)
        if (in.pin >= 0 && in.type >= 0 && in.type < 6) pinMode(in.pin, modes[in.type / 2]);
}

void SerialReport() {
    for (auto& in : inputs) {
        Log.printf("Input %d:      ", in.index);
        if (in.pin < 0)
            Log.println("disabled");
        else
            Log.printf("%s, pin %d%s\n", in.name.c_str(), in.pin, in.inverted ? " inverted" : "");
    }
}

void Loop() {
    bool motionChanged = false;
    for (auto& in : inputs) {
        if (in.pin < 0) continue;
        // Active HIGH unless inverted (pull-up wiring to ground reads LOW when closed).
        bool detected = (digitalRead(in.pin) == HIGH) != in.inverted;
        if (detected) in.lastMillis = millis();
        unsigned long since = millis() - in.lastMillis;
        int8_t value = (detected || since < (in.timeout * 1000)) ? HIGH : LOW;
        if (in.last == value) continue;
        pub((roomsTopic + "/" + in.id()).c_str(), 0, true, value == HIGH ? "ON" : "OFF");
        in.last = value;
        GUI::Input(in.index, in.name, value == HIGH);
        if (in.role == Role::Motion) motionChanged = true;
    }
    if (!motionChanged) return;
    bool motion = false;
    for (auto& in : inputs)
        if (in.role == Role::Motion && in.last == HIGH) motion = true;
    GUI::Motion(motion);
}

// Slots above input_count, or without a pin, delete their entities so a removed input doesn't
// linger in Home Assistant.
bool SendDiscovery() {
    for (int n = 1; n <= MAX; n++) {
        auto id = Sprintf("input_%d", n);
        const Input* in = nullptr;
        for (auto& i : inputs)
            if (i.index == n && i.pin >= 0) in = &i;
        if (!in) {
            if (!sendDeleteDiscovery("binary_sensor", id) || !sendDeleteDiscovery("number", id + "_timeout")) return false;
            continue;
        }
        if (!sendBinarySensorDiscovery(id, in->name, EC_NONE, in->role == Role::Motion ? "motion" : DEVICE_CLASS_NONE)) return false;
        if (!sendNumberDiscovery(in->timeoutId(), in->name + " Timeout", EC_CONFIG)) return false;
    }
    return true;
}

// On every (re)connect: the first reads usually happen before MQTT is up, so their publish is lost.
bool SendOnline() {
    for (auto& in : inputs) {
        if (in.pin < 0) continue;
        if (!pub((roomsTopic + "/" + in.timeoutId()).c_str(), 0, true, toStr(in.timeout).c_str())) return false;
        if (in.last >= 0 && !pub((roomsTopic + "/" + in.id()).c_str(), 0, true, in.last == HIGH ? "ON" : "OFF")) return false;
    }
    return true;
}

bool Command(std::string& command, std::string& pay) {
    for (auto& in : inputs)
        if (command == in.timeoutId()) {
            in.timeout = toFloat(pay);
            spurt("/" + command, pay);
            return true;
        }
    return false;
}

int8_t Value(int n) {
    for (auto& in : inputs)
        if (in.index == n) return in.last;
    return -1;
}
Role GetRole(int n) {
    for (auto& in : inputs)
        if (in.index == n) return in.role;
    return Role::Motion;
}
}  // namespace Inputs
