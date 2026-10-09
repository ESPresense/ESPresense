#pragma once

#include <cctype>
#include <cstdint>
#include <string>

// The relay decisions that need no hardware, kept apart from Relay.cpp so they can be tested
// on the host. See Relay.cpp for the callers.

// relay_N_state: what the outlet does after a power cut.
enum class RelayPowerOn : uint8_t { Off = 0, On = 1, Restore = 2 };

// The state to drive at boot. `last` is the persisted /relay_N_last file: "1" on, anything
// else (including a missing file, i.e. "") off, so a fresh node in Restore mode stays off.
inline bool relayPowerOnState(RelayPowerOn mode, const std::string& last) {
    switch (mode) {
        case RelayPowerOn::On: return true;
        case RelayPowerOn::Restore: return last == "1";
        default: return false;
    }
}

// Pin level for a logical state. Inverted relays (active-low driver boards) are on at LOW.
inline int relayLevel(bool on, bool inverted) { return (on != inverted) ? 1 : 0; }

// A …/relay_N/set payload: ON, OFF or TOGGLE (any case, surrounding whitespace ignored).
// Returns false and leaves `out` alone for anything else.
inline bool relayParsePayload(const std::string& payload, bool current, bool& out) {
    std::string p;
    for (char c : payload)
        if (!isspace((unsigned char)c)) p += (char)toupper((unsigned char)c);
    if (p == "ON") out = true;
    else if (p == "OFF") out = false;
    else if (p == "TOGGLE") out = !current;
    else return false;
    return true;
}
