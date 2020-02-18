#define ssid "Travis"
#define password "123456789"
#define hostname "esp32_room_presence"
#define mqttHost IPAddress(192, 168, 1, 195)
#define mqttPort 1883
#define mqttUser "homeassistant"
#define mqttPassword "123456789"
#define room "living-room"
#define LED_BUILTIN 2
#define LED_ON 0
#define channel "room_presence"
#define availabilityTopic "presence_nodes/" room
#define telemetryTopic "presence_nodes/" hostname "/tele"
#define scanInterval 15
#define singleScanTime 10
#define activeScan true
#define bleScanInterval 0x80
#define bleScanWindow 0x10
#define maxDistance 2
