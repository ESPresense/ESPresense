#include "Enrollment.h"

#include <cstring>

#include "Ble.h"
#include "BleFingerprintCollection.h"
#include "BleUuids.h"
#include "HttpWebServer.h"
#include "IBeacon.h"
#include "globals.h"
#include "mqtt.h"
#include "string_utils.h"
#include "util.h"

namespace Enrollment {
static std::string newName, newId;
static unsigned long lastLoop = 0;
static volatile int connectionToEnroll = -1;
static uint16_t major, minor;
static uint8_t beaconAdv[31];
static size_t beaconAdvLen = 0;

static void onConnect(uint16_t connHandle, const ble_addr_t& peer) {
    Log.print("Connected to: ");
    Log.println(Ble::addrStr(peer));
    if (enrolling) {
        connectionToEnroll = connHandle;
        Ble::Server::AdvertiseConnectable();
    }
}

static void onDisconnect(uint16_t connHandle, int reason) {
    if (enrolling) {
        Log.printf("Client disconnected, conn: %u reason: %d\n", connHandle, reason);
        Ble::Server::AdvertiseConnectable();
    }
}

void Setup() {
    Ble::Server::SetCallbacks({onConnect, onDisconnect});
    Ble::Server::SetModelNumber(room);

    uint32_t nodeId = CHIPID;
    major = (nodeId & 0xFFFF0000) >> 16;
    minor = nodeId & 0xFFFF;

    beaconAdvLen = buildIBeaconAdv(beaconAdv, espresenseUUID.u128.value, major, minor, BleFingerprintCollection::txRefRssi);
}

bool Loop() {
    static bool lastEnrolling = true;
    // tx_ref_rssi can change over MQTT; it's the payload's last byte. Refresh it here, on this
    // task, rather than from the MQTT callback, and re-advertise unless enrolling (the next switch
    // back to iBeacon picks it up).
    int8_t tx = BleFingerprintCollection::txRefRssi;
    bool txChanged = setIBeaconTx(beaconAdv, beaconAdvLen, tx);
    if (txChanged && !enrolling && enrolling == lastEnrolling) {
        Ble::Server::AdvertiseRaw(beaconAdv, beaconAdvLen);
        Log.printf("%u Advert | iBeacon (tx %d)\n", (unsigned)xPortGetCoreID(), tx);
    }
    if (enrolling != lastEnrolling) {
        if (enrolling) {
            Ble::Server::AdvertiseConnectable();
            Log.printf("%u Advert | HRM\n", (unsigned)xPortGetCoreID());
        } else {
            Ble::Server::AdvertiseRaw(beaconAdv, beaconAdvLen);
            Log.printf("%u Advert | iBeacon\n", (unsigned)xPortGetCoreID());
        }
        lastEnrolling = enrolling;
        HttpWebServer::SendState();
    }

    if (enrolling && enrollingEndMillis < millis()) enrolling = false;

    if (millis() - lastLoop > 500) {
        lastLoop = millis();
        if (enrolling) HttpWebServer::SendState();
        if (Ble::Server::ConnectedCount()) {
            static unsigned long lastNotify = 0;
            if (millis() - lastNotify > 250) {  // throttle to 4Hz
                lastNotify = millis();
                Ble::Server::NotifyHeartRate(0b00000110, micros() & 0xFF);
            }

            if (enrolling && connectionToEnroll > -1) {
                std::string irk;
                if (Ble::Server::ReadPeerIrk(connectionToEnroll, irk)) {
                    if (newId.empty()) newId = newName.empty() ? std::string("irk:") + irk : slugify(newName);
                    sendConfig(std::string("irk:") + irk, newId, newName);
                    enrolledId = newId;
                    newId = newName = "";
                    enrolling = false;
                    Ble::Server::Disconnect(connectionToEnroll);
                    connectionToEnroll = -1;
                }
            }
        }
    }
    return true;
}

// "id|name" binds the next connecting device to that id; "name" derives the id from the name;
// "PRESS"/"" derives it from the IRK. The window is 120 s; cancelEnroll closes it early.
bool Command(std::string& command, std::string& pay) {
    if (command == "enroll") {
        auto sep = pay.find('|');
        if (sep != std::string::npos) {
            newId = pay.substr(0, sep);
            newName = pay.substr(sep + 1);
        } else {
            newId = "";
            newName = pay == "PRESS" ? "" : pay;
        }
        enrolling = true;
        enrollingEndMillis = millis() + 120000;
        HttpWebServer::SendState();
        return true;
    }
    if (command == "cancelEnroll") {
        enrolledId = newId = newName = "";
        enrolling = false;
        HttpWebServer::SendState();
        return true;
    }
    return false;
}

bool SendDiscovery() {
    return sendConfig(Sprintf("iBeacon:e5ca1ade-f007-ba11-0000-000000000000-%hu-%hu", major, minor), "node:" + id, room) &&
           sendButtonDiscovery("Enroll", EC_CONFIG);
}
}  // namespace Enrollment
