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
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"

#include <AsyncTCP.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <AsyncMqttClient.h>
#include <ArduinoJSON.h>
#include <ArduinoOTA.h>
#include "BLEBeacon.h"
#include "BLEEddystoneTLM.h"
#include "BLEEddystoneURL.h"
#include "Settings_local.h"

BLEScan* pBLEScan;
int scanTime = 3; //In seconds
int waitTime = scanInterval; //In seconds
bool updateInProgress = false;

uint16_t beaconUUID = 0xFEAA;
#define ENDIAN_CHANGE_U16(x) ((((x)&0xFF00)>>8) + (((x)&0xFF)<<8))

WiFiClient espClient;
AsyncMqttClient mqttClient;
TimerHandle_t mqttReconnectTimer;
TimerHandle_t wifiReconnectTimer;

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
	Serial.print("Setting hostname to ");
	Serial.println(hostname);
	WiFi.begin(ssid, password);
	WiFi.setHostname(hostname);
}

void connectToMqtt() {
  Serial.println("Connecting to MQTT");
  mqttClient.setCredentials(mqttUser, mqttPassword);
  mqttClient.connect();
}

void WiFiEvent(WiFiEvent_t event) {
    Serial.print("[WiFi-event] event:");
		Serial.println(event);
    switch(event) {
	    case SYSTEM_EVENT_STA_GOT_IP:
					digitalWrite(LED_BUILTIN, !LED_ON);
	        Serial.println("IP address: ");
	        Serial.println(WiFi.localIP());
	        connectToMqtt();
	        break;
	    case SYSTEM_EVENT_STA_DISCONNECTED:
					digitalWrite(LED_BUILTIN, LED_ON);
	        Serial.println("WiFi lost connection");
					mqttClient.disconnect();
	        xTimerStop(mqttReconnectTimer, 0); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
	        xTimerStart(wifiReconnectTimer, 0);
	        break;
			case SYSTEM_EVENT_WIFI_READY:
					Serial.println("Wifi Ready");
					break;
			case SYSTEM_EVENT_STA_START:
					// Serial.println("STA Start");
					break;
			case SYSTEM_EVENT_STA_STOP:
					Serial.println("STA Stop");
					break;

    }
}

void onMqttConnect(bool sessionPresent) {
  Serial.println("Connected to MQTT.");
  Serial.print("Session present: ");
  Serial.println(sessionPresent);

  if (mqttClient.publish(availabilityTopic, 0, 0, "CONNECTED") == true) {
    Serial.print("Success sending message to topic:\t");
		Serial.println(availabilityTopic);
  } else {
    Serial.println("Error sending message");
  }

}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.println("Disconnected from MQTT.");
  if (WiFi.isConnected() && !updateInProgress) {
		Serial.println("Starting reconnect timer");
    xTimerStart(mqttReconnectTimer, 0);
  }
}


