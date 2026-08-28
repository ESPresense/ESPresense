#pragma once
#include <ArduinoJson.h>

#include <string>

const char* const EC_DIAGNOSTIC = "diagnostic";
const char* const EC_CONFIG = "config";
const char* const EC_NONE = "";

static const char* const DEVICE_CLASS_NONE = "";

namespace Mqtt {
void Setup(const std::string& host, uint16_t port, const std::string& user, const std::string& pass,
           const std::string& clientId, const std::string& willTopic);
bool connected();
void reconnect();
}  // namespace Mqtt

bool pub(const char* topic, uint8_t qos, bool retain, const char* payload, size_t length = 0);
bool pub(const char* topic, uint8_t qos, bool retain, JsonVariantConst jsonDoc);
void commonDiscovery();

bool sendConnectivityDiscovery();

bool sendTeleBinarySensorDiscovery(const std::string& name, const std::string& entityCategory, const std::string& temp, const std::string& devClass = DEVICE_CLASS_NONE);
bool sendTeleSensorDiscovery(const std::string& name, const std::string& entityCategory, const std::string& temp, const std::string& devClass = DEVICE_CLASS_NONE, const std::string& units = "");

bool sendBinarySensorDiscovery(const std::string& name, const std::string& entityCategory, const std::string& devClass = DEVICE_CLASS_NONE);
bool sendSensorDiscovery(const std::string& name, const std::string& entityCategory, const std::string& devClass = DEVICE_CLASS_NONE, const std::string& units = "", bool frcUpdate = false);

bool sendButtonDiscovery(const std::string& name, const std::string& entityCategory);
bool sendSwitchDiscovery(const std::string& name, const std::string& entityCategory);
bool sendNumberDiscovery(const std::string& name, const std::string& entityCategory);
bool sendLightDiscovery(const std::string& name, const std::string& entityCategory, bool rgb, bool rgbw);

bool sendDeleteDiscovery(const std::string& domain, const std::string& name);

bool sendConfig(const std::string& id, const std::string& alias, const std::string& name = "", int calRssi = -128);
bool deleteConfig(const std::string& id);
