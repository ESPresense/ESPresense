#define Sprintf(f, ...) ({ char* s; asprintf(&s, f, __VA_ARGS__); String r = s; free(s); r; })
#define Stdprintf(f, ...) ({ char* s; asprintf(&s, f, __VA_ARGS__); std::string r = s; free(s); r; })
#define ESPMAC (Sprintf("%06" PRIx64, ESP.getEfuseMac() >> 24))

#ifdef VERBOSE
#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#endif

//Replace with your MQTT Broker address
#define DEFAULT_MQTT_HOST "mqtt.z13.org"

//Replace with your MQTT Broker port
#define DEFAULT_MQTT_PORT 1883

//Replace with your MQTT Broker user
#define DEFAULT_MQTT_USER ""

//Replace with your MQTT Broker password
#define DEFAULT_MQTT_PASSWORD ""

// Maximum distance (in meters) to report. Devices that are calculated to be further than this distance in meters will not be reported
#define DEFAULT_MAX_DISTANCE 16

//Define the base topic for room detection. Usually "espresense"
#define CHANNEL String("espresense")

#define DEFAULT_QUERY ""
#define DEFAULT_INCLUDE ""
#define DEFAULT_EXCLUDE ""

#define BLE_SCAN_INTERVAL 40 // Used to determine antenna sharing between Bluetooth and Wi-Fi. Do not modify unless you are confident you know what you're doing
#define BLE_SCAN_WINDOW 30   // Used to determine antenna sharing between Bluetooth and Wi-Fi. Do not modify unless you are confident you know what you're doing

#define DEFAULT_REF_RSSI (-65)

#define DEFAULT_FORGET_MS 300000 // Ms to remove fingerprint after not seeing it

#define DEFAULT_SKIP_DISTANCE 0.1 // If beacon has moved less than this skip update
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
#define DEFAULT_OTA_UPDATE false
#else
#define DEFAULT_AUTO_UPDATE false
#define DEFAULT_OTA_UPDATE true
#endif
