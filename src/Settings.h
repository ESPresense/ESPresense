#define Sprintf(f, ...) ({ char* s; asprintf(&s, f, __VA_ARGS__); String r = s; free(s); r; })
#define ESPMAC (Sprintf("%06" PRIx64, ESP.getEfuseMac() >> 24))

#if VERBOSE
#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#endif

//Replace with your Wifi SSID; example: #define ssid "MyWifi"
#define DEFAULT_WIFI_SSID "$SSID$"

//Replace with your Wifi password; example: #define password "12345678"
#define DEFAULT_WIFI_PASSWORD "$WIFI_PASSWORD$"

//Replace with a human-friendly host name. Must not contain spaces or special characters and be unique on your network
#define DEFAULT_HOSTNAME "esp32_room_presence"

//Replace with your MQTT Broker address
#define DEFAULT_MQTT_HOST "mqtt.z13.org"

//Replace with your MQTT Broker port
#define DEFAULT_MQTT_PORT 1883

//Replace with your MQTT Broker user
#define DEFAULT_MQTT_USER ""

//Replace with your MQTT Broker password
#define DEFAULT_MQTT_PASSWORD ""

//Replace with the room name where the node will be placed
#define DEFAULT_ROOM "living-room"

#ifdef M5STICK

#define LED_BUILTIN 10
#define LED_BUILTIN_ON 0

#define BUTTON 39
#define BUTTON_PRESSED 0

#else
#if M5ATOM

#define LED_BUILTIN 10
#define LED_BUILTIN_ON 0

#define BUTTON 39
#define BUTTON_PRESSED 0

#else // Huzzah32

#define LED_BUILTIN 13
#define LED_BUILTIN_ON 1

#endif
#endif

#ifdef M5STICK

#else

#endif

//Define the base topic for room detection. Usually "room_presence"
#define CHANNEL String("room_presence")

//Define the topic for publishing availability
#define AVAILABILITY_TOPIC (Sprintf("presence_nodes/%s", room.c_str()))

//Define the topic for publishing JSON attributes
#define TELEMETRY_TOPIC (Sprintf("presence_nodes/%s/tele", room.c_str()))

// Define bluetooth scan parameters
#define BLE_ACTIVE_SCAN false // Active scan uses more power, but get results faster

#define BLE_SCAN_INTERVAL 40 // Used to determine antenna sharing between Bluetooth and WiFi. Do not modify unless you are confident you know what you're doing
#define BLE_SCAN_WINDOW 30   // Used to determine antenna sharing between Bluetooth and WiFi. Do not modify unless you are confident you know what you're doing

// Maximum distance (in meters) to report. Devices that are calculated to be further than this distance in meters will not be reported
#define MAX_DISTANCE 16

// Max number of mac addresses to keep track of
#define MAX_MAC_ADDRESSES 50

// Number of seconds between update checks
#define CHECK_FOR_UPDATES_INTERVAL 300

#define ONE_EURO_FCMIN 0.05

#define ONE_EURO_BETA 0.05

#define ONE_EURO_DCUTOFF 1
