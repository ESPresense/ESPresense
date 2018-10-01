//Replace with your Wifi SSID; example: #define ssid "MyWifi"
#define ssid "$SSID$"

//Replace with your Wifi password; example: #define password "12345678"
#define password "$WIFI_PASSWORD$"

//Replace with a human-friendly host name.
#define hostname "esp32_room_presence"

//Replace with your MQTT Broker address; example: #define mqttServer "192.168.0.112"
#define mqttServer "$MQTT_BROKER_ADDRESS$"

//Replace with your MQTT Broker port; example: #define mqttPort 1883
#define mqttPort $MQTT_PORT$

//Replace with your MQTT Broker user; example: #define mqttUser "homeassistant"
#define mqttUser "§MQTT_USER$"

//Replace with your MQTT Broker password; example: #define mqttPassword "12345678"
#define mqttPassword "$MQTT_PASSWORD"

//Replace with the room name where the node will be placed; example: #define room "living-room"
#define room "$ROOM_NAME$"
