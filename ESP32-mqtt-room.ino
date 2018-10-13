/*

   Major thank you to the following contributors for their efforts:

   pcbreflux for the original version of this code, as well as the eddystone handlers.

   Andreis Speiss for his work on YouTube and his invaluable github at sensorsiot

   Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleScan.cpp
   Ported to Arduino ESP32 by Evandro Copercini
*/
#include <WiFi.h>
extern "C" {
	#include "freertos/FreeRTOS.h"
	#include "freertos/timers.h"
}
#include <AsyncTCP.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <AsyncMqttClient.h>
#include <ArduinoJSON.h>
#include "BLEBeacon.h"
#include "BLEEddystoneTLM.h"
#include "BLEEddystoneURL.h"
#include "Settings_local.h"

BLEScan* pBLEScan;
int scanTime = 10; //In seconds
int waitTime = scanInterval; //In seconds

uint16_t beconUUID = 0xFEAA;
#define ENDIAN_CHANGE_U16(x) ((((x)&0xFF00)>>8) + (((x)&0xFF)<<8))

WiFiClient espClient;
AsyncMqttClient mqttClient;
TimerHandle_t mqttReconnectTimer;
TimerHandle_t wifiReconnectTimer;

TaskHandle_t CoreZeroTask;

void handleWatchdog( void * parameter ) {
  for (;;) {
    vTaskDelay(10); // watchdog timer
 }
}

char *uint64_to_string(uint64_t input);

String getProximityUUIDString(BLEBeacon beacon) {
  std::string serviceData = beacon.getProximityUUID().toString().c_str();
  int serviceDataLength = serviceData.length();
  String returnedString = "";
  int i = serviceDataLength;
  while (i > 0)
  {
    if (serviceData[i-1] == '-') {
      i--;
    }
    char a = serviceData[i-1];
    char b = serviceData[i-2];
    returnedString += b;
    returnedString += a;

    i -= 2;
  }

  return returnedString;
}

float calculateDistance(int rssi, int txPower) {

  if (rssi == 0) {
      return -1.0;
  }

  if (!txPower) {
      // somewhat reasonable default value
      txPower = -59;
  }

  const float ratio = rssi * 1.0 / txPower;
  if (ratio < 1.0) {
      return pow(ratio, 10);
  } else {
      return (0.89976) * pow(ratio, 7.7095) + 0.111;
  }

}

void connectToWifi() {
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  WiFi.setHostname(hostname);
}

void connectToMqtt() {
  Serial.println("Connecting to MQTT");
  mqttClient.setCredentials(mqttUser, mqttPassword);
  mqttClient.connect();
}

void WiFiEvent(WiFiEvent_t event) {
    Serial.printf("[WiFi-event] event: %d\n", event);
    switch(event) {
    case SYSTEM_EVENT_STA_GOT_IP:
        Serial.println("WiFi connected");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
        connectToMqtt();
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        Serial.println("WiFi lost connection");
        xTimerStop(mqttReconnectTimer, 0); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
        xTimerStart(wifiReconnectTimer, 0);
        break;
    }
}

void onMqttConnect(bool sessionPresent) {
  Serial.println("Connected to MQTT.");
  Serial.print("Session present: ");
  Serial.println(sessionPresent);

  String publishTopic = String(channel) + "/" + room;
  if (mqttClient.publish((char *)publishTopic.c_str(), 0, 0, "Hello from ESP32") == true) {
    Serial.println("Success sending message to topic");
  } else {
    Serial.println("Error sending message");
  }

}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.println("Disconnected from MQTT.");

  if (WiFi.isConnected()) {
    xTimerStart(mqttReconnectTimer, 0);
  }
}

