#pragma once

#include <string>
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "esp_http_server.h"
#include "esp_spiffs.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_ble_api.h"
#include "esp_gatt_defs.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "driver/i2c.h"
#include "cJSON.h"

// Component headers
#include "ble_fingerprint.h"
#include "mqtt_handler.h"

// Global variables
extern TimerHandle_t reconnectTimer;
extern TaskHandle_t scanTaskHandle;
extern unsigned long updateStartedMillis;
extern unsigned long lastTeleMillis;
extern int reconnectTries;
extern int teleFails;
extern int reportFailed;
extern bool online;
extern bool sentDiscovery;
extern UBaseType_t bleStack;

// Configuration
extern std::string room;
extern std::string mqttHost;
extern std::string mqttUser;
extern std::string mqttPass;
extern uint16_t mqttPort;

// Settings
extern bool discovery;
extern bool publishTele;
extern bool publishRooms;
extern bool publishDevices;

// Function declarations
void app_main(void);
bool sendTelemetry(unsigned int totalSeen, unsigned int totalFpSeen, 
                  unsigned int totalFpQueried, unsigned int totalFpReported, 
                  unsigned int count);

// Constants
constexpr const char* DEFAULT_MQTT_HOST = "mqtt.local";
constexpr uint16_t DEFAULT_MQTT_PORT = 1883;
constexpr const char* DEFAULT_MQTT_USER = "";
constexpr const char* DEFAULT_MQTT_PASSWORD = "";

