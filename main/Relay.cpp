#include "Relay.h"

#include <atomic>
#include <vector>

#include "Button.h"
#include "RelayLogic.h"
#include "Settings.h"
#include "globals.h"
#include "mqtt.h"
#include "string_utils.h"
#include "util.h"

// Defined in main.cpp: set once status=online (and the retained states) have gone out.
extern bool online;

// Plain on/off outputs (smart plug / Shelly relays), published to Home Assistant as switches:
// state on <room>/relay_N, commands (ON/OFF/TOGGLE) on <room>/relay_N/set.
namespace Relay {

struct Output {
    int index = 0;
    int pin = -1;
    bool inverted = false;
    RelayPowerOn powerOn = RelayPowerOn::Off;
    int button = 0;                // 0 none, n: Button n toggles this relay
    std::atomic<bool> state{false};  // written by the MQTT task (Command) and the main loop (button)
    int8_t published = -1;         // last state sent to MQTT; main task only
    int8_t saved = -1;             // last state written to /relay_N_last; main task only
    int8_t lastButton = -1;

    std::string id() const { return Sprintf("relay_%d", index); }
    std::string name() const { return Sprintf("Relay %d", index); }
    std::string lastFilename() const { return Sprintf("/relay_%d_last", index); }

    void set(bool on) {
        state = on;
        if (pin >= 0) digitalWrite(pin, relayLevel(on, inverted));
    }
};

Output relays[MAX_RELAYS];
int count = 0;

void Setup() {
}

/**
 * @brief Register relay settings on the hardware endpoint and drive each enabled relay to its power-on state.
 *
 * The output is set here rather than in Setup() so an outlet comes back as soon as settings
 * are read, not after the WiFi connect (or captive portal) that sits between the two.
 * The level is written before the pin becomes an output, so there is no glitch to the wrong state.
 */
void ConnectToWifi(bool updating) {
    std::vector<std::string> relayTypes = {"Output", "Output Inverted"};
    std::vector<std::string> powerOnStates = {"Off", "On", "Restore last"};
    std::vector<std::string> buttons = {"None", "Button 1", "Button 2", "Button 3", "Button 4"};

    count = Settings::integer("relay_count", 0, MAX_RELAYS, 0, "Number of relays");
    if (count < 0) count = 0;
    if (count > MAX_RELAYS) count = MAX_RELAYS;
    Settings::group("relay", MAX_RELAYS, {"type", "pin", "state", "button"});

    for (int i = 0; i < MAX_RELAYS; i++) relays[i].index = i + 1;
    for (int n = 1; n <= count; n++) {
        auto& r = relays[n - 1];
        r.inverted = Settings::dropdown(Sprintf("relay_%d_type", n), relayTypes, 0, "Relay pin type") == 1;
        r.pin = Settings::integer(Sprintf("relay_%d_pin", n), -1, 48, -1, "Pin (-1 to disable)");
        r.powerOn = (RelayPowerOn)Settings::dropdown(Sprintf("relay_%d_state", n), powerOnStates, 0, "Power-on state");
        r.button = Settings::dropdown(Sprintf("relay_%d_button", n), buttons, 0, "Toggle with button");
        if (r.pin < 0) continue;

        std::string last = Settings::slurp(r.lastFilename());
        r.saved = last.empty() ? -1 : (last == "1");
        r.set(relayPowerOnState(r.powerOn, last));
        pinMode(r.pin, OUTPUT);
    }
}

void SerialReport() {
    for (int i = 0; i < count; i++) {
        auto& r = relays[i];
        Log.printf("Relay %d:      ", r.index);
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
    for (auto& r : relays) {
        if (r.pin < 0) continue;

        if (r.button > 0) {
            auto b = Button::Value(r.button);
            // Toggle on the press edge only; -1 -> HIGH at boot is a held button, not a press.
            if (b == HIGH && r.lastButton == LOW) r.set(!r.state);
            r.lastButton = b;
        }

        bool on = r.state;
        if (r.powerOn == RelayPowerOn::Restore && r.saved != (int8_t)on) {
            r.saved = on;
            Log.printf("Saving %s: %d\n", r.lastFilename().c_str(), on);
            spurt(r.lastFilename(), on ? "1" : "0");
        }
        if (r.published != (int8_t)on && online) publish(r);
    }
}

// Slots above relay_count, or without a pin, delete their entity so a relay that was removed
// doesn't linger in Home Assistant.
bool SendDiscovery() {
    for (auto& r : relays)
        if (!(r.pin >= 0 ? sendSwitchDiscovery(r.name(), EC_NONE) : sendDeleteDiscovery("switch", r.name())))
            return false;
    return true;
}

bool SendOnline() {
    for (auto& r : relays)
        if (r.pin >= 0 && !publish(r)) return false;
    return true;
}

bool Command(std::string& command, std::string& pay) {
    for (auto& r : relays) {
        if (command != r.id()) continue;
        if (r.pin < 0) return true;
        bool on;
        if (relayParsePayload(pay, r.state, on))
            r.set(on);
        else
            Log.printf("Relay %d: unknown payload \"%s\"\n", r.index, pay.c_str());
        return true;
    }
    return false;
}
}  // namespace Relay
