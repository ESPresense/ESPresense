#pragma once
#include <Arduino.h>

const char *const EC_DIAGNOSTIC = "diagnostic";
const char *const EC_CONFIG = "config";
const char *const EC_NONE = "";

static const char *const DEVICE_CLASS_NONE = "";

bool pub(const char *topic, uint8_t qos, bool retain, const char *payload, size_t length = 0, bool dup = false, uint16_t message_id = 0);
void commonDiscovery();

bool sendConnectivityDiscovery();

bool sendTeleBinarySensorDiscovery(const String &name, const String &entityCategory, const String &temp, const String &devClass = DEVICE_CLASS_NONE);
bool sendTeleSensorDiscovery(const String &name, const String &entityCategory, const String &temp, const String &devClass = DEVICE_CLASS_NONE, const String &units = "");

bool sendBinarySensorDiscovery(const String &name, const String &entityCategory, const String &devClass = DEVICE_CLASS_NONE);
bool sendSensorDiscovery(const String &name, const String &entityCategory, const String &devClass = DEVICE_CLASS_NONE, const String &units = "", bool frcUpdate = false);

bool sendButtonDiscovery(const String &name, const String &entityCategory);
bool sendSwitchDiscovery(const String &name, const String &entityCategory);
bool sendNumberDiscovery(const String &name, const String &entityCategory);
bool sendLightDiscovery(const String &name, const String &entityCategory, bool rgb);

bool sendDeleteDiscovery(const String &domain, const String &name);

bool alias(const String &alias, const String &id, const String &name);
