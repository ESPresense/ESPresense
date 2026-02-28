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
static String newName, newId;
static unsigned long lastLoop = 0;
static int connectionToEnroll = -1;
static uint16_t major, minor;
static NimBLEServer *pServer;
static NimBLEAdvertisementData *oAdvertisementData;
static NimBLEService *heartRate;
static NimBLEService *deviceInfo;

class ServerCallbacks : public NimBLEServerCallbacks {
    /**
     * @brief Restarts BLE advertising if enrollment mode is active when a client connects.
     *
     * @param pServer Server instance that reported the connection.
     */
    void onConnect(NimBLEServer *pServer) {
        if (enrolling) {
        Log.println("DEBUG: Starting HRM advertising");
            NimBLEDevice::startAdvertising();
        }
    };

    /**
     * @brief Handle a new BLE connection and record it for enrollment when active.
     *
     * If enrollment mode is active, stores the connection handle from the provided
     * connection descriptor into the global `connectionToEnroll` so the connection
     * can be used for the enrollment process.
     *
     * @param pServer Pointer to the NimBLE server for the connection.
     * @param desc Pointer to the connection descriptor containing the peer address and connection handle.
     */
    void onConnect(NimBLEServer *pServer, ble_gap_conn_desc *desc) {
        std::string addr = NimBLEAddress(desc->peer_ota_addr).toString();
        Log.print("Connected to: ");
        Log.println(addr.c_str());
        if (enrolling) {
        Log.println("DEBUG: Starting HRM advertising");
            connectionToEnroll = desc->conn_handle;
        }
    };

    /**
     * @brief Handle a BLE client disconnection and resume advertising if enrolling.
     *
     * When enrollment mode is active, this callback restarts BLE advertising so the
     * device remains discoverable for new connections.
     */
    void onDisconnect(NimBLEServer *pServer) {
        if (enrolling) {
        Log.println("DEBUG: Starting HRM advertising");
            Log.println("Client disconnected");
            NimBLEDevice::startAdvertising();
        }
    };

    /**
     * @brief Logs the updated MTU size and connection handle for a BLE connection.
     *
     * @param MTU Negotiated MTU size for the connection.
     * @param desc Pointer to the BLE connection descriptor containing the connection handle.
     */
    void onMTUChange(uint16_t MTU, ble_gap_conn_desc *desc) {
        Log.printf("MTU updated: %u for connection ID: %u\r\n", MTU, desc->conn_handle);
    };

    /**
     * @brief Callback invoked when BLE authentication for a connection completes.
     *
     * Logs whether the link became encrypted and the associated connection handle.
     *
     * @param desc Pointer to the connection descriptor containing `sec_state.encrypted` and `conn_handle`.
     */
    void onAuthenticationComplete(ble_gap_conn_desc *desc) {
        Log.printf("Encrypt connection %s conn: %d!\r\n", desc->sec_state.encrypted ? "success" : "failed", desc->conn_handle);
    }
};

class CharacteristicCallbacks : public NimBLECharacteristicCallbacks {
    /**
     * @brief Handle a characteristic read event by logging its UUID and current value.
     *
     * Logs the characteristic's UUID and the value returned to the reader.
     *
     * @param pCharacteristic Pointer to the characteristic being read; its UUID and current value are output to the log.
     */
    void onRead(NimBLECharacteristic *pCharacteristic) {
        std::string uuid = pCharacteristic->getUUID().toString();
        Log.print(uuid.c_str());
        Log.print(": onRead(), value: ");
        Log.println(pCharacteristic->getValue().c_str());
    };

    /**
     * @brief Handle a characteristic write event by logging the characteristic UUID and its new value.
     *
     * @param pCharacteristic Pointer to the NimBLE characteristic that was written by a client.
     */
    void onWrite(NimBLECharacteristic *pCharacteristic) {
        std::string uuid = pCharacteristic->getUUID().toString();
        Log.print(uuid.c_str());
        Log.print(": onWrite(), value: ");
        Log.println(pCharacteristic->getValue().c_str());
    };

    /**
     * @brief Callback invoked when a characteristic notification is being sent to subscribed clients.
     *
     * Called with the characteristic that is notifying so implementers can react to or log the notification event.
     *
     * @param pCharacteristic Pointer to the characteristic that is sending the notification.
     */
    void onNotify(NimBLECharacteristic *pCharacteristic) {
        Log.println("Sending notification to clients");
    };

