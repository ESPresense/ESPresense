#ifdef SENSORS
#include "BMP180.h"

#include "globals.h"
#include "mqtt.h"
#include "defaults.h"
#include <HeadlessWiFiSettings.h>
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

    /**
     * @brief Initialize the BMP180 sensor if an I2C bus and valid address are configured.
     *
     * Attempts to allocate and begin the Adafruit_BMP085 sensor using the selected I2C bus.
     * If the sensor is successfully started, the module-level `initialized` flag is set to true.
     * If the sensor cannot be found, an error message is logged.
     *
     * This function returns immediately if neither I2C bus is started or if the configured I2C
     * address is not the expected value for this sensor.
     */
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
            Log.println("[BMP180] Couldn't find a sensor, check your wiring and I2C address!");
        } else {
            initialized = true;
        }
    }

    void ConnectToWifi()
    {
        BMP180_I2c_Bus = HeadlessWiFiSettings.integer("BMP180_I2c_Bus", 1, 2, DEFAULT_I2C_BUS, "I2C Bus");
        BMP180_I2c = HeadlessWiFiSettings.string("BMP180_I2c", "", "I2C address (0x77)");
    }

    /**
     * @brief Logs the BMP180 sensor I2C address and selected I2C bus.
     *
     * If neither I2C bus has started or the configured I2C address is empty, the function does nothing.
     */
    void SerialReport()
    {
        if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
        if (BMP180_I2c.isEmpty()) return;
        Log.print("BMP180:       ");
        Log.println(BMP180_I2c + " on bus " + BMP180_I2c_Bus);
    }

    /**
     * @brief Periodically reads temperature and pressure from the BMP180 sensor and publishes them to MQTT.
     *
     * When an I2C bus is available and the sensor has been initialized, this function reads the sensor at intervals defined by `sensorInterval` (or immediately on first run), publishes temperature (°C) to the topic "<roomsTopic>/bmp180_temperature" and pressure (hPa) to the topic "<roomsTopic>/bmp180_pressure", and updates `BMP180PreviousMillis` to the current time. If neither I2C bus is started or the sensor is not initialized, the function returns without performing any action.
     */
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