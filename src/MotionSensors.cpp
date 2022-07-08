#include "MotionSensors.h"
#include <WiFiSettings.h>
#include <AsyncMqttClient.h>
#include "globals.h"
#include "mqtt.h"
#include "string_utils.h"
#include "defaults.h"
#include "GUI.h"

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

    static void pirLoop()
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

    static void radarLoop()
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

    void Loop()
    {
        pirLoop();
        radarLoop();
        int motionValue = (lastRadarValue == HIGH || lastPirValue == HIGH) ? HIGH : LOW;
        if (lastMotionValue == motionValue) return;
        mqttClient.publish((roomsTopic + "/motion").c_str(), 0, true, motionValue == HIGH ? "ON" : "OFF");
        lastMotionValue = motionValue;
    }

    bool SendDiscovery()
    {
        if (!pirPin && !radarPin) return true;

        if (pirPin) if (!sendNumberDiscovery("Pir Timeout", EC_CONFIG)) return false;
        if (radarPin) if (!sendNumberDiscovery("Radar Timeout", EC_CONFIG)) return false;
        return sendSensorDiscovery("Motion", "", "", "motion");
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

    bool SendOnline()
    {
        return pub((roomsTopic + "/pir_timeout").c_str(), 0, true, String(pirTimeout).c_str())
               && pub((roomsTopic + "/radar_timeout").c_str(), 0, true, String(radarTimeout).c_str());
    }
}
