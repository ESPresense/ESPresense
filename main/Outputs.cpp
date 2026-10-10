#include "Outputs.h"

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

// Plain on/off outputs (relays, smart plug / Shelly outlets), published to Home Assistant as
// switches: state on <room>/output_N, commands (ON/OFF/TOGGLE) on <room>/output_N/set.
namespace Outputs {

struct Output {
    int index = 0;
    int pin = -1;
    std::string name;
    bool inverted = false;
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

Output outputs[MAX];
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
    std::vector<std::string> pinTypes = {"Output", "Output Inverted"};
    std::vector<std::string> powerOnStates = {"Off", "On", "Restore last"};
    std::vector<std::string> toggles = {"None"};
    for (int n = 1; n <= Inputs::MAX; n++) toggles.push_back(Sprintf("Input %d", n));

    count = Settings::integer("output_count", 0, MAX, 0, "Number of outputs");
    if (count < 0) count = 0;
    if (count > MAX) count = MAX;
    Settings::group("output", MAX, {"name", "pin", "type", "state", "input"});

    for (int i = 0; i < MAX; i++) outputs[i].index = i + 1;
    for (int n = 1; n <= count; n++) {
        auto& o = outputs[n - 1];
        o.name = Settings::string(Sprintf("output_%d_name", n), Sprintf("Output %d", n), "Name");
        o.pin = Settings::integer(Sprintf("output_%d_pin", n), -1, 48, -1, "Pin (-1 to disable)");
        o.inverted = Settings::dropdown(Sprintf("output_%d_type", n), pinTypes, 0, "Pin type") == 1;
        o.powerOn = (OutputPowerOn)Settings::dropdown(Sprintf("output_%d_state", n), powerOnStates, 0, "Power-on state");
        o.input = Settings::dropdown(Sprintf("output_%d_input", n), toggles, 0, "Linked input");
        if (o.name.empty()) o.name = Sprintf("Output %d", n);
        if (o.pin < 0) continue;

        std::string last = Settings::slurp(o.lastFilename());
        o.saved = last.empty() ? -1 : (last == "1");
        o.set(outputPowerOnState(o.powerOn, last));
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
            Log.printf("pin %d%s, %s\n", r.pin, r.inverted ? " inverted" : "", r.state ? "on" : "off");
    }
}

static bool publish(Output& r) {
    bool on = r.state;
    if (!pub((roomsTopic + "/" + r.id()).c_str(), 0, true, on ? "ON" : "OFF")) return false;
    r.published = on;
    return true;
}

void Loop() {
    for (auto& r : outputs) {
        if (r.pin < 0) continue;

        if (r.input > 0) {
            auto b = Inputs::Value(r.input);
            if (Inputs::GetRole(r.input) == Inputs::Role::Button) {
                // Toggle on the press edge only; -1 -> HIGH at boot is a held button, not a press.
                if (b == HIGH && r.lastInput == LOW) r.set(!r.state);
            } else if (b >= 0 && r.lastInput >= 0 && b != r.lastInput) {  // not the first read: keep the power-on state
                r.set(b == HIGH);  // Switch / Motion: follow the input on each change
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

// Slots above output_count, or without a pin, delete their entity so an output that was removed
// doesn't linger in Home Assistant.
bool SendDiscovery() {
    for (auto& r : outputs)
        if (!(r.pin >= 0 ? sendSwitchDiscovery(r.id(), r.name, EC_NONE) : sendDeleteDiscovery("switch", r.id())))
            return false;
    return true;
}

bool SendOnline() {
    for (auto& r : outputs)
        if (r.pin >= 0 && !publish(r)) return false;
    return true;
}

bool Command(std::string& command, std::string& pay) {
    for (auto& r : outputs) {
        if (command != r.id()) continue;
        if (r.pin < 0) return true;
        bool on;
        if (outputParsePayload(pay, r.state, on))
            r.set(on);
        else
            Log.printf("Output %d: unknown payload \"%s\"\n", r.index, pay.c_str());
        return true;
    }
    return false;
}
}  // namespace Outputs
