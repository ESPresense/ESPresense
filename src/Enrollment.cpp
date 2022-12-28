#include "Enrollment.h"

#include <NimBLEAdvertisedDevice.h>
#include <NimBLECharacteristic.h>
#include <NimBLEDescriptor.h>
#include <NimBLEDevice.h>
#include <NimBLEService.h>

#include "HttpWebServer.h"
#include "globals.h"
#include "mqtt.h"
#include "util.h"

namespace Enrollment {
static const char hex_digits[] = "0123456789abcdef";
static bool lastEnrolling = true;
static String name;
static unsigned long lastLoop = 0;
static int connectionToEnroll = -1;
static uint16_t major, minor;
static NimBLEServer *pServer;
static NimBLEAdvertisementData *oAdvertisementData;
static NimBLEService *heartRate;
static NimBLEService *deviceInfo;

class ServerCallbacks : public NimBLEServerCallbacks {
    void onConnect(NimBLEServer *pServer) {
        Serial.println("Client connected");
        if (enrolling) {
            Serial.println("Multi-connect support: start advertising");
            NimBLEDevice::startAdvertising();
        }
    };

    void onConnect(NimBLEServer *pServer, ble_gap_conn_desc *desc) {
        Serial.print("Client address: ");
        Serial.println(NimBLEAddress(desc->peer_ota_addr).toString().c_str());
        if (enrolling) {
            NimBLEDevice::startSecurity(desc->conn_handle);
            connectionToEnroll = desc->conn_handle;
        }
    };

    void onDisconnect(NimBLEServer *pServer) {
        if (enrolling) {
            Serial.println("Client disconnected - start advertising");
            NimBLEDevice::startAdvertising();
        }
    };

    void onMTUChange(uint16_t MTU, ble_gap_conn_desc *desc) {
        Serial.printf("MTU updated: %u for connection ID: %u\n", MTU, desc->conn_handle);
    };

    void onAuthenticationComplete(ble_gap_conn_desc *desc) {
        if (!desc->sec_state.encrypted)
            Serial.printf("Encrypt connection failed conn: %d!\n", desc->conn_handle);
        else
            Serial.printf("Encrypt connection success conn: %d!\n", desc->conn_handle);
    };
};

class CharacteristicCallbacks : public NimBLECharacteristicCallbacks {
    void onRead(NimBLECharacteristic *pCharacteristic) {
        Serial.print(pCharacteristic->getUUID().toString().c_str());
        Serial.print(": onRead(), value: ");
        Serial.println(pCharacteristic->getValue().c_str());
    };

    void onWrite(NimBLECharacteristic *pCharacteristic) {
        Serial.print(pCharacteristic->getUUID().toString().c_str());
        Serial.print(": onWrite(), value: ");
        Serial.println(pCharacteristic->getValue().c_str());
    };

    void onNotify(NimBLECharacteristic *pCharacteristic){
        // Serial.println("Sending notification to clients");
    };

    void onStatus(NimBLECharacteristic *pCharacteristic, Status status, int code){
        /*             String str = ("Notification/Indication status code: ");
                    str += status;
                    str += ", return code: ";
                    str += code;
                    str += ", ";
                    str += NimBLEUtils::returnCodeToString(code);
                    Serial.println(str); */
    };

    void onSubscribe(NimBLECharacteristic *pCharacteristic, ble_gap_conn_desc *desc, uint16_t subValue) {
        String str = "Client ID: ";
        str += desc->conn_handle;
        str += " Address: ";
        str += std::string(NimBLEAddress(desc->peer_ota_addr)).c_str();
        if (subValue == 0) {
            str += " Unsubscribed to ";
        } else if (subValue == 1) {
            str += " Subscribed to notfications for ";
        } else if (subValue == 2) {
            str += " Subscribed to indications for ";
        } else if (subValue == 3) {
            str += " Subscribed to notifications and indications for ";
        }
        str += std::string(pCharacteristic->getUUID()).c_str();
        Serial.println(str);
    };
};

class DescriptorCallbacks : public NimBLEDescriptorCallbacks {
    void onWrite(NimBLEDescriptor *pDescriptor) {
        std::string dscVal = pDescriptor->getValue();
        Serial.print("Descriptor witten value:");
        Serial.println(dscVal.c_str());
    };

