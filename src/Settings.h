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

//Specify the LED pin. For most dev boards, this is GPIO2
#ifdef M5STICK
#define LED_BUILTIN 10 // M5StickC
#else
#define LED_BUILTIN 13 // Feather
#endif

// Logic level for turning the led on. Most boards use active low, meaning LED_ON should be set to 0
#define LED_ON 0

//Define the base topic for room detection. Usually "room_presence"
#define CHANNEL String(F("room_presence"))

//Define the topic for publishing availability
#define AVAILABILITY_TOPIC String(F("presence_nodes/")) + room

//Define the topic for publishing JSON attributes
#define TELEMETRY_TOPIC String(F("presence_nodes/")) + room + String(F("/tele"))

// Define bluetooth scan parameters
#define BLE_ACTIVE_SCAN true // Active scan uses more power, but get results faster

#define BLE_SCAN_DURATION 2     // Define the duration of a single scan in seconds
#define BLE_SCAN_INTERVAL 333   // Used to determine antenna sharing between Bluetooth and WiFi. Do not modify unless you are confident you know what you're doing
#define BLE_SCAN_WINDOW 320     // Used to determine antenna sharing between Bluetooth and WiFi. Do not modify unless you are confident you know what you're doing

// Maximum distance (in meters) to report. Devices that are calculated to be further than this distance in meters will not be reported
#define MAX_DISTANCE 8

// MQTT topic for sensor values from HTU21D temperature and humidity sensor
//#define htuSensorTopic "presence_nodes/" hostname "/sensor"

//List of allowed MAC Addresses for MQTT Publish. All others will be ignored.
//Feature is disabled by default.
//#define ALLOWED_LIST_CHECK
String allowedList[] = {"11223344aabb", "11223344aabb"};
uint32_t allowedListNumberOfItems = 2;

