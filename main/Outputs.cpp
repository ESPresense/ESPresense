#include "Outputs.h"

#include <algorithm>
#include <atomic>
#include <vector>

#include "Inputs.h"
#include "OutputLogic.h"
#include "Settings.h"
#include "globals.h"
#include "mqtt.h"
#include "string_utils.h"
#include "util.h"

// Defined in main.cpp: set once status=online (and the retained states) have gone out.
extern bool online;

// Plain on/off outputs (relays, smart plug / Shelly outlets): the "outputs" setting on the
// hardware endpoint, a JSON list such as
//   [{"name": "Relay", "pin": 5, "type": "output", "power_on": "restore", "input": 1}]
// type: output | output_inverted (Home Assistant switch: state on <room>/output_N, commands
// ON/OFF/TOGGLE on <room>/output_N/set), or high | low: a fixed level set at boot with no MQTT,
// for enable pins (LED power, transceiver standby). power_on: off | on | restore.
// input: 1-based input number; a button input toggles the output, any other input drives it.
namespace Outputs {

struct Output {
    int index = 0;
    int pin = -1;
    std::string name;
    bool inverted = false;
    bool fixed = false;            // high/low: driven once at boot, not on MQTT
    OutputPowerOn powerOn = OutputPowerOn::Off;
    int input = 0;                 // 0 none, n: input n drives this output (Button toggles, others follow)
    std::atomic<bool> state{false};  // written by the MQTT task (Command) and the main loop (button)
    int8_t published = -1;         // last state sent to MQTT; main task only
    int8_t saved = -1;             // last state written to /output_N_last; main task only
    int8_t lastInput = -1;

    std::string id() const { return Sprintf("output_%d", index); }
    std::string lastFilename() const { return Sprintf("/output_%d_last", index); }

    void set(bool on) {
        state = on;
        if (pin >= 0) digitalWrite(pin, outputLevel(on, inverted));
    }
};

// Sized once per boot to the configured count: Output holds an atomic, so it can't be moved, and
// a vector built at its final size never moves its elements.
std::vector<Output> outputs;
int count = 0;

void Setup() {
}

/**
 * @brief Register output settings on the hardware endpoint and drive each enabled output to its power-on state.
 *
 * The output is set here rather than in Setup() so an outlet comes back as soon as settings
 * are read, not after the WiFi connect (or captive portal) that sits between the two.
 * The level is written before the pin becomes an output, so there is no glitch to the wrong state.
 */
void ConnectToWifi(bool updating) {
    std::string text = Settings::json("outputs", "[]", "Outputs");
    DynamicJsonDocument list(text.size() * 2 + 256);
    count = 0;
    outputs.clear();
    if (deserializeJson(list, text)) {
        Log.println("Outputs: invalid JSON, ignored");
        return;
    }
    JsonArray items = list.as<JsonArray>();
    outputs = std::vector<Output>(std::min<size_t>(items.size(), MAX));
    for (JsonObject j : items) {
        if (count >= (int)outputs.size()) break;
        auto& o = outputs[count++];
        o.index = count;
        o.name = j["name"] | "";
        if (o.name.empty()) o.name = Sprintf("Output %d", o.index);
        o.pin = j["pin"] | -1;
        std::string type = j["type"] | "output";
        o.inverted = type == "output_inverted";
        o.fixed = type == "high" || type == "low";
        std::string powerOn = j["power_on"] | "off";
        o.powerOn = powerOn == "on" ? OutputPowerOn::On : powerOn == "restore" ? OutputPowerOn::Restore : OutputPowerOn::Off;
        o.input = o.fixed ? 0 : (j["input"] | 0);
        if (o.pin < 0) continue;

        if (o.fixed) {
            o.set(type == "high");
        } else {
            std::string last = Settings::slurp(o.lastFilename());
            o.saved = last.empty() ? -1 : (last == "1");
            o.set(outputPowerOnState(o.powerOn, last));
        }
        pinMode(o.pin, OUTPUT);
    }
}

void SerialReport() {
    for (int i = 0; i < count; i++) {
        auto& r = outputs[i];
        Log.printf("Output %d:     ", r.index);
        if (r.pin < 0)
            Log.println("disabled");
        else
            Log.printf("%s, pin %d%s, %s%s\n", r.name.c_str(), r.pin, r.inverted ? " inverted" : "", r.state ? "on" : "off", r.fixed ? " (fixed)" : "");
    }
}

// Shown in Home Assistant and driven over MQTT: has a pin and isn't a fixed high/low.
static bool live(const Output& r) { return r.pin >= 0 && !r.fixed; }

static bool publish(Output& r) {
    bool on = r.state;
    if (!pub((roomsTopic + "/" + r.id()).c_str(), 0, true, on ? "ON" : "OFF")) return false;
    r.published = on;
    return true;
}

void Loop() {
    for (int i = 0; i < count; i++) {
        auto& r = outputs[i];
        if (!live(r)) continue;

        if (r.input > 0) {
            auto b = Inputs::Value(r.input);
            if (Inputs::GetRole(r.input) == Inputs::Role::Button) {
                // Toggle on the press edge only; -1 -> HIGH at boot is a held button, not a press.
                if (b == HIGH && r.lastInput == LOW) r.set(!r.state);
            } else if (b >= 0 && r.lastInput >= 0 && b != r.lastInput) {  // not the first read: keep the power-on state
                r.set(b == HIGH);  // switch / motion: follow the input on each change
            }
            r.lastInput = b;
        }

        bool on = r.state;
        if (r.powerOn == OutputPowerOn::Restore && r.saved != (int8_t)on) {
            r.saved = on;
            Log.printf("Saving %s: %d\n", r.lastFilename().c_str(), on);
            spurt(r.lastFilename(), on ? "1" : "0");
        }
        if (r.published != (int8_t)on && online) publish(r);
    }
}

// Outputs past the end of the list, without a pin, or fixed delete their entity so they don't
// linger in Home Assistant.
bool SendDiscovery() {
    for (int n = 1; n <= MAX; n++) {
        auto id = Sprintf("output_%d", n);
        bool ok = n <= count && live(outputs[n - 1]) ? sendSwitchDiscovery(id, outputs[n - 1].name, EC_NONE)
                                                     : sendDeleteDiscovery("switch", id);
        if (!ok) return false;
    }
    return true;
}

bool SendOnline() {
    for (int i = 0; i < count; i++)
        if (live(outputs[i]) && !publish(outputs[i])) return false;
    return true;
}

bool Command(std::string& command, std::string& pay) {
    for (int i = 0; i < count; i++) {
        auto& r = outputs[i];
        if (command != r.id()) continue;
        if (!live(r)) return true;
        bool on;
        if (outputParsePayload(pay, r.state, on))
            r.set(on);
        else
            Log.printf("Output %d: unknown payload \"%s\"\n", r.index, pay.c_str());
        return true;
    }
    return false;
}
bool State(int n) { return n >= 1 && n <= count && outputs[n - 1].state; }
}  // namespace Outputs
