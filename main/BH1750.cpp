#ifdef SENSORS
#include "BH1750.h"

#include "I2C.h"
#include "Settings.h"
#include "defaults.h"
#include "globals.h"
#include "mqtt.h"

namespace BH1750
{
    unsigned long ms_BH1750;
    std::string BH1750_I2c;
    int BH1750_I2c_Bus;
    bool initialized = false;
    unsigned long sensorInterval = 5000;

    static uint8_t addr = 0;
    static int bus = 1;

    static const uint8_t POWER_ON = 0x01;
    static const uint8_t RESET = 0x07;
    static const uint8_t CONT_HIGH_RES = 0x10;  // 1 lx resolution, ~120 ms per measurement

    static bool cmd(uint8_t c) { return I2C::write(bus, addr, &c, 1); }

    // ponytail: hp_BH1750's timing calibration and MTreg auto-ranging are gone; the sensor runs
    // continuous high-res mode with the default MTreg (69) and a reading of 0xFFFF counts as saturated.
    void Setup()
    {
        if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
        if (BH1750_I2c != "0x23" && BH1750_I2c != "0x5C") return;

        addr = BH1750_I2c == "0x23" ? 0x23 : 0x5C;
        bus = BH1750_I2c_Bus;

        bool state = cmd(POWER_ON) && cmd(RESET) && cmd(CONT_HIGH_RES);

        if (!state)
        {
            Log.println("Error on initialisation BH1750 GY-302");
        }
        else
        {
            Log.println("initialisation BH1750 GY-302 success");
            ms_BH1750 = millis();
            initialized = true;
        }
    }

    void ConnectToWifi(bool updating)
    {
        BH1750_I2c_Bus = Settings::integer("BH1750_I2c_Bus", 1, 2, DEFAULT_I2C_BUS, "I2C Bus");
        BH1750_I2c = Settings::string("BH1750_I2c", "", "I2C address (0x23 or 0x5C)");
    }

    void SerialReport()
    {
        if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
        if (BH1750_I2c.empty()) return;
        Log.print("BH1750:       ");
        Log.println(BH1750_I2c + " on bus " + toStr(BH1750_I2c_Bus));
    }

    void Loop()
    {
        if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
        if (!initialized || millis() - ms_BH1750 < sensorInterval) return;

        uint8_t d[2];
        if (I2C::read(bus, addr, d, 2))
        {
            uint16_t raw = (d[0] << 8) | d[1];
            if (raw != 0xFFFF)
            {
                float lux = raw / 1.2f;
                pub((roomsTopic + "/bh1750_lux").c_str(), 0, 1, toStr(int(lux)).c_str());
            }
        }
        ms_BH1750 = millis();
    }

    bool SendDiscovery()
    {
        if (BH1750_I2c.empty()) return true;

        return sendSensorDiscovery("BH1750 Lux", EC_NONE, "illuminance", "lx");
    }
}

#endif
