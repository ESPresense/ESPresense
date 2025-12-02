#ifdef SENSORS
#include "HX711.h"

#include "globals.h"
#include "mqtt.h"
#include "defaults.h"
#include <HeadlessWiFiSettings.h>
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
        sckPin = HeadlessWiFiSettings.integer("HX711_sckPin", 0, "HX711 SCK (Clock) pin");
        doutPin = HeadlessWiFiSettings.integer("HX711_doutPin", 0, "HX711 DOUT (Data) pin");
    }

    /**
     * @brief Logs configured HX711 clock and data pin numbers.
     *
     * If the sensor pins are configured, writes a single log line containing the SCK/DOUT pin pair in the format "sck/dout".
     */
    void SerialReport()
    {
        if (!sckPin && !doutPin) return;
        Log.print("HX711 Weight Sensor: ");
        Log.println(String(sckPin) + "/" + String(doutPin));
    }

    /**
     * @brief Periodically reads raw 24-bit values from the configured HX711 load cell and publishes them.
     *
     * When pins are configured and the device is ready, this function reads a 24-bit sample from the HX711,
     * applies the configured gain cycles, sign-extends the 24-bit value to a 32-bit unsigned representation,
     * updates the internal read timestamp, and publishes the numeric value as a string to the MQTT topic
     * "<roomsTopic>/raw_weight" with QoS 0 and the retained flag set.
     *
     * The function returns immediately if pins are not configured, if the minimum interval since the last read
     * has not elapsed, or if the HX711 indicates it is not ready.
     */
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