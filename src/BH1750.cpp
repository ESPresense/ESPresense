#ifdef SENSORS
#include "BH1750.h"

#include "globals.h"
#include "mqtt.h"
#include "defaults.h"
#include <WiFiSettings.h>
#include <AsyncMqttClient.h>
#include "string_utils.h"

#include <hp_BH1750.h>

namespace BH1750
{
    hp_BH1750 BH1750;
    unsigned long ms_BH1750;
    float lux_BH1750;
    int lux_BH1750_MQTT;
    String BH1750_I2c;
    int BH1750_I2c_Bus;
    bool initialized = false;

    void Setup()
    {
        if (!I2C_Bus_1_Enabled && !I2C_Bus_2_Enabled) return;
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
            lux_BH1750 = -1; // nothing to compare
            BH1750.start(BH1750_QUALITY_HIGH, 1);
            ms_BH1750 = millis();
            initialized = true;
        }
    }

    void ConnectToWifi()
    {
        WiFiSettings.html("h4", "BH1750 - Ambient Light Sensor:");
        BH1750_I2c_Bus = WiFiSettings.integer("BH1750_I2c_Bus", 1, 2, DEFAULT_I2C_BUS, "I2C Bus");
        BH1750_I2c = WiFiSettings.string("BH1750_I2c", "", "I2C address (0x23 or 0x5C)");
    }

    void SerialReport()
    {
        Serial.print("BH1750_I2c Sensor: ");
        Serial.println(BH1750_I2c + " on bus " + BH1750_I2c_Bus);
    }

    void Loop()
    {
        if (!I2C_Bus_1_Enabled && !I2C_Bus_2_Enabled) return;
        if (!initialized) return;

        if (BH1750_I2c == "0x23" || BH1750_I2c == "0x5C")
        {
            float lux;
            int lux_mqtt;

            if (BH1750.hasValue())
            {
                ms_BH1750 = millis() - ms_BH1750;
                if (!BH1750.saturated())
                {
                    lux = BH1750.getLux();
                    lux_mqtt = int(lux);

                    if (lux != lux_BH1750)
                    {
                        lux_BH1750 = lux;
                        // Serial.print("BH1750 (");
                        // Serial.print(ms_BH1750);
                        // Serial.print(" ms): ");
                        // Serial.print(lux);
                        // Serial.println(" lx");
                    }

                    //convert lx to integer to reduce mqtt traffic, send only if lx changed
                    if (lux_mqtt != lux_BH1750_MQTT)
                    {
                        lux_BH1750_MQTT = lux_mqtt;
                        Serial.print("BH1750 (");
                        Serial.print(ms_BH1750);
                        Serial.print(" ms): ");
                        Serial.print(lux_mqtt);
                        Serial.println(" lx");
                        mqttClient.publish((roomsTopic + "/lux").c_str(), 0, 1, String(lux_mqtt).c_str());
                    }
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

        return sendSensorDiscovery("Lux", "", "illuminance", "lx");
    }
}

#endif
