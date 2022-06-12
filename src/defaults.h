#pragma once

// Replace with your MQTT Broker address
#define DEFAULT_MQTT_HOST "mqtt.z13.org"

// Replace with your MQTT Broker port
#define DEFAULT_MQTT_PORT 1883

// Replace with your MQTT Broker user
#define DEFAULT_MQTT_USER ""

// Replace with your MQTT Broker password
#define DEFAULT_MQTT_PASSWORD ""

// Maximum distance (in meters) to report. Devices that are calculated to be further than this distance in meters will not be reported
#define DEFAULT_MAX_DISTANCE 16

// Seconds before reporting radar/motion cleared
#define DEFAULT_DEBOUNCE_TIMEOUT 0.5

// Define the base topic for room detection. Usually "espresense"
#define CHANNEL String("espresense")

#define DEFAULT_QUERY ""
#define DEFAULT_INCLUDE ""
#define DEFAULT_EXCLUDE ""

#define BLE_SCAN_INTERVAL 0x80
#define BLE_SCAN_WINDOW 0x80

#define DEFAULT_REF_RSSI (-65)
#define DEFAULT_ABSORPTION (3.5)

#define DEFAULT_FORGET_MS 300000 // Ms to remove fingerprint after not seeing it

#define DEFAULT_SKIP_DISTANCE 0.5 // If beacon has moved less than this skip update
#define DEFAULT_SKIP_MS 5000 // Ms to skip mqtt update if no movement

// Number of seconds between update checks
#define CHECK_FOR_UPDATES_INTERVAL 300

// I2C Defaults
#define DEFAULT_I2C_BUS_1_SDA 21
#define DEFAULT_I2C_BUS_1_SCL 22
#define DEFAULT_I2C_BUS 1

// TSL2561 Defaults
#define DEFAULT_TSL2561_I2C_GAIN "auto"

#ifdef VERSION
#define DEFAULT_AUTO_UPDATE true
#define DEFAULT_ARDUINO_OTA false
#else
#define DEFAULT_AUTO_UPDATE false
#define DEFAULT_ARDUINO_OTA true
#endif

static const char *const EC_DIAGNOSTIC = "diagnostic";
static const char *const EC_CONFIG = "config";
