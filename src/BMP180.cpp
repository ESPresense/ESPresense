#ifdef SENSORS
#include "BMP180.h"

#include "globals.h"
#include "mqtt.h"
#include "defaults.h"
#include <AsyncWiFiSettings.h>
#include "string_utils.h"

#include <Adafruit_BMP085.h>

namespace BMP180
{
    Adafruit_BMP085* bmp;
    long BMP180_status;
    String BMP180_I2c;
    int BMP180_I2c_Bus;
    unsigned long BMP180PreviousMillis = 0;
    int sensorInterval = 60000;
    bool initialized = false;

    void Setup()
    {
        if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;

        bmp = new Adafruit_BMP085();
        if (BMP180_I2c == "0x77") {
            BMP180_status = bmp->begin(BMP085_STANDARD, BMP180_I2c_Bus == 1 ? &Wire : &Wire1);
        } else {
            return;
        }

        if (!BMP180_status) {
            Serial.println("[BMP180] Couldn't find a sensor, check your wiring and I2C address!");
        } else {
            initialized = true;
        }
    }

    void ConnectToWifi()
    {
        BMP180_I2c_Bus = AsyncWiFiSettings.integer("BMP180_I2c_Bus", 1, 2, DEFAULT_I2C_BUS, "I2C Bus");
        BMP180_I2c = AsyncWiFiSettings.string("BMP180_I2c", "", "I2C address (0x77)");
    }

    void SerialReport()
    {
        if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
        if (BMP180_I2c.isEmpty()) return;
        Serial.print("BMP180:       ");
        Serial.println(BMP180_I2c + " on bus " + BMP180_I2c_Bus);
    }

    void Loop()
    {
        if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
        if (!initialized) return;

        if (BMP180PreviousMillis == 0 || millis() - BMP180PreviousMillis >= sensorInterval) {

            float temperature = bmp->readTemperature();
            float pressure = bmp->readPressure() / 100.0F;

            pub((roomsTopic + "/bmp180_temperature").c_str(), 0, 1, String(temperature).c_str());
            pub((roomsTopic + "/bmp180_pressure").c_str(), 0, 1, String(pressure).c_str());

            BMP180PreviousMillis = millis();
        }
    }

    bool SendDiscovery()
    {
        if (BMP180_I2c.isEmpty()) return true;

        return sendSensorDiscovery("BMP180 Temperature", EC_NONE, "temperature", "°C")
            && sendSensorDiscovery("BMP180 Pressure", EC_NONE, "pressure", "hPa");
    }
}

#endif
