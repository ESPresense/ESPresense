#ifdef SENSORS
#include "ENVIV.h"

#include "globals.h"
#include "mqtt.h"
#include "defaults.h"
#include <HeadlessWiFiSettings.h>
#include "string_utils.h"
#include "BMP280m5.h"
#include <SHTSensor.h>

namespace ENVIV
{
    BMP280m5* bmp;
    SHTSensor* sht;
    int ENVIV_I2c_Bus;
    int sht_bus = -1;
    unsigned long previousMillis = 0;
    int sensorInterval = 60000;
    bool bmpInitialized = false;
    bool shtInitialized = false;

    /**
     * @brief Initializes the BMP280 and SHT40 sensors on the configured I2C bus.
     *
     * Attempts to initialize the BMP280 using the M5Stack I2C driver, which requires
     * Wire.end()/Wire.begin() on ESP32-S3 hardware to correctly enumerate the sensor.
     * If successful, configures forced measurement mode. Also attempts to initialize
     * the SHT40 on the configured bus using the Sensirion arduino-sht library.
     */
    void Setup()
    {
        if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;

#if SOC_I2C_NUM > 1
        TwoWire* wire = (ENVIV_I2c_Bus == 1) ? &Wire : &Wire1;
#else
        TwoWire* wire = &Wire;
#endif

        bmp = new BMP280m5();
        if (!bmp->begin(wire, 0x76)) {
            Log.println("[ENVIV BMP280] Couldn't find sensor, check wiring and I2C address!");
        } else {
            bmpInitialized = true;
            bmp->setSampling(
                BMP280m5::MODE_FORCED,
                BMP280m5::SAMPLING_X1,
                BMP280m5::SAMPLING_X1,
                BMP280m5::FILTER_OFF
            );
        }

        // Setup SHT40
        if (sht_bus < 1 || sht_bus > 2) return;
        sht = new SHTSensor();
#if SOC_I2C_NUM > 1
        shtInitialized = sht->init(sht_bus == 1 ? Wire : Wire1);
#else
        shtInitialized = sht->init(Wire);
#endif
        if (!shtInitialized) {
            Log.println("[ENVIV SHT40] Couldn't find sensor, check wiring and I2C address!");
        } else {
            sht->setAccuracy(SHTSensor::SHT_ACCURACY_MEDIUM);
        }
    }

    void ConnectToWifi()
    {
#if SOC_I2C_NUM > 1
        ENVIV_I2c_Bus = HeadlessWiFiSettings.integer("ENVIV_I2c_Bus", 1, 2, DEFAULT_I2C_BUS, "ENVIV I2C Bus");
        sht_bus = HeadlessWiFiSettings.integer("ENVIV_SHT_Bus", -1, 2, DEFAULT_I2C_BUS, "ENVIV SHT40 I2C Bus (-1 to disable)");
        // Clamp to available I2C bus count
        if (ENVIV_I2c_Bus > SOC_I2C_NUM) ENVIV_I2c_Bus = SOC_I2C_NUM;
        if (sht_bus > SOC_I2C_NUM) sht_bus = SOC_I2C_NUM;
#else
        ENVIV_I2c_Bus = 1;
        sht_bus = HeadlessWiFiSettings.integer("ENVIV_SHT_Bus", -1, 1, 1, "ENVIV SHT40 I2C Bus (-1 to disable)");
#endif
    }

    /**
     * @brief Reports the configured ENVIV sensor buses to the log.
     */
    void SerialReport()
    {
        if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
        Log.println("ENVIV BMP280: 0x76 on bus " + String(ENVIV_I2c_Bus));
        if (sht_bus >= 1 && sht_bus <= 2)
            Log.println("ENVIV SHT40:  Auto-detect on bus " + String(sht_bus));
    }

    /**
     * @brief Sends Home Assistant MQTT discovery messages for initialized sensors.
     *
     * Only sends discovery for sensors that successfully initialized during Setup().
     */
    bool SendDiscovery()
    {
        bool result = true;
        if (bmpInitialized)
            result = result
                && sendSensorDiscovery("BMP280 Temperature", EC_NONE, "temperature", "°C")
                && sendSensorDiscovery("BMP280 Pressure", EC_NONE, "pressure", "hPa");
        if (shtInitialized)
            result = result
                && sendSensorDiscovery("SHT40 Temperature", EC_NONE, "temperature", "°C")
                && sendSensorDiscovery("SHT40 Humidity", EC_NONE, "humidity", "%");
        return result;
    }

    /**
     * @brief Polls sensors at the configured interval and publishes readings to MQTT.
     *
     * Publishes bmp280_temperature and bmp280_pressure from the BMP280, and
     * sht40_temperature and sht40_humidity from the SHT40.
     */
    void Loop()
    {
        if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
        if (!bmpInitialized && !shtInitialized) return;

        if (previousMillis == 0 || millis() - previousMillis >= sensorInterval) {

            if (bmpInitialized) {
                if (bmp->takeForcedMeasurement()) {
                    float temperature = bmp->readTemperature();
                    float pressure = bmp->readPressure() / 100.0F;
                    pub((roomsTopic + "/bmp280_temperature").c_str(), 0, 1, String(temperature).c_str());
                    pub((roomsTopic + "/bmp280_pressure").c_str(), 0, 1, String(pressure).c_str());
                }
            }

            if (shtInitialized) {
                if (sht->readSample()) {
                    pub((roomsTopic + "/sht40_temperature").c_str(), 0, 1, String(sht->getTemperature()).c_str());
                    pub((roomsTopic + "/sht40_humidity").c_str(), 0, 1, String(sht->getHumidity()).c_str());
                }
            }

            previousMillis = millis();
        }
    }
}

#endif
