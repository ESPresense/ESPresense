#pragma once
#include <Arduino.h>

bool pub(const char *topic, uint8_t qos, bool retain, const char *payload, size_t length = 0, bool dup = false, uint16_t message_id = 0);
void commonDiscovery();

bool sendConnectivityDiscovery();

bool sendTeleBinarySensorDiscovery(const String &name, const String &entityCategory, const String &temp, const String &devClass);
bool sendTeleSensorDiscovery(const String &name, const String &entityCategory, const String &temp, const String &units, const String &devClass = "");

bool sendSensorDiscovery(const String &name, const String &entityCategory, const String &units, const String &devClass);
bool sendButtonDiscovery(const String &name, const String &entityCategory);
bool sendSwitchDiscovery(const String &name, const String &entityCategory);
bool sendNumberDiscovery(const String &name, const String &entityCategory);

bool sendDeleteDiscovery(const String &domain, const String &name);