void reportDevice(BLEAdvertisedDevice advertisedDevice) {

	// Serial.printf("\n\n");

	StaticJsonBuffer<500> JSONbuffer;
	JsonObject& JSONencoder = JSONbuffer.createObject();

	String mac_address = advertisedDevice.getAddress().toString().c_str();
	mac_address.replace(":","");
	mac_address.toLowerCase();
	int rssi = advertisedDevice.getRSSI();

	JSONencoder["id"] = mac_address;
	JSONencoder["uuid"] = mac_address;
	JSONencoder["rssi"] = rssi;

	if (advertisedDevice.haveName()){
		String nameBLE = String(advertisedDevice.getName().c_str());
		// Serial.print("Name: ");
		// Serial.println(nameBLE);
		JSONencoder["name"] = nameBLE;
	// } else {
		// JSONencoder["name"] = "unknown";
	}

	// Serial.printf("\n\n");
	// Serial.printf("Advertised Device: %s \n", advertisedDevice.toString().c_str());
	std::string strServiceData = advertisedDevice.getServiceData();
	 uint8_t cServiceData[100];
	 strServiceData.copy((char *)cServiceData, strServiceData.length(), 0);

	 if (advertisedDevice.getServiceDataUUID().equals(BLEUUID(beaconUUID))==true) {  // found Eddystone UUID
		// Serial.printf("is Eddystone: %d %s length %d\n", advertisedDevice.getServiceDataUUID().bitSize(), advertisedDevice.getServiceDataUUID().toString().c_str(),strServiceData.length());
		if (cServiceData[0]==0x10) {
			 BLEEddystoneURL oBeacon = BLEEddystoneURL();
			 oBeacon.setData(strServiceData);
			 // Serial.printf("Eddystone Frame Type (Eddystone-URL) ");
			 // Serial.printf(oBeacon.getDecodedURL().c_str());
			 JSONencoder["url"] = oBeacon.getDecodedURL().c_str();

		} else if (cServiceData[0]==0x20) {
			 BLEEddystoneTLM oBeacon = BLEEddystoneTLM();
			 oBeacon.setData(strServiceData);
			 // Serial.printf("Eddystone Frame Type (Unencrypted Eddystone-TLM) \n");
			 // Serial.printf(oBeacon.toString().c_str());
		} else {
			for (int i=0;i<strServiceData.length();i++) {
				// Serial.printf("[%X]",cServiceData[i]);
			}
		}
		// Serial.printf("\n");

	 } else {
		if (advertisedDevice.haveManufacturerData()==true) {
			std::string strManufacturerData = advertisedDevice.getManufacturerData();
			// Serial.println("Got manufacturer data");
			uint8_t cManufacturerData[100];
			strManufacturerData.copy((char *)cManufacturerData, strManufacturerData.length(), 0);

			if (strManufacturerData.length()==25 && cManufacturerData[0] == 0x4C  && cManufacturerData[1] == 0x00 ) {
				BLEBeacon oBeacon = BLEBeacon();
				oBeacon.setData(strManufacturerData);

				String proximityUUID = getProximityUUIDString(oBeacon);

				// Serial.printf("iBeacon Frame\n");
				// Serial.printf("Major: %d Minor: %d UUID: %s Power: %d\n",ENDIAN_CHANGE_U16(oBeacon.getMajor()),ENDIAN_CHANGE_U16(oBeacon.getMinor()),proximityUUID.c_str(),oBeacon.getSignalPower());

				float distance = calculateDistance(rssi, oBeacon.getSignalPower());
				// Serial.print("RSSI: ");
				// Serial.print(rssi);
				// Serial.print("\ttxPower: ");
				// Serial.print(oBeacon.getSignalPower());
				// Serial.print("\tDistance: ");
				// Serial.println(distance);

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

				// Serial.printf("strManufacturerData: %d \n",strManufacturerData.length());
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

			// Serial.printf("no Beacon Advertised ServiceDataUUID: %d %s \n", advertisedDevice.getServiceDataUUID().bitSize(), advertisedDevice.getServiceDataUUID().toString().c_str());
		 }
		}

		char JSONmessageBuffer[512];
		JSONencoder.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));

		String publishTopic = String(channel) + "/" + room;

		if (mqttClient.connected()) {
			if (mqttClient.publish((char *)publishTopic.c_str(), 0, 0, JSONmessageBuffer) == true) {

		    // Serial.print("Success sending message to topic: "); Serial.println(publishTopic);

		  } else {
		    Serial.print("Error sending message: ");
				Serial.println(publishTopic);
		    Serial.print("Message: ");
				Serial.println(JSONmessageBuffer);
		  }
		} else {

			Serial.println("MQTT disconnected.");

		}

}

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {

	void onResult(BLEAdvertisedDevice advertisedDevice) {

		digitalWrite(LED_BUILTIN, LED_ON);
		// Serial.printf("Advertised Device: %s \n", advertisedDevice.toString().c_str());
		vTaskDelay(10 / portTICK_PERIOD_MS);
		digitalWrite(LED_BUILTIN, !LED_ON);

	}

};

unsigned long last = 0;
TaskHandle_t BLEScan;

void scanForDevices(void * parameter) {
	while(1) {
		if (!updateInProgress && WiFi.isConnected() && (millis() - last > (waitTime * 1000) || last == 0)) {
			// mqttClient.disconnect(true);
			// xTimerStop(mqttReconnectTimer, 0); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
	    Serial.print("Scanning...\t");
			BLEScanResults foundDevices = pBLEScan->start(scanTime);
	    Serial.printf("Scan done! Devices found: %d\t",foundDevices.getCount());
			// mqttClient.connect();
			// while (!mqttClient.connected()) {
			// 	Serial.print(".");
			// 	vTaskDelay(10 / portTICK_PERIOD_MS);
			// }
			for (uint32_t i = 0; i < foundDevices.getCount(); i++) {
				if (mqttClient.connected()) {
					reportDevice(foundDevices.getDevice(i));
				}
			}
	    last = millis();
			Serial.println("Reports sent");
	  }
	}
}

void configureOTA() {
	ArduinoOTA
    .onStart([]() {
			Serial.println("OTA Start");
			pBLEScan->stop();
			updateInProgress = true;
			mqttClient.disconnect(true);
			xTimerStop(mqttReconnectTimer, 0); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
    })
    .onEnd([]() {
			updateInProgress = false;
			digitalWrite(LED_BUILTIN, !LED_ON);
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
			byte percent = (progress / (total / 100));
      Serial.printf("Progress: %u% \n", percent);
			digitalWrite(LED_BUILTIN, percent % 2);
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
			ESP.restart();
    });

  ArduinoOTA.begin();
}

void setup() {

  Serial.begin(115200);

	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, LED_ON);

  mqttReconnectTimer = xTimerCreate("mqttTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToMqtt));
  wifiReconnectTimer = xTimerCreate("wifiTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToWifi));

  WiFi.onEvent(WiFiEvent);

  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);

  mqttClient.setServer(mqttHost, mqttPort);
	mqttClient.setWill(availabilityTopic, 0, 0, "DISCONNECTED");
	mqttClient.setKeepAlive(60);

  connectToWifi();

	configureOTA();

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
	pBLEScan->setInterval(0x80);
	pBLEScan->setWindow(0x10);

	xTaskCreatePinnedToCore(
		scanForDevices,
		"BLE Scan",
		4096,
		pBLEScan,
		1,
		&BLEScan,
		1);

}

void loop() {
	TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;
	TIMERG0.wdt_feed=1;
	TIMERG0.wdt_wprotect=0;
	ArduinoOTA.handle();
}
