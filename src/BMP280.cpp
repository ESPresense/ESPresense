#ifdef SENSORS
#include "BMP280.h"

#include "globals.h"
#include "mqtt.h"
#include "defaults.h"
#include <AsyncWiFiSettings.h>
#include "string_utils.h"

#include <Adafruit_BMP280.h>

namespace BMP280
{
    Adafruit_BMP280* bmp;
    long BMP280_status;
    String BMP280_I2c;
    int BMP280_I2c_Bus;
    unsigned long BMP280PreviousMillis = 0;
    int sensorInterval = 60000;
    bool initialized = false;

    void Setup()
    {
        if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;

        bmp = new Adafruit_BMP280(BMP280_I2c_Bus == 1 ? &Wire : &Wire1);
        if (BMP280_I2c == "0x76") {
            BMP280_status = bmp->begin(0x76);
        } else if (BMP280_I2c == "0x77") {
            BMP280_status = bmp->begin(0x77);
        } else {
            return;
        }

        if (!BMP280_status) {
            Serial.println("[BMP280] Couldn't find a sensor, check your wiring and I2C address!");
        } else {
            initialized = true;
        }

        bmp->setSampling(
            Adafruit_BMP280::MODE_FORCED,
            Adafruit_BMP280::SAMPLING_X1,  // Temperature
            Adafruit_BMP280::SAMPLING_X1,  // Pressure
            Adafruit_BMP280::FILTER_OFF
        );
    }

    void ConnectToWifi()
    {
        BMP280_I2c_Bus = AsyncWiFiSettings.integer("BMP280_I2c_Bus", 1, 2, DEFAULT_I2C_BUS, "I2C Bus");
        BMP280_I2c = AsyncWiFiSettings.string("BMP280_I2c", "", "I2C address (0x76 or 0x77)");
    }

    void SerialReport()
    {
        if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
        if (BMP280_I2c.isEmpty()) return;
        Serial.print("BMP280:       ");
        Serial.println(BMP280_I2c + " on bus " + BMP280_I2c_Bus);
    }

    void Loop()
    {
        if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
        if (!initialized) return;

        if (BMP280PreviousMillis == 0 || millis() - BMP280PreviousMillis >= sensorInterval) {

            bmp->takeForcedMeasurement();
            float temperature = bmp->readTemperature();
            float pressure = bmp->readPressure() / 100.0F;

            pub((roomsTopic + "/bmp280_temperature").c_str(), 0, 1, String(temperature).c_str());
            pub((roomsTopic + "/bmp280_pressure").c_str(), 0, 1, String(pressure).c_str());

            BMP280PreviousMillis = millis();
        }
    }

    bool SendDiscovery()
    {
        if (BMP280_I2c.isEmpty()) return true;

        return sendSensorDiscovery("BMP280 Temperature", EC_NONE, "temperature", "°C")
            && sendSensorDiscovery("BMP280 Pressure", EC_NONE, "pressure", "hPa");
    }
}

#endif
