#include "Relay.h"

#include <HeadlessWiFiSettings.h>

#include "globals.h"
#include "mqtt.h"
#include "string_utils.h"

namespace Relay {
namespace {
int8_t relay_1Pin = -1;
bool relay_1Inverted = false;
bool relay_1State = false;
bool relay_1LastPublished = false;
bool relay_1Published = false;

int8_t relay_2Pin = -1;
bool relay_2Inverted = false;
bool relay_2State = false;
bool relay_2LastPublished = false;
bool relay_2Published = false;

bool online = false;

bool toBool(const String &payload, bool &value) {
    if (payload.equalsIgnoreCase("ON") || payload == "1" || payload.equalsIgnoreCase("TRUE")) {
        value = true;
        return true;
    }
    if (payload.equalsIgnoreCase("OFF") || payload == "0" || payload.equalsIgnoreCase("FALSE")) {
        value = false;
        return true;
    }
    return false;
}

void applyRelay(int8_t pin, bool inverted, bool state) {
    if (pin < 0) return;
    digitalWrite(pin, (state ^ inverted) ? HIGH : LOW);
}

bool publishRelayState(const char *name, bool state, bool &published, bool &lastPublished) {
    if (!published || lastPublished != state) {
        if (!pub((roomsTopic + "/" + name).c_str(), 0, true, state ? "ON" : "OFF")) return false;
        lastPublished = state;
        published = true;
    }
    return true;
}

bool setRelay(const char *topicName, int8_t pin, bool inverted, bool &state, bool &published, bool &lastPublished, const String &payload) {
    bool requested = state;
    if (payload.equalsIgnoreCase("TOGGLE")) {
        requested = !state;
    } else if (!toBool(payload, requested)) {
        return false;
    }

    state = requested;
    applyRelay(pin, inverted, state);
    return publishRelayState(topicName, state, published, lastPublished);
}
}  // namespace

void Setup() {
    if (relay_1Pin >= 0) {
        pinMode(relay_1Pin, OUTPUT);
        applyRelay(relay_1Pin, relay_1Inverted, relay_1State);
    }
    if (relay_2Pin >= 0) {
        pinMode(relay_2Pin, OUTPUT);
        applyRelay(relay_2Pin, relay_2Inverted, relay_2State);
    }
}

void ConnectToWifi() {
    relay_1Pin = HeadlessWiFiSettings.integer("relay_1_pin", -1, "Relay One output pin (-1 for disable)");
    relay_1Inverted = HeadlessWiFiSettings.checkbox("relay_1_inverted", false, "Relay One invert output");

    relay_2Pin = HeadlessWiFiSettings.integer("relay_2_pin", -1, "Relay Two output pin (-1 for disable)");
    relay_2Inverted = HeadlessWiFiSettings.checkbox("relay_2_inverted", false, "Relay Two invert output");
}

void SerialReport() {
    Log.print("Relay One:    ");
    Log.println(relay_1Pin >= 0 ? "enabled" : "disabled");
    Log.print("Relay Two:    ");
    Log.println(relay_2Pin >= 0 ? "enabled" : "disabled");
}

void Loop() {
    if (relay_1Pin >= 0) {
        applyRelay(relay_1Pin, relay_1Inverted, relay_1State);
    }
    if (relay_2Pin >= 0) {
        applyRelay(relay_2Pin, relay_2Inverted, relay_2State);
    }
}

bool Command(String &command, String &pay) {
    if (command == "relay_1") {
        if (relay_1Pin < 0) return true;
        return setRelay("relay_1", relay_1Pin, relay_1Inverted, relay_1State, relay_1Published, relay_1LastPublished, pay);
    }
    if (command == "relay_2") {
        if (relay_2Pin < 0) return true;
        return setRelay("relay_2", relay_2Pin, relay_2Inverted, relay_2State, relay_2Published, relay_2LastPublished, pay);
    }
    return false;
}

bool SendDiscovery() {
    if (relay_1Pin >= 0) {
        if (!sendSwitchDiscovery("relay_1", EC_NONE)) return false;
    } else {
        if (!sendDeleteDiscovery("switch", "relay_1")) return false;
    }

    if (relay_2Pin >= 0) {
        if (!sendSwitchDiscovery("relay_2", EC_NONE)) return false;
    } else {
        if (!sendDeleteDiscovery("switch", "relay_2")) return false;
    }

    return true;
}

bool SendOnline() {
    if (online) return true;

    if (relay_1Pin >= 0) {
        if (!publishRelayState("relay_1", relay_1State, relay_1Published, relay_1LastPublished)) return false;
    }
    if (relay_2Pin >= 0) {
        if (!publishRelayState("relay_2", relay_2State, relay_2Published, relay_2LastPublished)) return false;
    }

    online = true;
    return true;
}
}  // namespace Relay
