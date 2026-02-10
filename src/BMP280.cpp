#ifdef SENSORS
#include "BMP280.h"

#include "globals.h"
#include "mqtt.h"
#include "defaults.h"
#include <HeadlessWiFiSettings.h>
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

    /**
     * @brief Initializes the BMP280 sensor if an I2C bus and valid address are configured.
     *
     * Attempts to create and initialize the Adafruit BMP280 on the selected I2C bus (1 or 2)
     * using the configured address "0x76" or "0x77". On success, sets `initialized` to true,
     * stores the initialization result in `BMP280_status`, and configures forced measurement
     * sampling for temperature and pressure. If initialization fails or no valid address/bus
     * is available, the function returns without initializing the sensor and logs an error.
     */
    void Setup()
    {
        if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;

        bmp = new Adafruit_BMP280(
#if SOC_I2C_NUM > 1
            BMP280_I2c_Bus == 1 ? &Wire : &Wire1
#else
            &Wire
#endif
        );
        if (BMP280_I2c == "0x76") {
            BMP280_status = bmp->begin(0x76);
        } else if (BMP280_I2c == "0x77") {
            BMP280_status = bmp->begin(0x77);
        } else {
            return;
        }

        if (!BMP280_status) {
            Log.println("[BMP280] Couldn't find a sensor, check your wiring and I2C address!");
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
        BMP280_I2c_Bus = HeadlessWiFiSettings.integer("BMP280_I2c_Bus", 1, 2, DEFAULT_I2C_BUS, "I2C Bus");
        BMP280_I2c = HeadlessWiFiSettings.string("BMP280_I2c", "", "I2C address (0x76 or 0x77)");
    }

    /**
     * @brief Reports the configured BMP280 I2C address and bus to the log.
     *
     * If an I2C bus is active and an address is configured, writes a single line
     * containing the BMP280 I2C address and bus number to the Log. Does nothing
     * if no I2C bus is started or if the BMP280 address is empty.
     */
    void SerialReport()
    {
        if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
        if (BMP280_I2c.isEmpty()) return;
        Log.print("BMP280:       ");
        Log.println(BMP280_I2c + " on bus " + BMP280_I2c_Bus);
    }

    /**
     * @brief Polls the BMP280 sensor at configured intervals and publishes temperature and pressure.
     *
     * If no I2C bus is started or the sensor is not initialized, the function returns without action.
     * When the configured interval has elapsed (or on the first eligible call), it performs a forced
     * measurement, reads temperature and pressure (pressure converted to hPa), publishes the values to
     * roomsTopic+"/bmp280_temperature" and roomsTopic+"/bmp280_pressure", and updates the last-measurement timestamp.
     */
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