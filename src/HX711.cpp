#ifdef SENSORS
#include "HX711.h"

#include "globals.h"
#include "mqtt.h"
#include "defaults.h"
#include <AsyncWiFiSettings.h>
#include <AsyncMqttClient.h>
#include "string_utils.h"

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
        sckPin = AsyncWiFiSettings.integer("HX711_sckPin", 0, "HX711 SCK (Clock) pin");
        doutPin = AsyncWiFiSettings.integer("HX711_doutPin", 0, "HX711 DOUT (Data) pin");
    }

    void SerialReport()
    {
        if (!sckPin && !doutPin) return;
        Serial.print("HX711 Weight Sensor: ");
        Serial.println(String(sckPin) + "/" + String(doutPin));
    }

    void Loop()
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

    bool SendDiscovery()
    {
        if (!sckPin && !doutPin) return true;
        return sendSensorDiscovery("Raw Weight", EC_NONE);
    }
}

#endif
