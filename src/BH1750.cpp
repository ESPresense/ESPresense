#ifdef SENSORS
#include "BH1750.h"

#include "globals.h"
#include "mqtt.h"
#include "defaults.h"
#include <AsyncWiFiSettings.h>
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
            Serial.println("Error on initialisation BH1750 GY-302");
        }
        else
        {
            Serial.println("initialisation BH1750 GY-302 success");
            m = millis();
            rc = BH1750.calibrateTiming();
            m = millis() - m;
            Serial.print("Lux Sensor BH1750 GY-302 calibrated (Time: ");
            Serial.print(m);
            Serial.print(" ms)");
            if (rc != 0)
            {
                Serial.print(" - Lux Sensor Error code ");
                Serial.print(rc);
            }
            Serial.println();

            // start first measure and timecount
            BH1750.start(BH1750_QUALITY_HIGH, 1);
            ms_BH1750 = millis();
            initialized = true;
        }
    }

    void ConnectToWifi()
    {
        BH1750_I2c_Bus = AsyncWiFiSettings.integer("BH1750_I2c_Bus", 1, 2, DEFAULT_I2C_BUS, "I2C Bus");
        BH1750_I2c = AsyncWiFiSettings.string("BH1750_I2c", "", "I2C address (0x23 or 0x5C)");
    }

    void SerialReport()
    {
        if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
        if (BH1750_I2c.isEmpty()) return;
        Serial.print("BH1750:       ");
        Serial.println(BH1750_I2c + " on bus " + BH1750_I2c_Bus);
    }

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
