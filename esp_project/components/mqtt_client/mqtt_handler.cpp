#include "mqtt_handler.h"
#include "esp_log.h"
#include <cstring>

static const char* TAG = "MQTT";

// Static member initialization
esp_mqtt_client_handle_t MqttHandler::client = nullptr;
bool MqttHandler::connected = false;

void MqttHandler::init(const std::string& host, uint16_t port,
                      const std::string& username, const std::string& password) {
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = host.c_str(),
        .broker.address.port = port,
        .credentials.username = username.c_str(),
        .credentials.authentication.password = password.c_str(),
    };

    client = esp_mqtt_client_init(&mqtt_cfg);
    if (!client) {
        ESP_LOGE(TAG, "Failed to initialize MQTT client");
        return;
    }

    // Register event handler
    esp_mqtt_client_register_event(client, MQTT_EVENT_ANY, mqtt_event_handler, NULL);

    // Start client
    esp_mqtt_client_start(client);
}

void MqttHandler::mqtt_event_handler(void *handler_args, esp_event_base_t base, 
                                   int32_t event_id, void *event_data) {
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;
    
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT Connected");
            connected = true;
            break;
            
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT Disconnected");
            connected = false;
            break;
            
        case MQTT_EVENT_ERROR:
            ESP_LOGE(TAG, "MQTT Error");
            break;
            
        default:
            break;
    }
}

bool MqttHandler::publish(const char* topic, int qos, bool retain, const char* payload) {
    if (!connected || !client) {
        ESP_LOGW(TAG, "MQTT not connected");
        return false;
    }

    int msg_id = esp_mqtt_client_publish(client, topic, payload, 
                                       strlen(payload), qos, retain);
    return msg_id != -1;
}

bool MqttHandler::is_connected() {
    return connected;
}

bool MqttHandler::send_discovery(const char* component, const char* name, 
                               const char* config) {
    if (!connected) return false;
    
    // Format discovery topic
    char topic[128];
    snprintf(topic, sizeof(topic), "homeassistant/%s/%s/config", 
             component, name);
             
    return publish(topic, 0, true, config);
}

bool MqttHandler::send_telemetry(const char* payload) {
    if (!connected) return false;
    
    return publish("espresense/telemetry", 0, false, payload);
}

void MqttHandler::connect() {
    if (client) {
        esp_mqtt_client_start(client);
    }
}

void MqttHandler::disconnect() {
    if (client) {
        esp_mqtt_client_stop(client);
    }
    connected = false;
}
