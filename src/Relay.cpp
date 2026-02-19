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

int8_t relay_3Pin = -1;
bool relay_3Inverted = false;
bool relay_3State = false;
bool relay_3LastPublished = false;
bool relay_3Published = false;

int8_t relay_4Pin = -1;
bool relay_4Inverted = false;
bool relay_4State = false;
bool relay_4LastPublished = false;
bool relay_4Published = false;

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
    if (relay_3Pin >= 0) {
        pinMode(relay_3Pin, OUTPUT);
        applyRelay(relay_3Pin, relay_3Inverted, relay_3State);
    }
    if (relay_4Pin >= 0) {
        pinMode(relay_4Pin, OUTPUT);
        applyRelay(relay_4Pin, relay_4Inverted, relay_4State);
    }
}

void ConnectToWifi() {
    relay_1Pin = HeadlessWiFiSettings.integer("relay_1_pin", -1, "Relay One output pin (-1 for disable)");
    relay_1Inverted = HeadlessWiFiSettings.checkbox("relay_1_inverted", false, "Relay One invert output");

    relay_2Pin = HeadlessWiFiSettings.integer("relay_2_pin", -1, "Relay Two output pin (-1 for disable)");
    relay_2Inverted = HeadlessWiFiSettings.checkbox("relay_2_inverted", false, "Relay Two invert output");

    relay_3Pin = HeadlessWiFiSettings.integer("relay_3_pin", -1, "Relay Three output pin (-1 for disable)");
    relay_3Inverted = HeadlessWiFiSettings.checkbox("relay_3_inverted", false, "Relay Three invert output");

    relay_4Pin = HeadlessWiFiSettings.integer("relay_4_pin", -1, "Relay Four output pin (-1 for disable)");
    relay_4Inverted = HeadlessWiFiSettings.checkbox("relay_4_inverted", false, "Relay Four invert output");
}

void SerialReport() {
    Log.print("Relay One:    ");
    Log.println(relay_1Pin >= 0 ? "enabled" : "disabled");
    Log.print("Relay Two:    ");
    Log.println(relay_2Pin >= 0 ? "enabled" : "disabled");
    Log.print("Relay Three:  ");
    Log.println(relay_3Pin >= 0 ? "enabled" : "disabled");
    Log.print("Relay Four:   ");
    Log.println(relay_4Pin >= 0 ? "enabled" : "disabled");
}

void Loop() {
    if (relay_1Pin >= 0) {
        applyRelay(relay_1Pin, relay_1Inverted, relay_1State);
    }
    if (relay_2Pin >= 0) {
        applyRelay(relay_2Pin, relay_2Inverted, relay_2State);
    }
    if (relay_3Pin >= 0) {
        applyRelay(relay_3Pin, relay_3Inverted, relay_3State);
    }
    if (relay_4Pin >= 0) {
        applyRelay(relay_4Pin, relay_4Inverted, relay_4State);
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
    if (command == "relay_3") {
        if (relay_3Pin < 0) return true;
        return setRelay("relay_3", relay_3Pin, relay_3Inverted, relay_3State, relay_3Published, relay_3LastPublished, pay);
    }
    if (command == "relay_4") {
        if (relay_4Pin < 0) return true;
        return setRelay("relay_4", relay_4Pin, relay_4Inverted, relay_4State, relay_4Published, relay_4LastPublished, pay);
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

    if (relay_3Pin >= 0) {
        if (!sendSwitchDiscovery("relay_3", EC_NONE)) return false;
    } else {
        if (!sendDeleteDiscovery("switch", "relay_3")) return false;
    }

    if (relay_4Pin >= 0) {
        if (!sendSwitchDiscovery("relay_4", EC_NONE)) return false;
    } else {
        if (!sendDeleteDiscovery("switch", "relay_4")) return false;
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
    if (relay_3Pin >= 0) {
        if (!publishRelayState("relay_3", relay_3State, relay_3Published, relay_3LastPublished)) return false;
    }
    if (relay_4Pin >= 0) {
        if (!publishRelayState("relay_4", relay_4State, relay_4Published, relay_4LastPublished)) return false;
    }

    online = true;
    return true;
}
}  // namespace Relay
