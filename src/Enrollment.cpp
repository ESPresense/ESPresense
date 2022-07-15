#include "Enrollment.h"
#include "mqtt.h"

#include <NimBLEDevice.h>
#include <NimBLEAdvertisedDevice.h>
#include <NimBLEService.h>
#include <NimBLECharacteristic.h>
#include <NimBLEDescriptor.h>

namespace Enrollment
{
    static bool enrolling = false;
    static NimBLEServer* pServer;
    static String irk, id;

    class ServerCallbacks : public NimBLEServerCallbacks {

        void onConnect(NimBLEServer* pServer) {
            Serial.println("Client connected");
            Serial.println("Multi-connect support: start advertising");
            NimBLEDevice::startAdvertising();
        };

        void onConnect(NimBLEServer* pServer, ble_gap_conn_desc* desc) {
            Serial.print("Client address: ");
            Serial.println(NimBLEAddress(desc->peer_ota_addr).toString().c_str());
            NimBLEDevice::startSecurity(desc->conn_handle);
        };

        void onDisconnect(NimBLEServer* pServer) {
            Serial.println("Client disconnected - start advertising");
            NimBLEDevice::startAdvertising();
        };

        void onMTUChange(uint16_t MTU, ble_gap_conn_desc* desc) {
            Serial.printf("MTU updated: %u for connection ID: %u\n", MTU, desc->conn_handle);
        };

        void onAuthenticationComplete(ble_gap_conn_desc* desc){
            if(!desc->sec_state.encrypted) Serial.println("Encrypt connection failed!");
            else Serial.println("Encrypt connection success!");
        };
    };

    class CharacteristicCallbacks : public NimBLECharacteristicCallbacks {
        void onRead(NimBLECharacteristic* pCharacteristic){
            Serial.print(pCharacteristic->getUUID().toString().c_str());
            Serial.print(": onRead(), value: ");
            Serial.println(pCharacteristic->getValue().c_str());
        };

        void onWrite(NimBLECharacteristic* pCharacteristic) {
            Serial.print(pCharacteristic->getUUID().toString().c_str());
            Serial.print(": onWrite(), value: ");
            Serial.println(pCharacteristic->getValue().c_str());
        };
        /** Called before notification or indication is sent,
         *  the value can be changed here before sending if desired.
         */
        void onNotify(NimBLECharacteristic* pCharacteristic) {
            Serial.println("Sending notification to clients");
        };


        /** The status returned in status is defined in NimBLECharacteristic.h.
         *  The value returned in code is the NimBLE host return code.
         */
        void onStatus(NimBLECharacteristic* pCharacteristic, Status status, int code) {
            String str = ("Notification/Indication status code: ");
            str += status;
            str += ", return code: ";
            str += code;
            str += ", ";
            str += NimBLEUtils::returnCodeToString(code);
            Serial.println(str);
        };

        void onSubscribe(NimBLECharacteristic* pCharacteristic, ble_gap_conn_desc* desc, uint16_t subValue) {
            String str = "Client ID: ";
            str += desc->conn_handle;
            str += " Address: ";
            str += std::string(NimBLEAddress(desc->peer_ota_addr)).c_str();
            if(subValue == 0) {
                str += " Unsubscribed to ";
            }else if(subValue == 1) {
                str += " Subscribed to notfications for ";
            } else if(subValue == 2) {
                str += " Subscribed to indications for ";
            } else if(subValue == 3) {
                str += " Subscribed to notifications and indications for ";
            }
            str += std::string(pCharacteristic->getUUID()).c_str();

            Serial.println(str);
        };
    };

    class DescriptorCallbacks : public NimBLEDescriptorCallbacks {
        void onWrite(NimBLEDescriptor* pDescriptor) {
            std::string dscVal = pDescriptor->getValue();
            Serial.print("Descriptor witten value:");
            Serial.println(dscVal.c_str());
        };

        void onRead(NimBLEDescriptor* pDescriptor) {
            Serial.print(pDescriptor->getUUID().toString().c_str());
            Serial.println(" Descriptor read");
        };
    };


    static DescriptorCallbacks dscCallbacks;
    static CharacteristicCallbacks chrCallbacks;

    bool Busy() {
        return enrolling;
    }

    void Setup() {
        NimBLEDevice::setSecurityIOCap(BLE_HS_IO_DISPLAY_YESNO);
        NimBLEDevice::setSecurityAuth(BLE_SM_PAIR_AUTHREQ_BOND | BLE_SM_PAIR_AUTHREQ_MITM | BLE_SM_PAIR_AUTHREQ_SC);
        NimBLEDevice::setSecurityPasskey(123456);
    }

