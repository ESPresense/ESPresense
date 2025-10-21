#ifdef SENSORS

#include "globals.h"
#include "defaults.h"
#include "mqtt.h"
#include "TSL2561.h"
#include <HeadlessWiFiSettings.h>
#include <AsyncMqttClient.h>
#include <Adafruit_TSL2561_U.h>
#include "string_utils.h"

namespace TSL2561
{
    String TSL2561_I2c;
    int TSL2561_I2c_Bus;
    String TSL2561_I2c_Gain;
    unsigned long tsl2561PreviousMillis = 0;
    int sensorInterval = 60000;

    void Setup()
    {
    }

    void ConnectToWifi()
    {
        TSL2561_I2c_Bus = HeadlessWiFiSettings.integer("TSL2561_I2c_Bus", 1, 2, DEFAULT_I2C_BUS, "I2C Bus");
        TSL2561_I2c = HeadlessWiFiSettings.string("TSL2561_I2c", "", "I2C address (0x39, 0x49 or 0x29)");
        TSL2561_I2c_Gain = HeadlessWiFiSettings.string("TSL2561_I2c_Gain", DEFAULT_TSL2561_I2C_GAIN, "Gain (auto, 1x or 16x)");
    }

    /**
     * @brief Logs the configured TSL2561 I2C address and selected I2C bus.
     *
     * If neither I2C bus is started or the configured I2C address is empty, this function does nothing.
     */
    void SerialReport()
    {
        if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
        if (TSL2561_I2c.isEmpty()) return;
        Log.print("TSL2561:      ");
        Log.println(TSL2561_I2c + " on bus " + TSL2561_I2c_Bus);
    }

    /**
     * @brief Reads the TSL2561 ambient light sensor and publishes periodic illuminance readings.
     *
     * Initializes the sensor on the configured I2C bus and gain, sets a 402 ms integration
     * time, and reads the current illuminance. If a nonzero lux value is read and the
     * configured publish interval has elapsed, the lux value is published to
     * "<roomsTopic>/tsl2561_lux". If the configured I2C address or gain is invalid the
     * function returns without publishing; if the sensor reports overload (zero lux) this
     * condition is logged.
     */
    void Loop()
    {
        if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;

        int tsl2561_address;

        if (TSL2561_I2c == "0x39") {
            tsl2561_address = 0x39;
        } else if (TSL2561_I2c == "0x29") {
            tsl2561_address = 0x29;
        } else if (TSL2561_I2c == "0x49") {
            tsl2561_address = 0x49;
        } else {
            return;
        }

        Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(tsl2561_address);

        if (TSL2561_I2c_Bus == 1) {
            tsl.begin(&Wire);
        } else if (TSL2561_I2c_Bus == 2) {
            tsl.begin(&Wire1);
        }

        if (TSL2561_I2c_Gain == "auto") {
            tsl.enableAutoRange(true);
        } else if (TSL2561_I2c_Gain == "1x") {
            tsl.setGain(TSL2561_GAIN_1X);
        } else if (TSL2561_I2c_Gain == "16x") {
            tsl.setGain(TSL2561_GAIN_16X);
        } else {
            Log.println("[TSL2561] Invalid gain");
            return;
        }

        tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_402MS);

        sensors_event_t event;
        tsl.getEvent(&event);

        if (event.light) {
            if (millis() - tsl2561PreviousMillis >= sensorInterval) {
                pub((roomsTopic + "/tsl2561_lux").c_str(), 0, 1, String(event.light).c_str());

                tsl2561PreviousMillis = millis();
            }
        } else {
            Log.println("[TSL2561] Sensor overloaded");
        }
    }

    bool SendDiscovery()
    {
        if (TSL2561_I2c.isEmpty()) return true;

        return sendSensorDiscovery("TSL2561 Lux", EC_NONE, "illuminance", "lx");
    }
}

#endif