#include "MotionSensors.h"
#include <WiFiSettings.h>
#include <AsyncMqttClient.h>

// for #define ESPMAC
#include "string_utils.h"
#include "defaults.h"
#include "GUI.h"

// TODO: Not a fan of externs, but this helps refactoring for now
extern char buffer[2048];
extern String room;
extern String roomsTopic;
extern void commonDiscovery();
extern bool sendNumberDiscovery(const String& name, const String& entityCategory);
extern bool pub(const char *topic, uint8_t qos, bool retain, const char *payload, size_t length = 0, bool dup = false, uint16_t message_id = 0);
extern bool spurt(const String& fn, const String& content);

namespace Motion
{
    int lastMotionValue = -1;

    int pirPin = 0;
    float pirTimeout = 0;
    int lastPirValue = -1;
    unsigned long lastPirMilli = 0;

    int radarPin = 0;
    float radarTimeout = 0;
    int lastRadarValue = -1;
    unsigned long lastRadarMilli = 0;

    void Setup()
    {
        if (pirPin)
            pinMode(pirPin, INPUT_PULLUP);
        if (radarPin)
            pinMode(radarPin, INPUT_PULLUP);
    }

    void ConnectToWifi()
    {
        pirPin = WiFiSettings.integer("pir_pin", 0, "PIR motion pin (0 for disable)");
        pirTimeout = WiFiSettings.floating("pir_timeout", 0, 300, DEFAULT_DEBOUNCE_TIMEOUT, "PIR motion timeout (in seconds)");
        radarPin = WiFiSettings.integer("radar_pin", 0, "Radar motion pin (0 for disable)");
        radarTimeout = WiFiSettings.floating("radar_timeout", 0, 300, DEFAULT_DEBOUNCE_TIMEOUT, "Radar motion timeout (in seconds)");
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
        bool detected = digitalRead(pirPin) == HIGH;
        if (detected) lastPirMilli = millis();
        unsigned long since = millis() - lastPirMilli;
        int pirValue = (detected || since < (pirTimeout * 1000)) ? HIGH : LOW;

        if (lastPirValue == pirValue) return;
        mqttClient.publish((roomsTopic + "/pir").c_str(), 0, true, pirValue == HIGH ? "ON" : "OFF");
        GUI::pir(pirValue == HIGH);
        lastPirValue = pirValue;
    }

    static void RadarLoop(AsyncMqttClient& mqttClient)
    {
        if (!radarPin) return;
        bool detected = digitalRead(radarPin) == HIGH;
        if (detected) lastRadarMilli = millis();
        unsigned long since = millis() - lastRadarMilli;
        int radarValue = (detected || since < (radarTimeout * 1000)) ? HIGH : LOW;

        if (lastRadarValue == radarValue) return;
        mqttClient.publish((roomsTopic + "/radar").c_str(), 0, true, radarValue == HIGH ? "ON" : "OFF");
        GUI::radar(radarValue == HIGH);
        lastRadarValue = radarValue;
    }

    void Loop(AsyncMqttClient& mqttClient)
    {
        PirLoop(mqttClient);
        RadarLoop(mqttClient);
        int motionValue = (lastRadarValue == HIGH || lastPirValue == HIGH) ? HIGH : LOW;
        if (lastMotionValue == motionValue) return;
        mqttClient.publish((roomsTopic + "/motion").c_str(), 0, true, motionValue == HIGH ? "ON" : "OFF");
        lastMotionValue = motionValue;
    }

    bool SendDiscovery(DynamicJsonDocument& doc)
    {
        if (!pirPin && !radarPin)
            return true;

        commonDiscovery();
        doc["~"] = roomsTopic;
        doc["name"] = "ESPresense " + room + " Motion";
        doc["uniq_id"] = Sprintf("espresense_%06lx_motion", CHIPID);
        doc["avty_t"] = "~/status";
        doc["stat_t"] = "~/motion";
        doc["dev_cla"] = "motion";

        serializeJson(doc, buffer);
        String discoveryTopic = "homeassistant/binary_sensor/espresense_" + ESPMAC + "/motion/config";

        if (!pub(discoveryTopic.c_str(), 0, true, buffer)) return false;
        return (!pirPin || sendNumberDiscovery("Pir Timeout", EC_CONFIG)) && (!radarPin || sendNumberDiscovery("Radar Timeout", EC_CONFIG));
    }

    bool Command(String& command, String& pay)
    {
        if (command == "pir_timeout")
        {
            pirTimeout = pay.toInt();
            spurt("/pir_timeout", pay);
        }
        else if (command == "radar_timeout")
        {
            radarTimeout = pay.toInt();
            spurt("/radar_timeout", pay);
        }
        else return false;
        return true;
    }

    bool SendOnline(DynamicJsonDocument& doc)
    {
        return pub((roomsTopic + "/pir_timeout").c_str(), 0, true, String(pirTimeout).c_str())
               && pub((roomsTopic + "/radar_timeout").c_str(), 0, true, String(radarTimeout).c_str());
    }
}
