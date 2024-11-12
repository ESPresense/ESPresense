#pragma once

#include <string>
#include "mqtt_client.h"

class MqttHandler {
public:
    static void init(const std::string& host, uint16_t port, 
                    const std::string& username, const std::string& password);
    static bool publish(const char* topic, int qos, bool retain, const char* payload);
    static bool is_connected();
    
    // Discovery related functions
    static bool send_discovery(const char* component, const char* name, const char* config);
    static bool send_telemetry(const char* payload);
    
private:
    static esp_mqtt_client_handle_t client;
    static bool connected;
    static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);
    static void connect();
    static void disconnect();
};
