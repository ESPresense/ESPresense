#ifdef SENSORS
#include "SHT30.h"

#include "globals.h"
#include "mqtt.h"
#include "defaults.h"
#include <AsyncWiFiSettings.h>
#include "string_utils.h"

#include <Adafruit_SHT31.h>

namespace SHT30
{
    Adafruit_SHT31* sht30;
    long SHT30_status;
    String SHT30_I2c;
    int SHT30_I2c_Bus;
    unsigned long SHT30PreviousMillis = 0;
    int sensorInterval = 60000;
    bool initialized = false;

    void Setup()
    {
        if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;

        sht30 = new Adafruit_SHT31(SHT30_I2c_Bus == 1 ? &Wire : &Wire1);
        if (SHT30_I2c == "0x44") {
            SHT30_status = sht30->begin(0x44);
        } else if (SHT30_I2c == "0x45") {
            SHT30_status = sht30->begin(0x45);
        } else {
            return;
        }

        if (!SHT30_status) {
            Serial.println("[SHT30] Couldn't find a sensor, check your wiring and I2C address!");
        } else {
            initialized = true;
        }
    }

    void ConnectToWifi()
    {
        AsyncWiFiSettings.html("h4", "SHT30/31 - Temperature and Humidity Sensor:");
        SHT30_I2c_Bus = AsyncWiFiSettings.integer("SHT30_I2c_Bus", 1, 2, DEFAULT_I2C_BUS, "I2C Bus");
        SHT30_I2c = AsyncWiFiSettings.string("SHT30_I2c", "", "I2C address (0x77)");
    }

    void SerialReport()
    {
        if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
        if (SHT30_I2c.isEmpty()) return;
        Serial.print("SHT30:       ");
        Serial.println(SHT30_I2c + " on bus " + SHT30_I2c_Bus);
    }

    void Loop()
    {
        if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
        if (!initialized) return;

        if (SHT30PreviousMillis == 0 || millis() - SHT30PreviousMillis >= sensorInterval) {

            float temperature = sht30->readTemperature();
            float humidity = sht30->readHumidity();

            mqttClient.publish((roomsTopic + "/sht30_temperature").c_str(), 0, 1, String(temperature).c_str());
            mqttClient.publish((roomsTopic + "/sht30_humidity").c_str(), 0, 1, String(humidity).c_str());

            SHT30PreviousMillis = millis();
        }
    }

    bool SendDiscovery()
    {
        if (SHT30_I2c.isEmpty()) return true;

        return sendSensorDiscovery("SHT30 Temperature", EC_NONE, "temperature", "°C")
            && sendSensorDiscovery("SHT30 Humidity", EC_NONE, "humidity", "%");
    }
}

#endif