    static int iter_irk(int obj_type, union ble_store_value *val, void *cookie)
    {
        static const char hex_digits[] = "0123456789abcdef";
        const struct ble_store_value_sec *sec;
        int rc;

        sec = &val->sec;
        if (sec->irk_present) {
            std::string output;
            output.reserve(32);
            for(int i = 0; i < 16; i++) {
                auto c = sec->irk[15 - i];
                output.push_back(hex_digits[c >> 4]);
                output.push_back(hex_digits[c & 15]);
            }
            printf("IRK found: %s\n", output.c_str());
            irk = output.c_str();
        }

        return 0;
    }

    bool Loop() {

        if (!enrolling) {
            return false;
        }
        irk = "";

        NimBLEDevice::getScan()->stop();
        NimBLEAdvertising* pAdvertising = NimBLEDevice::getAdvertising();

        if (!pServer) {
            pServer = NimBLEDevice::createServer();
            pServer->setCallbacks(new ServerCallbacks());

            NimBLEService* pDeadService = pServer->createService("DEAD");
            NimBLECharacteristic* pBeefCharacteristic = pDeadService->createCharacteristic(
                                                    "BEEF",
                                                    NIMBLE_PROPERTY::READ |
                                                    NIMBLE_PROPERTY::WRITE |
                                                    NIMBLE_PROPERTY::READ_ENC |  // only allow reading if paired / encrypted
                                                    NIMBLE_PROPERTY::WRITE_ENC   // only allow writing if paired / encrypted
                                                    );

            pBeefCharacteristic->setValue("Burger");
            pBeefCharacteristic->setCallbacks(&chrCallbacks);

            /** 2904 descriptors are a special case, when createDescriptor is called with
             *  0x2904 a NimBLE2904 class is created with the correct properties and sizes.
             *  However we must cast the returned reference to the correct type as the method
             *  only returns a pointer to the base NimBLEDescriptor class.
             */
            NimBLE2904* pBeef2904 = (NimBLE2904*)pBeefCharacteristic->createDescriptor("2904");
            pBeef2904->setFormat(NimBLE2904::FORMAT_UTF8);
            pBeef2904->setCallbacks(&dscCallbacks);


            NimBLEService* pBaadService = pServer->createService("BAAD");
            NimBLECharacteristic* pFoodCharacteristic = pBaadService->createCharacteristic(
                                                    "F00D",
                                                    NIMBLE_PROPERTY::READ |
                                                    NIMBLE_PROPERTY::WRITE |
                                                    NIMBLE_PROPERTY::NOTIFY
                                                    );

            pFoodCharacteristic->setValue("Fries");
            pFoodCharacteristic->setCallbacks(&chrCallbacks);

            /** Note a 0x2902 descriptor MUST NOT be created as NimBLE will create one automatically
             *  if notification or indication properties are assigned to a characteristic.
             */

            /** Custom descriptor: Arguments are UUID, Properties, max length in bytes of the value */
            NimBLEDescriptor* pC01Ddsc = pFoodCharacteristic->createDescriptor(
                                                    "C01D",
                                                    NIMBLE_PROPERTY::READ |
                                                    NIMBLE_PROPERTY::WRITE|
                                                    NIMBLE_PROPERTY::WRITE_ENC, // only allow writing if paired / encrypted
                                                    20
                                                    );
            pC01Ddsc->setValue("Send it back!");
            pC01Ddsc->setCallbacks(&dscCallbacks);

            /** Start the services when finished creating all Characteristics and Descriptors */
            pDeadService->start();
            pBaadService->start();

            pAdvertising->addServiceUUID(pDeadService->getUUID());
            pAdvertising->addServiceUUID(pBaadService->getUUID());
        }

        pAdvertising->setScanResponse(true);
        pAdvertising->start();

        Serial.println("Advertising Started");
        if(pServer->getConnectedCount()) {
            NimBLEService* pSvc = pServer->getServiceByUUID("BAAD");
            if(pSvc) {
                NimBLECharacteristic* pChr = pSvc->getCharacteristic("F00D");
                if(pChr) {
                    pChr->notify(true);
                }
            }
        }

        auto started = millis();
        while (millis() - started < 60000)
        {
            auto rc = ble_store_iterate(BLE_STORE_OBJ_TYPE_PEER_SEC, iter_irk, NULL);
            if (!irk.isEmpty()) {
                alias(String("irk:") + irk, id.isEmpty() ? (String("irk:") + irk) : id);
                NimBLEDevice::deleteAllBonds();
                break;
            }
            printf("%d num bonds\n", NimBLEDevice::getNumBonds());
            delay(1000);
        }

        pAdvertising->stop();
        enrolling = false;
        return true;
    }

    bool Command(String& command, String& pay)
    {
        if (command == "enroll")
        {
            id = pay;
            enrolling = true;
            return true;
        }
        return false;
    }
}
