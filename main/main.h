#pragma once
#include <ArduinoJson.h>

#include <string>

#include "Battery.h"
#include "Ble.h"
#include "BleFingerprint.h"
#include "BleFingerprintCollection.h"
#include "Button.h"
#include "CAN.h"
#include "Enrollment.h"
#include "GUI.h"
#include "HeapWatchdog.h"
#include "HttpWebServer.h"
#include "Logger.h"
#include "Motion.h"
#include "NTP.h"
#include "Network.h"
#include "SerialImprov.h"
#include "Settings.h"
#include "Switch.h"
#include "Updater.h"
#include "build_timestamp.h"
#include "defaults.h"
#include "esp_app_desc.h"
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "globals.h"
#include "mqtt.h"
#include "string_utils.h"
#include "util.h"
#ifdef M5STICK
#include "AXP192.h"
#endif
#ifdef SENSORS
#include "AHTX0.h"
#include "BH1750.h"
#include "BME280.h"
#include "BMP180.h"
#include "BMP280.h"
#include "DHT.h"
#include "DS18B20Sensor.h"
#include "HX711.h"
#include "I2C.h"
#include "SHT.h"
#include "SensirionSCD4x.h"
#include "SensirionSGP30.h"
#include "TSL2561.h"
#endif

TimerHandle_t reconnectTimer;
TaskHandle_t scanTaskHandle;

unsigned long updateStartedMillis = 0;
unsigned long lastTeleMillis = 0;
int reconnectTries = 0;
int teleFails = 0;
int reportFailed = 0;
bool online = false;         // Have we successfully sent status=online
bool sentDiscovery = false;  // Have we successfully sent discovery
UBaseType_t bleStack = 0;

int ethernetType = 0;
std::string mqttHost, mqttUser, mqttPass;
uint16_t mqttPort;

bool discovery, publishTele, publishDevices;
