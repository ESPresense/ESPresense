#include "Enrollment.h"

#include <cstring>

#include "Ble.h"
#include "BleFingerprintCollection.h"
#include "BleUuids.h"
#include "HttpWebServer.h"
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
        Log.printf("Client disconnected, conn: %u reason: %d\r\n", connHandle, reason);
        Ble::Server::AdvertiseConnectable();
    }
}

void Setup() {
    Ble::Server::SetCallbacks({onConnect, onDisconnect});
    Ble::Server::SetModelNumber(room);

    uint32_t nodeId = CHIPID;
    major = (nodeId & 0xFFFF0000) >> 16;
    minor = nodeId & 0xFFFF;

    // iBeacon advertisement: flags + Apple manufacturer data (uuid big-endian, major, minor, tx).
    uint8_t* p = beaconAdv;
    *p++ = 2; *p++ = 0x01; *p++ = 0x04;  // flags: BR/EDR not supported
    *p++ = 26; *p++ = 0xFF; *p++ = 0x4C; *p++ = 0x00; *p++ = 0x02; *p++ = 0x15;
    for (int i = 15; i >= 0; i--) *p++ = espresenseUUID.u128.value[i];
    *p++ = major >> 8; *p++ = major & 0xFF;
    *p++ = minor >> 8; *p++ = minor & 0xFF;
    *p++ = (uint8_t)BleFingerprintCollection::txRefRssi;
    beaconAdvLen = p - beaconAdv;
}

bool Loop() {
    static bool lastEnrolling = true;
    if (enrolling != lastEnrolling) {
        if (enrolling) {
            Ble::Server::AdvertiseConnectable();
            Log.printf("%u Advert | HRM\r\n", (unsigned)xPortGetCoreID());
        } else {
            Ble::Server::AdvertiseRaw(beaconAdv, beaconAdvLen);
            Log.printf("%u Advert | iBeacon\r\n", (unsigned)xPortGetCoreID());
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