void onMqttPublish(uint16_t packetId) {
  Serial.println("Publish acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {

    void onResult(BLEAdvertisedDevice advertisedDevice) {

      unsigned long started = millis();
      Serial.printf("\n\n");
      Serial.println("onResult started");

      StaticJsonBuffer<500> JSONbuffer;
      JsonObject& JSONencoder = JSONbuffer.createObject();

      String mac_address = advertisedDevice.getAddress().toString().c_str();
      mac_address.replace(":","");
      mac_address.toLowerCase();
      int rssi = advertisedDevice.getRSSI();

      JSONencoder["id"] = mac_address;
      JSONencoder["uuid"] = mac_address;
      JSONencoder["rssi"] = rssi;

      Serial.println("Parsed id");

      if (advertisedDevice.haveName()){
        String nameBLE = String(advertisedDevice.getName().c_str());
        Serial.print("Name: ");
        Serial.println(nameBLE);
        JSONencoder["name"] = nameBLE;
      } else {
        JSONencoder["name"] = "unknown";
      }

      // Serial.printf("\n\n");
      Serial.printf("Advertised Device: %s \n", advertisedDevice.toString().c_str());
      std::string strServiceData = advertisedDevice.getServiceData();
       uint8_t cServiceData[100];
       strServiceData.copy((char *)cServiceData, strServiceData.length(), 0);

       if (advertisedDevice.getServiceDataUUID().equals(BLEUUID(beconUUID))==true) {  // found Eddystone UUID
        Serial.printf("is Eddystone: %d %s length %d\n", advertisedDevice.getServiceDataUUID().bitSize(), advertisedDevice.getServiceDataUUID().toString().c_str(),strServiceData.length());
        if (cServiceData[0]==0x10) {
           BLEEddystoneURL oBeacon = BLEEddystoneURL();
           oBeacon.setData(strServiceData);
           Serial.printf("Eddystone Frame Type (Eddystone-URL) ");
           Serial.printf(oBeacon.getDecodedURL().c_str());
           JSONencoder["url"] = oBeacon.getDecodedURL().c_str();

        } else if (cServiceData[0]==0x20) {
           BLEEddystoneTLM oBeacon = BLEEddystoneTLM();
           oBeacon.setData(strServiceData);
           Serial.printf("Eddystone Frame Type (Unencrypted Eddystone-TLM) \n");
           Serial.printf(oBeacon.toString().c_str());
        } else {
          for (int i=0;i<strServiceData.length();i++) {
            Serial.printf("[%X]",cServiceData[i]);
          }
        }
        Serial.printf("\n");

       } else {
        if (advertisedDevice.haveManufacturerData()==true) {
          std::string strManufacturerData = advertisedDevice.getManufacturerData();
          Serial.println("Got manufacturer data");
          uint8_t cManufacturerData[100];
          strManufacturerData.copy((char *)cManufacturerData, strManufacturerData.length(), 0);

          if (strManufacturerData.length()==25 && cManufacturerData[0] == 0x4C  && cManufacturerData[1] == 0x00 ) {
            BLEBeacon oBeacon = BLEBeacon();
            oBeacon.setData(strManufacturerData);

            String proximityUUID = getProximityUUIDString(oBeacon);

            Serial.printf("iBeacon Frame\n");
            Serial.printf("Major: %d Minor: %d UUID: %s Power: %d\n",ENDIAN_CHANGE_U16(oBeacon.getMajor()),ENDIAN_CHANGE_U16(oBeacon.getMinor()),proximityUUID.c_str(),oBeacon.getSignalPower());

            float distance = calculateDistance(rssi, oBeacon.getSignalPower());
            Serial.print("RSSI: ");
            Serial.print(rssi);
            Serial.print("\ttxPower: ");
            Serial.print(oBeacon.getSignalPower());
            Serial.print("\tDistance: ");
            Serial.println(distance);

            int major = ENDIAN_CHANGE_U16(oBeacon.getMajor());
            int minor = ENDIAN_CHANGE_U16(oBeacon.getMinor());

            JSONencoder["major"] = major;
            JSONencoder["minor"] = minor;

            JSONencoder["uuid"] = proximityUUID;
            JSONencoder["id"] = proximityUUID + "-" + String(major) + "-0";
            JSONencoder["txPower"] = oBeacon.getSignalPower();
            JSONencoder["distance"] = distance;

          } else {

            if (advertisedDevice.haveTXPower()) {
              float distance = calculateDistance(rssi, advertisedDevice.getTXPower());
              JSONencoder["txPower"] = advertisedDevice.getTXPower();
              JSONencoder["distance"] = distance;
            } else {
              float distance = calculateDistance(rssi, -59);
              JSONencoder["distance"] = distance;
            }

            Serial.printf("strManufacturerData: %d \n",strManufacturerData.length());
            // TODO: parse manufacturer data

          }
         } else {

          if (advertisedDevice.haveTXPower()) {
            float distance = calculateDistance(rssi, advertisedDevice.getTXPower());
            JSONencoder["txPower"] = advertisedDevice.getTXPower();
            JSONencoder["distance"] = distance;
          } else {
            float distance = calculateDistance(rssi, -59);
            JSONencoder["distance"] = distance;
          }

          Serial.printf("no Beacon Advertised ServiceDataUUID: %d %s \n", advertisedDevice.getServiceDataUUID().bitSize(), advertisedDevice.getServiceDataUUID().toString().c_str());
         }
        }

        char JSONmessageBuffer[512];
        JSONencoder.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));

        String publishTopic = String(channel) + "/" + room;

        if (mqttClient.publish((char *)publishTopic.c_str(), 0, 0, JSONmessageBuffer) == true) {

          Serial.print("Success sending message to topic: "); Serial.println(publishTopic);
      //    Serial.print("Message: "); Serial.println(JSONmessageBuffer);

        } else {
          Serial.print("Error sending message: "); Serial.println(publishTopic);
          Serial.print("Message: "); Serial.println(JSONmessageBuffer);
        }

        unsigned long duration = millis() - started;
        Serial.print("duration ");
        Serial.println(duration);
    }
};

void createTaskOnCoreZero() {
  xTaskCreatePinnedToCore(
    handleWatchdog,            /* Task function. */
    "CoreZeroTask",                 /* name of task. */
    1000,                    /* Stack size of task */
    NULL,                     /* parameter of the task */
    1,                        /* priority of the task */
    &CoreZeroTask,                   /* Task handle to keep track of created task */
    0);                       /* Core */
}


void setup() {

  Serial.begin(115200);

  mqttReconnectTimer = xTimerCreate("mqttTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToMqtt));
  wifiReconnectTimer = xTimerCreate("wifiTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToWifi));

  WiFi.onEvent(WiFiEvent);

  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);

  mqttClient.onPublish(onMqttPublish);

  connectToWifi();

  createTaskOnCoreZero();

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster

}

unsigned long last = 0;

void loop() {

  vTaskDelay(10); // watchdog timer

  if (millis() - last > (waitTime * 1000) || last == 0) {
    Serial.println("Scanning...");
    BLEScanResults foundDevices = pBLEScan->start(scanTime);
    Serial.printf("\nScan done! Devices found: %d\n",foundDevices.getCount());
    last = millis();
  }

}

char *uint64_to_string(uint64_t input) {
  static char result[21] = "";
  // Clear result from any leftover digits from previous function call.
  memset(&result[0], 0, sizeof(result));
  // temp is used as a temporary result storage to prevent sprintf bugs.
  char temp[21] = "";
  char c;
  uint8_t base = 10;

  while (input) {
      int num = input % base;
      input /= base;
      c = '0' + num;

      sprintf(temp, "%c%s", c, result);
      strcpy(result, temp);
  }
  return result;
}
