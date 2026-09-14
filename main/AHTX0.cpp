#ifdef SENSORS
#include "AHTX0.h"

#include "I2C.h"
#include "Settings.h"
#include "defaults.h"
#include "globals.h"
#include "mqtt.h"

namespace AHTX0
{
    std::string AHTX0_I2c;
    int AHTX0_I2c_Bus;
    unsigned long AHTX0PreviousMillis = 0;
    unsigned long sensorInterval = 60000;
    bool initialized = false;

    static uint8_t addr = 0;
    static int bus = 1;

    static const uint8_t CMD_CALIBRATE[3] = {0xE1, 0x08, 0x00};
    static const uint8_t CMD_TRIGGER[3] = {0xAC, 0x33, 0x00};
    static const uint8_t CMD_SOFTRESET = 0xBA;
    static const uint8_t STATUS_BUSY = 0x80;
    static const uint8_t STATUS_CALIBRATED = 0x08;

    static bool status(uint8_t& s) { return I2C::read(bus, addr, &s, 1); }

    // Wait for the busy bit to clear; false on I2C failure or timeout.
    static bool waitIdle()
    {
        for (int i = 0; i < 100; i++) {
            uint8_t s;
            if (!status(s)) return false;
            if (!(s & STATUS_BUSY)) return true;
            delay(10);
        }
        return false;
    }

    void Setup()
    {
        if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;

        if (AHTX0_I2c == "0x38") addr = 0x38;
        else if (AHTX0_I2c == "0x39") addr = 0x39;
        else return;
        bus = AHTX0_I2c_Bus;

        bool ok = false;
        delay(20);  // power-on settle
        if (I2C::write(bus, addr, &CMD_SOFTRESET, 1)) {
            delay(20);
            uint8_t s = 0;
            ok = waitIdle() && I2C::write(bus, addr, CMD_CALIBRATE, 3) && waitIdle() && status(s) && (s & STATUS_CALIBRATED);
        }

        if (!ok) {
            Log.println("[AHTX0] Couldn't find a sensor, check your wiring and I2C address!");
        } else {
            initialized = true;
        }
    }

    void ConnectToWifi(bool updating)
    {
        AHTX0_I2c_Bus = Settings::integer("AHTX0_I2c_Bus", 1, 2, DEFAULT_I2C_BUS, "I2C Bus");
        AHTX0_I2c = Settings::string("AHTX0_I2c", "", "I2C address (0x38 or 0x39)");
    }

    void SerialReport()
    {
        if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
        if (AHTX0_I2c.empty()) return;
        Log.print("AHTX0_I2c Sensor: ");
        Log.println(AHTX0_I2c + " on bus " + toStr(AHTX0_I2c_Bus));
    }

    static bool readSensor(float& temperature, float& humidity)
    {
        if (!I2C::write(bus, addr, CMD_TRIGGER, 3)) return false;
        delay(80);
        if (!waitIdle()) return false;
        uint8_t d[6];
        if (!I2C::read(bus, addr, d, 6)) return false;
        // 20-bit humidity then 20-bit temperature packed after the status byte
        uint32_t h = ((uint32_t)d[1] << 12) | ((uint32_t)d[2] << 4) | (d[3] >> 4);
        uint32_t t = ((uint32_t)(d[3] & 0x0F) << 16) | ((uint32_t)d[4] << 8) | d[5];
        humidity = (float)h * 100.0f / 1048576.0f;
        temperature = (float)t * 200.0f / 1048576.0f - 50.0f;
        return true;
    }

    void Loop()
    {
        if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
        if (!initialized) return;

        if (AHTX0PreviousMillis == 0 || millis() - AHTX0PreviousMillis >= sensorInterval) {
            float temperature, humidity;
            if (readSensor(temperature, humidity)) {
                pub((roomsTopic + "/ahtx0_temperature").c_str(), 0, 1, toStr(temperature).c_str());
                pub((roomsTopic + "/ahtx0_humidity").c_str(), 0, 1, toStr(humidity).c_str());
            }

            AHTX0PreviousMillis = millis();
        }
    }

    bool SendDiscovery()
    {
        if (AHTX0_I2c.empty()) return true;

        return sendSensorDiscovery("AHTX0 Temperature", EC_NONE, "temperature", "°C")
            && sendSensorDiscovery("AHTX0 Humidity", EC_NONE, "humidity", "%");
    }
}

#endif
