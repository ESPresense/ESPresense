#include "Inputs.h"

#include <cstring>
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
    int8_t pin = -1, pull = 0;  // pull: 0 up, 1 down, 2 floating
    bool inverted = false;
    float timeout = 0;
    int8_t last = -1;
    unsigned long lastMillis = 0;

    std::string id() const { return Sprintf("input_%d", index); }
};

std::vector<Input> inputs;

int indexOf(const char* s, std::initializer_list<const char*> options, int fallback) {
    int i = 0;
    for (auto* o : options) {
        if (s && strcmp(s, o) == 0) return i;
        i++;
    }
    return fallback;
}
}  // namespace

void ConnectToWifi(bool updating) {
    std::string text = Settings::json("inputs", "[]", "Inputs");
    DynamicJsonDocument list(text.size() * 2 + 256);
    inputs.clear();
    if (deserializeJson(list, text)) {
        Log.println("Inputs: invalid JSON, ignored");
        return;
    }
    for (JsonObject o : list.as<JsonArray>()) {
        if ((int)inputs.size() >= MAX) break;
        Input in;
        in.index = inputs.size() + 1;
        in.name = o["name"] | "";
        if (in.name.empty()) in.name = Sprintf("Input %d", in.index);
        in.role = (Role)indexOf(o["role"], {"motion", "switch", "button"}, 0);
        in.pin = o["pin"] | -1;
        std::string type = o["type"] | "pullup";
        in.inverted = endsWith(type, "_inverted");
        if (in.inverted) type.resize(type.size() - 9);
        in.pull = indexOf(type.c_str(), {"pullup", "pulldown", "floating"}, 0);
        in.timeout = o["timeout"] | (float)DEFAULT_DEBOUNCE_TIMEOUT;
        inputs.push_back(in);
    }
}

void Setup() {
    static const PinMode modes[] = {INPUT_PULLUP, INPUT_PULLDOWN, INPUT};
    for (auto& in : inputs)
        if (in.pin >= 0) pinMode(in.pin, modes[in.pull]);
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

// Removed inputs are cleaned up by Mqtt::PruneStaleDiscovery.
bool SendDiscovery() {
    for (auto& in : inputs)
        if (in.pin >= 0 && !sendBinarySensorDiscovery(in.id(), in.name, EC_NONE, in.role == Role::Motion ? "motion" : DEVICE_CLASS_NONE))
            return false;
    return true;
}

// On every (re)connect: the first reads usually happen before MQTT is up, so their publish is lost.
bool SendOnline() {
    for (auto& in : inputs)
        if (in.pin >= 0 && in.last >= 0 && !pub((roomsTopic + "/" + in.id()).c_str(), 0, true, in.last == HIGH ? "ON" : "OFF"))
            return false;
    return true;
}

int8_t Value(int n) { return n >= 1 && n <= (int)inputs.size() ? inputs[n - 1].last : -1; }

Role GetRole(int n) { return n >= 1 && n <= (int)inputs.size() ? inputs[n - 1].role : Role::Motion; }
}  // namespace Inputs