    /**
     * @brief Logs the final status of a notification or indication for a characteristic.
     *
     * Records the characteristic, the notification/indication status, and the BLE stack return code.
     *
     * @param pCharacteristic Pointer to the characteristic for which the status was reported.
     * @param status The notification/indication status value.
     * @param code Numeric BLE stack return code associated with the operation.
     */
    void onStatus(NimBLECharacteristic *pCharacteristic, Status status, int code) {
        String str = ("Notification/Indication status code: ");
        str += status;
        str += ", return code: ";
        str += code;
        str += ", ";
        str += NimBLEUtils::returnCodeToString(code);
        Log.println(str);
    };

    /**
     * @brief Log a client's subscription state change for a characteristic.
     *
     * Logs the connection id, peer address, subscription action, and the characteristic UUID
     * when a client subscribes or unsubscribes to notifications/indications.
     *
     * @param pCharacteristic Pointer to the characteristic whose subscription changed.
     * @param desc Pointer to the GAP connection descriptor for the client (provides conn_handle and peer address).
     * @param subValue Subscription value where `0` = unsubscribed, `1` = notifications, `2` = indications, `3` = notifications and indications.
     */
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
        Log.println(str);
    };
};

class DescriptorCallbacks : public NimBLEDescriptorCallbacks {
    /**
     * @brief Log the value written to a BLE descriptor.
     *
     * Logs the descriptor's current value when a client writes to it.
     *
     * @param pDescriptor Pointer to the NimBLEDescriptor that was written.
     */
    void onWrite(NimBLEDescriptor *pDescriptor) {
        std::string dscVal = pDescriptor->getValue();
        Log.print("Descriptor written value:");
        Log.println(dscVal.c_str());
    };

