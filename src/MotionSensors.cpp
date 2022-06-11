#include "MotionSensors.h"
#include <WiFiSettings.h>
#include <AsyncMqttClient.h>

// for #define ESPMAC
#include "string_utils.h"

// TODO: Not a fan of externs, but this helps refactoring for now
extern char buffer[2048];
extern String room;
extern String roomsTopic;
extern void commonDiscovery();
extern bool pub(const char *topic, uint8_t qos, bool retain, const char *payload, size_t length = 0, bool dup = false, uint16_t message_id = 0);

namespace Motion
{
    int pirPin = 0;
    int lastPirValue = -1;
    int radarPin = 0;
    int lastRadarValue = -1;

    void Setup()
    {
        if (pirPin) pinMode(pirPin, INPUT_PULLUP);
        if (radarPin) pinMode(radarPin, INPUT_PULLUP);
    }

    void ConnectToWifi()
    {
        pirPin = WiFiSettings.integer("pir_pin", 0, "PIR motion pin (0 for disable)");
        radarPin = WiFiSettings.integer("radar_pin", 0, "Radar motion pin (0 for disable)");
    }

    void SerialReport()
    {
        Serial.print("PIR Sensor:   ");
        Serial.println(pirPin ? "enabled" : "disabled");
        Serial.print("Radar Sensor: ");
        Serial.println(radarPin ? "enabled" : "disabled");
    }

    static void PirLoop(AsyncMqttClient& mqttClient)
    {
        if (!pirPin) return;
        int pirValue = digitalRead(pirPin);

        if (pirValue != lastPirValue)
        {
            if (pirValue == HIGH)
            {
                mqttClient.publish((roomsTopic + "/motion").c_str(), 0, true, "ON");
                Serial.println("PIR MOTION DETECTED!!!");
            }
            else
            {
                mqttClient.publish((roomsTopic + "/motion").c_str(), 0, true, "OFF");
                Serial.println("NO PIR MOTION DETECTED!!!");
            }

            lastPirValue = pirValue;
        }
    }

    static void RadarLoop(AsyncMqttClient& mqttClient)
    {
        if (!radarPin) return;
        int radarValue = digitalRead(radarPin);

        if (radarValue != lastRadarValue)
        {
            if (radarValue == HIGH)
            {
                mqttClient.publish((roomsTopic + "/motion").c_str(), 0, true, "ON");
                Serial.println("Radar MOTION DETECTED!!!");
            }
            else
            {
                mqttClient.publish((roomsTopic + "/motion").c_str(), 0, true, "OFF");
                Serial.println("NO Radar MOTION DETECTED!!!");
            }

            lastRadarValue = radarValue;
        }
    }

    void Loop(AsyncMqttClient& mqttClient)
    {
        PirLoop(mqttClient);
        RadarLoop(mqttClient);
    }

    bool SendDiscovery(DynamicJsonDocument& doc)
    {
        if (!pirPin && !radarPin) return true;

        commonDiscovery();
        doc["~"] = roomsTopic;
        doc["name"] = "ESPresense " + room + " Motion";
        doc["uniq_id"] = Sprintf("espresense_%06" PRIx64 "_motion", ESP.getEfuseMac() >> 24);
        doc["avty_t"] = "~/status";
        doc["stat_t"] = "~/motion";
        doc["dev_cla"] = "motion";

        serializeJson(doc, buffer);
        String discoveryTopic = "homeassistant/binary_sensor/espresense_" + ESPMAC + "/motion/config";

        return pub(discoveryTopic.c_str(), 0, true, buffer);
    }
}
