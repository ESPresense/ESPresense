#ifdef SENSORS
#include "BME280.h"

#include "globals.h"
#include "mqtt.h"
#include "defaults.h"
#include <HeadlessWiFiSettings.h>
#include "string_utils.h"

#include <Adafruit_BME280.h>

namespace BME280
{
    Adafruit_BME280 BME280;
    String BME280_I2c;
    int BME280_I2c_Bus;
    unsigned long bme280PreviousMillis = 0;
    int sensorInterval = 60000;
    bool initialized = false;

    /**
     * @brief Initializes the BME280 sensor according to configured I2C address and bus.
     *
     * Attempts to begin communication with the BME280 using the configured I2C address and bus.
     * If initialization succeeds, marks the sensor as initialized and configures it for forced-mode,
     * single-sample readings for temperature, pressure, and humidity with the hardware filter disabled.
     * If initialization fails, logs an error message.
     */
    void Setup()
    {
        if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;

        bool ok = false;
        if (BME280_I2c == "0x76" && BME280_I2c_Bus == 1) {
            ok = BME280.begin(0x76, &Wire);
        } else if (BME280_I2c == "0x77" && BME280_I2c_Bus == 1) {
            ok = BME280.begin(0x77, &Wire);
        }
#if SOC_I2C_NUM > 1
        else if (BME280_I2c == "0x76" && BME280_I2c_Bus == 2) {
            ok = BME280.begin(0x76, &Wire1);
        } else if (BME280_I2c == "0x77" && BME280_I2c_Bus == 2) {
            ok = BME280.begin(0x77, &Wire1);
        }
#endif
        else {
            return;
        }

        if (!ok) {
            Log.println("[BME280] Couldn't find a sensor, check your wiring and I2C address!");
            return;
        }

        BME280.setSampling(
            Adafruit_BME280::MODE_FORCED,
            Adafruit_BME280::SAMPLING_X1,  // Temperature
            Adafruit_BME280::SAMPLING_X1,  // Pressure
            Adafruit_BME280::SAMPLING_X1,  // Humidity
            Adafruit_BME280::FILTER_OFF
        );

        initialized = true;
    }

    void ConnectToWifi()
    {
        BME280_I2c_Bus = HeadlessWiFiSettings.integer("BME280_I2c_Bus", 1, 2, DEFAULT_I2C_BUS, "I2C Bus");
        BME280_I2c = HeadlessWiFiSettings.string("BME280_I2c", "", "I2C address (0x76 or 0x77)");
    }

    /**
     * @brief Logs the BME280 sensor I2C address and bus to the system logger when available.
     *
     * If neither I2C bus is started or the configured I2C address is empty, the function does nothing.
     */
    void SerialReport()
    {
        if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
        if (BME280_I2c.isEmpty()) return;
        Log.print("BME280:       ");
        Log.println(BME280_I2c + " on bus " + BME280_I2c_Bus);
    }

    /**
     * @brief Periodically samples the BME280 sensor and publishes measurements to MQTT.
     *
     * If neither I2C bus is started or the sensor is not initialized, the function returns immediately.
     * When the configured sampling interval has elapsed, it triggers a forced measurement, reads
     * temperature, humidity, and pressure (converted to hPa), publishes each reading to the room
     * topics "/bme280_temperature", "/bme280_humidity", and "/bme280_pressure", and updates the
     * internal timestamp for the last sample.
     */
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
