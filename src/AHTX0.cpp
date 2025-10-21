#ifdef SENSORS
#include "AHTX0.h"

#include "globals.h"
#include "mqtt.h"
#include "defaults.h"
#include <HeadlessWiFiSettings.h>
#include "string_utils.h"

#include <Adafruit_AHTX0.h>

namespace AHTX0
{
    Adafruit_AHTX0* aht;
    long AHTX0_status;
    String AHTX0_I2c;
    int AHTX0_I2c_Bus;
    unsigned long AHTX0PreviousMillis = 0;
    int sensorInterval = 60000;
    bool initialized = false;

    /**
     * @brief Initialize the AHTX0 temperature/humidity sensor using configured I2C settings.
     *
     * Attempts to create and start the AHTX0 sensor on the configured I2C bus and address.
     * If initialization succeeds, sets the module's initialized flag to true.
     * If no I2C bus is active or the configured address is unsupported, the function returns without action.
     * On initialization failure, logs an error message indicating the sensor was not found.
     */
    void Setup()
    {
        if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;

        aht = new Adafruit_AHTX0();
        if (AHTX0_I2c == "0x38") {
            AHTX0_status = aht->begin(AHTX0_I2c_Bus == 1 ? &Wire : &Wire1, 0x38);
        } else if (AHTX0_I2c == "0x39") {
            AHTX0_status = aht->begin(AHTX0_I2c_Bus == 1 ? &Wire : &Wire1, 0x39);
        } else {
            return;
        }

        if (!AHTX0_status) {
            Log.println("[AHTX0] Couldn't find a sensor, check your wiring and I2C address!");
        } else {
            initialized = true;
        }
    }

    void ConnectToWifi()
    {
        AHTX0_I2c_Bus = HeadlessWiFiSettings.integer("AHTX0_I2c_Bus", 1, 2, DEFAULT_I2C_BUS, "I2C Bus");
        AHTX0_I2c = HeadlessWiFiSettings.string("AHTX0_I2c", "", "I2C address (0x38 or 0x39)");
    }

    /**
     * @brief Logs the configured AHTX0 sensor I2C address and bus.
     *
     * If neither I2C bus has been started or the configured I2C address string is empty, the function does nothing.
     * Otherwise it writes a single diagnostic line in the form "AHTX0_I2c Sensor: <address> on bus <bus>" to the Log.
     */
    void SerialReport()
    {
        if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
        if (AHTX0_I2c.isEmpty()) return;
        Log.print("AHTX0_I2c Sensor: ");
        Log.println(AHTX0_I2c + " on bus " + AHTX0_I2c_Bus);
    }

    /**
     * @brief Periodically reads the AHTX0 sensor and publishes temperature and humidity.
     *
     * If neither I2C bus has started or the sensor was not initialized, the function returns immediately.
     * When called and the configured interval has elapsed (or on first run), it reads temperature and
     * relative humidity from the AHTX0 sensor and publishes the values to MQTT topics:
     * roomsTopic + "/ahtx0_temperature" and roomsTopic + "/ahtx0_humidity".
     * The function also updates the internal timestamp used to enforce the read interval.
     *
     * @note Published messages use QoS 1 and are retained.
     */
    void Loop()
    {
        if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
        if (!initialized) return;

        if (AHTX0PreviousMillis == 0 || millis() - AHTX0PreviousMillis >= sensorInterval) {
            sensors_event_t humidity, temp;
            aht->getEvent(&humidity, &temp);

            pub((roomsTopic + "/ahtx0_temperature").c_str(), 0, 1, String(temp.temperature).c_str());
            pub((roomsTopic + "/ahtx0_humidity").c_str(), 0, 1, String(humidity.relative_humidity).c_str());

            AHTX0PreviousMillis = millis();
        }
    }

    bool SendDiscovery()
    {
        if (AHTX0_I2c.isEmpty()) return true;

        return sendSensorDiscovery("AHTX0 Temperature", EC_NONE, "temperature", "°C")
            && sendSensorDiscovery("AHTX0 Humidity", EC_NONE, "humidity", "%");
    }
}

#endif