    void onRead(NimBLEDescriptor *pDescriptor) {
        Serial.print(pDescriptor->getUUID().toString().c_str());
        Serial.println(" Descriptor read");
    };
};

static DescriptorCallbacks dscCallbacks;
static CharacteristicCallbacks chrCallbacks;

static int ble_sm_read_bond(uint16_t conn_handle, struct ble_store_value_sec *out_bond) {
    struct ble_store_key_sec key_sec;
    struct ble_gap_conn_desc desc;
    int rc;

    rc = ble_gap_conn_find(conn_handle, &desc);
    if (rc != 0) {
        return rc;
    }

    memset(&key_sec, 0, sizeof key_sec);
    key_sec.peer_addr = desc.peer_id_addr;

    rc = ble_store_read_peer_sec(&key_sec, out_bond);
    return rc;
}

bool tryGetIrkFromConnection(uint16_t conn_handle, std::string &irk) {
    struct ble_store_value_sec bond;
    int rc;

    rc = ble_sm_read_bond(conn_handle, &bond);
    if (rc != 0)
        return false;

    if (!bond.irk_present)
        return false;

    std::string output;
    output.reserve(32);
    for (int i = 0; i < 16; i++) {
        auto c = bond.irk[15 - i];
        output.push_back(hex_digits[c >> 4]);
        output.push_back(hex_digits[c & 15]);
    }
    irk = output;
    return true;
}

void Setup() {
    NimBLEDevice::setSecurityIOCap(BLE_HS_IO_NO_INPUT_OUTPUT);
    NimBLEDevice::setSecurityAuth(true, true, true);

    pServer = NimBLEDevice::createServer();
    pServer->setCallbacks(new ServerCallbacks());

    heartRate = pServer->createService("180D");
    NimBLECharacteristic *bpm = heartRate->createCharacteristic("2A37", NIMBLE_PROPERTY::NOTIFY, 2);
    bpm->setCallbacks(&chrCallbacks);

    deviceInfo = pServer->createService("180A");
    NimBLECharacteristic *manufName = deviceInfo->createCharacteristic("2A29", NIMBLE_PROPERTY::READ);
    manufName->setValue("ESPresense");
    manufName->setCallbacks(&chrCallbacks);
    NimBLECharacteristic *appearance = deviceInfo->createCharacteristic("2A01", NIMBLE_PROPERTY::READ, 2);
    appearance->setValue((int16_t)0x4142);
    appearance->setCallbacks(&chrCallbacks);
    NimBLECharacteristic *modelNum = deviceInfo->createCharacteristic("2A24", NIMBLE_PROPERTY::READ);
    modelNum->setValue(std::string(room.c_str()));
    modelNum->setCallbacks(&chrCallbacks);

    heartRate->start();
    deviceInfo->start();

    uint32_t nodeId = (uint32_t)(ESP.getEfuseMac() >> 24);
    major = (nodeId & 0xFFFF0000) >> 16;
    minor = nodeId & 0xFFFF;

    BLEBeacon oBeacon = BLEBeacon();
    oBeacon.setManufacturerId(0x4C00);
    oBeacon.setProximityUUID(espresenseUUID);
    oBeacon.setMajor(major);
    oBeacon.setMinor(minor);
    oBeacon.setSignalPower(BleFingerprintCollection::txRefRssi);
    oAdvertisementData = new NimBLEAdvertisementData();
    oAdvertisementData->setFlags(BLE_HS_ADV_F_BREDR_UNSUP);
    oAdvertisementData->setManufacturerData(oBeacon.getData());

    pServer->start();
}

bool Loop() {
    if (enrolling != lastEnrolling) {
        HttpWebServer::SendState();
        auto pAdvertising = NimBLEDevice::getAdvertising();
        if (enrolling) {
            pAdvertising->reset();
            pAdvertising->setScanResponse(true);
            pAdvertising->setAdvertisementType(BLE_GAP_CONN_MODE_UND);
            pAdvertising->addServiceUUID(heartRate->getUUID());
            pAdvertising->start();
            Serial.println("Advertising HRM");
        } else {
            alias(Sprintf("iBeacon:e5ca1ade-f007-ba11-0000-000000000000-%hu-%hu", major, minor), "node:" + id, room);
            pAdvertising->reset();
            pAdvertising->setMaxInterval(3200);
            pAdvertising->setMinInterval(1600);
            pAdvertising->setScanResponse(false);
            pAdvertising->setAdvertisementType(BLE_GAP_CONN_MODE_NON);
            pAdvertising->setAdvertisementData(*oAdvertisementData);
            pAdvertising->start();
            Serial.println("Advertising iBeacon");
        }
        lastEnrolling = enrolling;
        if (enrolling) enrollingEndMillis = millis() + 120000;
    }

    if (enrolling && enrollingEndMillis < millis()) {
        enrolling = false;
    }

    if (millis() - lastLoop > 1000) {
        lastLoop = millis();

        if (enrolling) HttpWebServer::SendState();
        if (pServer->getConnectedCount()) {
            NimBLEService *pSvc = pServer->getServiceByUUID("180D");
            if (pSvc) {
                NimBLECharacteristic *pChr = pSvc->getCharacteristic("2A37");
                if (pChr) {
                    pChr->setValue((short)(micros() && 0x00FF));
                    pChr->notify(true);
                }
            }

            if (enrolling && connectionToEnroll > -1) {
                std::string irk;
                if (tryGetIrkFromConnection(connectionToEnroll, irk)) {
                    auto id = name.isEmpty() ? "" : slugify(name);
                    alias(String("irk:") + irk.c_str(), id.isEmpty() ? (String("irk:") + irk.c_str()) : id, name);
                    enrolling = false;
                    pServer->disconnect(connectionToEnroll);
                    connectionToEnroll = -1;
                }
            }
        }
    }

    return true;
}

bool Command(String &command, String &pay) {
    if (command == "enroll") {
        name = pay.equals("PRESS") ? "" : pay;
        enrolling = true;
        return true;
    }
    return false;
}

bool SendDiscovery() {
    return sendButtonDiscovery("Enroll", EC_CONFIG);
}
}  // namespace Enrollment
