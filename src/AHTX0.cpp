#ifdef SENSORS
#include "AHTX0.h"

#include "globals.h"
#include "mqtt.h"
#include "defaults.h"
#include <AsyncWiFiSettings.h>
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
            Serial.println("[AHTX0] Couldn't find a sensor, check your wiring and I2C address!");
        } else {
            initialized = true;
        }
    }

    void ConnectToWifi()
    {
        AsyncWiFiSettings.html("h4", "AHTX0 - Temperature + Humidity Sensor:");
        AHTX0_I2c_Bus = AsyncWiFiSettings.integer("AHTX0_I2c_Bus", 1, 2, DEFAULT_I2C_BUS, "I2C Bus");
        AHTX0_I2c = AsyncWiFiSettings.string("AHTX0_I2c", "", "I2C address (0x38 or 0x39)");
    }

    void SerialReport()
    {
        if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
        if (AHTX0_I2c.isEmpty()) return;
        Serial.print("AHTX0_I2c Sensor: ");
        Serial.println(AHTX0_I2c + " on bus " + AHTX0_I2c_Bus);
    }

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
