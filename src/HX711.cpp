#ifdef SENSORS

#include "defaults.h"
#include "HX711.h"
#include <WiFiSettings.h>
#include <AsyncMqttClient.h>

// for #define ESPMAC
#include "strings.h"

// TODO: Not a fan of externs, but this helps refactoring for now
extern char buffer[2048];
extern String room;
extern String roomsTopic;
extern void commonDiscovery();
extern bool pub(const char *topic, uint8_t qos, bool retain, const char *payload, size_t length = 0, bool dup = false, uint16_t message_id = 0);

namespace HX711
{
    int sckPin = 0;
    int doutPin = 0;
    int gain = 1;
    int sensorInterval = 5000;
    unsigned long lastMillis = 0;

    void Setup()
    {
        if (!sckPin && !doutPin) return;
        pinMode(doutPin, INPUT);
        pinMode(sckPin, OUTPUT);
        digitalWrite(sckPin, LOW);
    }

    void ConnectToWifi()
    {
        WiFiSettings.html("h4", "HX711 - Weight Sensor:");
        sckPin = WiFiSettings.integer("HX711_sckPin", 0, "HX711 SCK (Clock) pin");
        doutPin = WiFiSettings.integer("HX711_doutPin", 0, "HX711 DOUT (Data) pin");
    }

    void SerialReport()
    {
        if (!sckPin && !doutPin) return;
        Serial.print("HX711 Weight Sensor: ");
        Serial.println(String(sckPin) + "/" + String(doutPin));
    }

    void Loop(AsyncMqttClient& mqttClient)
    {
        if (!sckPin && !doutPin) return;
        if (millis() - lastMillis < sensorInterval) return;
        if (digitalRead(doutPin) != LOW)
        {
            //Serial1.println("HX711 - Weight Sensor: Not ready yet");
            //HX711 is not ready for new measurements yet!
            return;
        }

        lastMillis = millis();

        uint32_t data = 0;

        for (uint8_t i = 0; i < 24; i++)
        {
            digitalWrite(sckPin, HIGH);
            delayMicroseconds(1);
            data |= uint32_t(digitalRead(doutPin)) << (23 - i);
            digitalWrite(sckPin, LOW);
            delayMicroseconds(1);
        }

        // Cycle clock pin for gain setting
        for (uint8_t i = 0; i < gain; i++)
        {
            digitalWrite(sckPin, HIGH);
            delayMicroseconds(1);
            digitalWrite(sckPin, LOW);
            delayMicroseconds(1);
        }

        if (data & 0x800000ULL) data |= 0xFF000000ULL;

        pub((roomsTopic + "/raw_weight").c_str(), 0, true, String(data).c_str());
    }

    static bool SendWeight(DynamicJsonDocument& doc)
    {
        commonDiscovery();
        doc["~"] = roomsTopic;
        doc["name"] = "ESPresense " + room + " Raw Weight";
        doc["uniq_id"] = Sprintf("espresense_%06" PRIx64 "_raw_weight", ESP.getEfuseMac() >> 24);
        doc["avty_t"] = "~/status";
        doc["stat_t"] = "~/raw_weight";
        doc["frc_upd"] = true;

        serializeJson(doc, buffer);
        String discoveryTopic = "homeassistant/sensor/espresense_" + ESPMAC + "/raw_weight/config";

        return pub(discoveryTopic.c_str(), 0, true, buffer);
    }

    bool SendDiscovery(DynamicJsonDocument& doc)
    {
        if (!sckPin && !doutPin) return true;
        return SendWeight(doc);
    }
}

#endif
