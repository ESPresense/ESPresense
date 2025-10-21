#ifdef SENSORS
#include "BH1750.h"

#include "globals.h"
#include "mqtt.h"
#include "defaults.h"
#include <HeadlessWiFiSettings.h>
#include <AsyncMqttClient.h>
#include "string_utils.h"

#include <hp_BH1750.h>

namespace BH1750
{
    hp_BH1750 BH1750;
    unsigned long ms_BH1750;
    String BH1750_I2c;
    int BH1750_I2c_Bus;
    bool initialized = false;
    int sensorInterval = 5000;

    /**
     * @brief Initializes the BH1750 light sensor when a valid I2C bus and address are configured.
     *
     * @details If neither I2C bus is started or the configured I2C address is not "0x23" or "0x5C",
     * the function returns without action. When a valid bus/address is present, the sensor is
     * initialized on the selected Wire interface, its timing is calibrated, and a first high-quality
     * measurement is started. On successful initialization this function sets the module state:
     * `initialized = true` and updates `ms_BH1750` to the current time. On initialization failure an
     * error is logged.
     */
    void Setup()
    {
        if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
        if (BH1750_I2c != "0x23" && BH1750_I2c != "0x5C") return;

        int rc;
        unsigned long m; // milli for calibration
        bool state = false;

        if (BH1750_I2c == "0x23" && BH1750_I2c_Bus == 1)
        {
            state = BH1750.begin(BH1750_TO_GROUND, &Wire);
        }
        else if (BH1750_I2c == "0x5C" && BH1750_I2c_Bus == 1)
        {
            state = BH1750.begin(BH1750_TO_VCC, &Wire);
        }
        else if (BH1750_I2c == "0x23" && BH1750_I2c_Bus == 2)
        {
            state = BH1750.begin(BH1750_TO_GROUND, &Wire1);
        }
        else if (BH1750_I2c == "0x5C" && BH1750_I2c_Bus == 2)
        {
            state = BH1750.begin(BH1750_TO_VCC, &Wire1);
        }

        if (!state)
        {
            Log.println("Error on initialisation BH1750 GY-302");
        }
        else
        {
            Log.println("initialisation BH1750 GY-302 success");
            m = millis();
            rc = BH1750.calibrateTiming();
            m = millis() - m;
            Log.print("Lux Sensor BH1750 GY-302 calibrated (Time: ");
            Log.print(m);
            Log.print(" ms)");
            if (rc != 0)
            {
                Log.print(" - Lux Sensor Error code ");
                Log.print(rc);
            }
            Log.println();

            // start first measure and timecount
            BH1750.start(BH1750_QUALITY_HIGH, 1);
            ms_BH1750 = millis();
            initialized = true;
        }
    }

    void ConnectToWifi()
    {
        BH1750_I2c_Bus = HeadlessWiFiSettings.integer("BH1750_I2c_Bus", 1, 2, DEFAULT_I2C_BUS, "I2C Bus");
        BH1750_I2c = HeadlessWiFiSettings.string("BH1750_I2c", "", "I2C address (0x23 or 0x5C)");
    }

    /**
     * @brief Logs the configured BH1750 sensor I2C address and bus.
     *
     * Writes a single line to the system logger describing the BH1750 I2C address
     * and which I2C bus it is using when an I2C bus is active and an address is configured.
     */
    void SerialReport()
    {
        if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
        if (BH1750_I2c.isEmpty()) return;
        Log.print("BH1750:       ");
        Log.println(BH1750_I2c + " on bus " + BH1750_I2c_Bus);
    }

    /**
     * @brief Periodically reads the BH1750 light sensor and publishes a retained illuminance value.
     *
     * If the configured I2C bus is active, the sensor is initialized, and the measurement interval has elapsed,
     * this function reads an available measurement. When a valid, non-saturated reading exists, it publishes the
     * luminance (in lux) to the roomsTopic "/bh1750_lux". After handling the reading, it adjusts sensor settings,
     * restarts a new measurement cycle, and updates the last-measurement timestamp.
     */
    void Loop()
    {
        if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
        if (!initialized || millis() - ms_BH1750 < sensorInterval) return;

        if (BH1750_I2c == "0x23" || BH1750_I2c == "0x5C")
        {
            if (BH1750.hasValue())
            {
                if (!BH1750.saturated())
                {
                    float lux = BH1750.getLux();
                    pub((roomsTopic + "/bh1750_lux").c_str(), 0, 1, String(int(lux)).c_str());
                }

                BH1750.adjustSettings(90);
                BH1750.start();
                ms_BH1750 = millis();
            }
        }
    }

    bool SendDiscovery()
    {
        if (BH1750_I2c.isEmpty()) return true;

        return sendSensorDiscovery("BH1750 Lux", EC_NONE, "illuminance", "lx");
    }
}

#endif