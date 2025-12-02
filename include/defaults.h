#pragma once

// Number of ms between update checks
#ifndef CHECK_FOR_UPDATES_INTERVAL
#define CHECK_FOR_UPDATES_INTERVAL 900000
#endif

// Number of seconds to wait for a Station Wifi connection to be established
#ifndef DEFAULT_WIFI_TIMEOUT
#define DEFAULT_WIFI_TIMEOUT 60
#endif

// Number of seconds to keep up captive portal (SoftAP) before rebooting
#ifndef DEFAULT_PORTAL_TIMEOUT
#define DEFAULT_PORTAL_TIMEOUT 300
#endif

#define UPDATE_STARTED -255
#define UPDATE_COMPLETE 255

#define JSON_BUFFER_SIZE (12 * 1024)

#define BLE_SCAN_INTERVAL 0x80
#define BLE_SCAN_WINDOW 0x80

#define MAX_TIME_SLOTS 64

// Define the base topic for room detection. Usually "espresense"
#define CHANNEL "espresense"

// Replace with your MQTT Broker address
#define DEFAULT_MQTT_HOST "mqtt.z13.org"

// Replace with your MQTT Broker port
#define DEFAULT_MQTT_PORT 1883

// Replace with your MQTT Broker user
#define DEFAULT_MQTT_USER ""

// Replace with your MQTT Broker password
#define DEFAULT_MQTT_PASSWORD ""

#define DEFAULT_HA_DISCOVERY_PREFIX "homeassistant"

// Maximum distance (in meters) to report. Devices that are calculated to be further than this distance in meters will not be reported
#define DEFAULT_MAX_DISTANCE 16

// Seconds before reporting radar/motion cleared
#define DEFAULT_DEBOUNCE_TIMEOUT 0.5

#define DEFAULT_QUERY ""
#define DEFAULT_REQUERY_MS 300000

#define DEFAULT_INCLUDE ""
#define DEFAULT_EXCLUDE ""
#define DEFAULT_KNOWN_MACS ""
#define DEFAULT_KNOWN_IRKS ""

#define DEFAULT_RX_REF_RSSI (-65)
#define DEFAULT_TX_REF_RSSI (-59)
#define DEFAULT_ABSORPTION (2.7)

#define DEFAULT_FORGET_MS 150000  // Ms to remove fingerprint after not seeing it
#define DEFAULT_SKIP_DISTANCE 0.5 // If beacon has moved less than this skip update
#define DEFAULT_SKIP_MS 5000      // Ms to skip mqtt update if no movement
#define DEFAULT_MAX_DIVISOR 10    // Max times to divide the skip update interval for large movements

#define DEFAULT_COUNT_ENTER 2.0f
#define DEFAULT_COUNT_EXIT 4.0f
#define DEFAULT_COUNT_MS 10000
#define DEFAULT_COUNT_IDS ""

// RX_ADJ_RSSI Defaults
#ifdef M5STICK
#define DEFAULT_RX_ADJ_RSSI 0
#else
#ifdef M5ATOM
#define DEFAULT_RX_ADJ_RSSI 0
#else
#ifdef ESP32S3
#define DEFAULT_RX_ADJ_RSSI 20
#else
#define DEFAULT_RX_ADJ_RSSI 0
#endif
#endif
#endif

// I2C Defaults
#ifdef M5STICK
#define DEFAULT_I2C_BUS_1_SDA 32
#define DEFAULT_I2C_BUS_1_SCL 33
#define DEFAULT_I2C_BUS_2_SDA 21
#define DEFAULT_I2C_BUS_2_SCL 22
#define DEFAULT_I2C_BUS 1
#else
#ifdef M5ATOM
#define DEFAULT_I2C_BUS_1_SDA 26
#define DEFAULT_I2C_BUS_1_SCL 32
#define DEFAULT_I2C_BUS_2_SDA 25
#define DEFAULT_I2C_BUS_2_SCL 21
#define DEFAULT_I2C_BUS 1
#else
#ifdef ESP32C3
#define DEFAULT_I2C_BUS_1_SDA 19
#define DEFAULT_I2C_BUS_1_SCL 18
#define DEFAULT_I2C_BUS_2_SDA -1
#define DEFAULT_I2C_BUS_2_SCL -1
#define DEFAULT_I2C_BUS 1
#else
#define DEFAULT_I2C_BUS_1_SDA 21
#define DEFAULT_I2C_BUS_1_SCL 22
#define DEFAULT_I2C_BUS_2_SDA -1
#define DEFAULT_I2C_BUS_2_SCL -1
#define DEFAULT_I2C_BUS 1
#endif
#endif
#endif

// TSL2561 Defaults
#define DEFAULT_TSL2561_I2C_GAIN "auto"

#define DEFAULT_ARDUINO_OTA false
#define DEFAULT_AUTO_UPDATE false

#if defined M5STICK

#define DEFAULT_LED1_TYPE 1
#define DEFAULT_LED1_PIN 10
#define DEFAULT_LED1_CNTRL Control_Type_Status
#define DEFAULT_LED1_CNT 1

#define BUTTON 39
#define BUTTON_PRESSED 0

#define MAX_BRIGHTNESS 100

#elif defined M5ATOM

#define DEFAULT_LED1_TYPE 2
#define DEFAULT_LED1_PIN 27
#define DEFAULT_LED1_CNTRL Control_Type_Status
#define DEFAULT_LED1_CNT 25

#define BUTTON 39
#define BUTTON_PRESSED 0

#define MAX_BRIGHTNESS 50 // M5Atom Matrix has a lower max brightness so it doesn't melt

#elif defined MACCHINA_A0

#define DEFAULT_LED1_TYPE 2
#define DEFAULT_LED1_PIN 2
#define DEFAULT_LED1_CNTRL Control_Type_Status
#define DEFAULT_LED1_CNT 1

#define MAX_BRIGHTNESS 20

#else  // DevKit / generic

#define DEFAULT_LED1_TYPE 0
#define DEFAULT_LED1_PIN 2
#define DEFAULT_LED1_CNTRL Control_Type_Status
#define DEFAULT_LED1_CNT 1

#define MAX_BRIGHTNESS 100

#endif