    /**
     * @brief Callback invoked when a BLE descriptor is read by a client.
     *
     * Logs the descriptor's UUID and a read event message.
     *
     * @param pDescriptor Descriptor that was read.
     */
    void onRead(NimBLEDescriptor *pDescriptor) {
        std::string uuid = pDescriptor->getUUID().toString();
        Log.print(uuid.c_str());
        Log.println(" Descriptor read");
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
    NimBLEDevice::setSecurityInitKey(BLE_SM_PAIR_KEY_DIST_ENC | BLE_SM_PAIR_KEY_DIST_ID);
    NimBLEDevice::setSecurityRespKey(BLE_SM_PAIR_KEY_DIST_ENC | BLE_SM_PAIR_KEY_DIST_ID);
    NimBLEDevice::setSecurityIOCap(BLE_HS_IO_NO_INPUT_OUTPUT);
    NimBLEDevice::setSecurityAuth(true, true, true);

    pServer = NimBLEDevice::createServer();
    pServer->setCallbacks(new ServerCallbacks());

    heartRate = pServer->createService("180D");
    NimBLECharacteristic *bpm = heartRate->createCharacteristic("2A37", NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::READ_ENC, 2);
    bpm->setCallbacks(&chrCallbacks);

    deviceInfo = pServer->createService("180A");
    NimBLECharacteristic *manufName = deviceInfo->createCharacteristic("2A29", NIMBLE_PROPERTY::READ_ENC);
    manufName->setValue("ESPresense");
    manufName->setCallbacks(&chrCallbacks);
    NimBLECharacteristic *appearance = deviceInfo->createCharacteristic("2A01", NIMBLE_PROPERTY::READ_ENC, 2);
    appearance->setValue((int16_t)0x4142);
    appearance->setCallbacks(&chrCallbacks);
    NimBLECharacteristic *modelNum = deviceInfo->createCharacteristic("2A24", NIMBLE_PROPERTY::READ_ENC);
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

/**
 * @brief Periodic BLE runtime loop that manages advertising mode, heart-rate notifications, and enrollment processing.
 *
 * Performs state transitions when enrolling mode changes (switches between connectable HRM advertising and non-connectable iBeacon advertising),
 * enforces the enrollment timeout, sends periodic state updates to the HTTP server, emits Heart Rate Measurement notifications to connected clients,
 * and, when an enrolling connection provides an IRK, finalizes enrollment by sending node configuration, recording the enrolled ID, and disconnecting the enrolling peer.
 *
 * The function may modify global enrollment-related state (e.g., enrolling, enrollingEndMillis, newId, newName, enrolledId, connectionToEnroll)
 * and interacts with the BLE server and advertising objects. It also calls HttpWebServer::SendState() and sendConfig().
 *
 * @return bool Always returns `true`.
 */
bool Loop() {
    static bool lastEnrolling = true;
    if (enrolling != lastEnrolling) {
        auto pAdvertising = NimBLEDevice::getAdvertising();
        if (enrolling) {
        Log.println("DEBUG: Starting HRM advertising");
            pAdvertising->reset();
            pAdvertising->setScanResponse(true);
            pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
            pAdvertising->setMinPreferred(0x12);
            pAdvertising->setAdvertisementType(BLE_GAP_CONN_MODE_UND);
            pAdvertising->addServiceUUID(heartRate->getUUID());
            Log.println("DEBUG: before pAdvertising->start()");
        pAdvertising->start();
        Log.println("DEBUG: after pAdvertising->start()");
            Log.printf("%u Advert | HRM\r\n", xPortGetCoreID());
        } else {
            pAdvertising->reset();
            pAdvertising->setScanResponse(false);
            pAdvertising->setAdvertisementType(BLE_GAP_CONN_MODE_NON);
            pAdvertising->setAdvertisementData(*oAdvertisementData);
            Log.println("DEBUG: before pAdvertising->start()");
        pAdvertising->start();
        Log.println("DEBUG: after pAdvertising->start()");
            Log.printf("%u Advert | iBeacon\r\n", xPortGetCoreID());
        }
        lastEnrolling = enrolling;
        Log.println("DEBUG: calling SendState()");
        HttpWebServer::SendState();
        Log.println("DEBUG: SendState() complete - about to return from Loop");
    }

    if (enrolling && enrollingEndMillis < millis()) {
        enrolling = false;
    }

    if (millis() - lastLoop > 500) {
        lastLoop = millis();

        if (enrolling) Log.println("DEBUG: calling SendState()");
        HttpWebServer::SendState();
        Log.println("DEBUG: SendState() complete - about to return from Loop");
        if (pServer->getConnectedCount()) {
            NimBLEService *pSvc = pServer->getServiceByUUID("180D");
            if (pSvc) {
                NimBLECharacteristic *pChr = pSvc->getCharacteristic("2A37");
                if (pChr) {
                    static unsigned long lastNotify = 0;
                    if (millis() - lastNotify > 250) {  // throttle to 4Hz
                        lastNotify = millis();
                        uint8_t hr = micros() & 0xFF;
                        uint8_t buf[2] = {0b00000110, hr};
                        pChr->setValue(buf, 2);
                        pChr->notify();
                    }
                }
            }

            if (enrolling && connectionToEnroll > -1) {
                std::string irk;
                if (tryGetIrkFromConnection(connectionToEnroll, irk)) {
                    if (newId.isEmpty()) newId = newName.isEmpty() ? String("irk:") + irk.c_str() : slugify(newName);
                    sendConfig(String("irk:") + irk.c_str(), newId, newName);
                    enrolledId = newId;
                    newId = newName = "";
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
        Log.println("DEBUG: enroll command received");
        const int separatorIndex = pay.indexOf('|');
        if (separatorIndex != -1) {
            newId = pay.substring(0, separatorIndex);
            newName = pay.substring(separatorIndex + 1);
        } else {
            newId = "";
            newName = pay.equals("PRESS") ? "" : pay;
        }
        enrolling = true;
        Log.println("DEBUG: enrolling=true set");
        enrollingEndMillis = millis() + 120000;
        Log.println("DEBUG: calling SendState()");
        HttpWebServer::SendState();
        Log.println("DEBUG: SendState() complete - about to return from Loop");
        return true;
    }
    if (command == "cancelEnroll") {
        enrolledId = newId = newName = "";
        enrolling = false;
        Log.println("DEBUG: calling SendState()");
        HttpWebServer::SendState();
        Log.println("DEBUG: SendState() complete - about to return from Loop");
        return true;
    }
    return false;
}

bool SendDiscovery() {
    return sendConfig(Sprintf("iBeacon:e5ca1ade-f007-ba11-0000-000000000000-%hu-%hu", major, minor), "node:" + id, room) &&
           sendButtonDiscovery("Enroll", EC_CONFIG);
}
}  // namespace Enrollment