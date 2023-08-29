#ifdef SENSORS
#include "BME280.h"

#include "globals.h"
#include "mqtt.h"
#include "defaults.h"
#include <AsyncWiFiSettings.h>
#include "string_utils.h"

#include <Adafruit_BME280.h>

namespace BME280
{
    Adafruit_BME280 BME280;
    long BME280_status;
    String BME280_I2c;
    int BME280_I2c_Bus;
    unsigned long bme280PreviousMillis = 0;
    int sensorInterval = 60000;
    bool initialized = false;

    void Setup()
    {
        if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;

        if (BME280_I2c == "0x76" && BME280_I2c_Bus == 1) {
            BME280_status = BME280.begin(0x76, &Wire);
        } else if (BME280_I2c == "0x77" && BME280_I2c_Bus == 1) {
            BME280_status = BME280.begin(0x77, &Wire);
        } else if (BME280_I2c == "0x76" && BME280_I2c_Bus == 2) {
            BME280_status = BME280.begin(0x76, &Wire1);
        } else if (BME280_I2c == "0x77" && BME280_I2c_Bus == 2) {
            BME280_status = BME280.begin(0x77, &Wire1);
        } else {
            return;
        }

        if (!BME280_status) {
            Serial.println("[BME280] Couldn't find a sensor, check your wiring and I2C address!");
        } else {
            initialized = true;
        }

        BME280.setSampling(
            Adafruit_BME280::MODE_FORCED,
            Adafruit_BME280::SAMPLING_X1,  // Temperature
            Adafruit_BME280::SAMPLING_X1,  // Pressure
            Adafruit_BME280::SAMPLING_X1,  // Humidity
            Adafruit_BME280::FILTER_OFF
        );
    }

    void ConnectToWifi()
    {
        BME280_I2c_Bus = AsyncWiFiSettings.integer("BME280_I2c_Bus", 1, 2, DEFAULT_I2C_BUS, "I2C Bus");
        BME280_I2c = AsyncWiFiSettings.string("BME280_I2c", "", "I2C address (0x76 or 0x77)");
    }

    void SerialReport()
    {
        if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
        if (BME280_I2c.isEmpty()) return;
        Serial.print("BME280:       ");
        Serial.println(BME280_I2c + " on bus " + BME280_I2c_Bus);
    }

    void Loop()
    {
        if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
        if (!initialized) return;

        if (millis() - bme280PreviousMillis >= sensorInterval) {

            BME280.takeForcedMeasurement();
            float temperature = BME280.readTemperature();
            float humidity = BME280.readHumidity();
            float pressure = BME280.readPressure() / 100.0F;

            pub((roomsTopic + "/bme280_temperature").c_str(), 0, 1, String(temperature).c_str());
            pub((roomsTopic + "/bme280_humidity").c_str(), 0, 1, String(humidity).c_str());
            pub((roomsTopic + "/bme280_pressure").c_str(), 0, 1, String(pressure).c_str());

            bme280PreviousMillis = millis();
        }
    }

    bool SendDiscovery()
    {
        if (BME280_I2c.isEmpty()) return true;

        return sendSensorDiscovery("BME280 Temperature", EC_NONE, "temperature", "°C")
            && sendSensorDiscovery("BME280 Humidity", EC_NONE, "humidity", "%")
            && sendSensorDiscovery("BME280 Pressure", EC_NONE, "pressure", "hPa");
    }
}

#endif
