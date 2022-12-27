#include <Arduino.h>
#include <AsyncTCP.h>
#include <AsyncWiFiSettings.h>
#include <ESPAsyncWebServer.h>
#include <HTTPClient.h>
#include <NimBLEDevice.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include <freertos/FreeRTOS.h>
#include <freertos/timers.h>

#include "Battery.h"
#include "BleFingerprint.h"
#include "BleFingerprintCollection.h"
#include "Enrollment.h"
#include "GUI.h"
#include "HttpReleaseUpdate.h"
#include "HttpWebServer.h"
#include "I2C.h"
#include "Motion.h"
#include "Network.h"
#include "SerialImprov.h"
#include "Updater.h"
#include "defaults.h"
#include "globals.h"
#include "mqtt.h"
#include "string_utils.h"
#ifdef M5STICK
#include <AXP192.h>
#endif
#ifdef SENSORS
#include <Wire.h>

#include "BH1750.h"
#include "BME280.h"
#include "BMP180.h"
#include "BMP280.h"
#include "AHTX0.h"
#include "DHT.h"
#include "HX711.h"
#include "SHT.h"
#include "SensirionSGP30.h"
#include "TSL2561.h"
#endif

TimerHandle_t reconnectTimer;
TaskHandle_t scanTaskHandle, reportTaskHandle;

unsigned long updateStartedMillis = 0;
unsigned long lastTeleMillis = 0;
int reconnectTries = 0;
int teleFails = 0;
bool online = false;         // Have we successfully sent status=online
bool sentDiscovery = false;  // Have we successfully sent discovery

int ethernetType = 0;
String mqttHost, mqttUser, mqttPass;
uint16_t mqttPort;

bool discovery, publishTele, publishRooms, publishDevices;
