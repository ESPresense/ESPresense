#ifdef SENSORS

#include "TSL2561.h"

#include "I2C.h"
#include "Settings.h"
#include "defaults.h"
#include "globals.h"
#include "mqtt.h"

namespace TSL2561
{
    std::string TSL2561_I2c;
    int TSL2561_I2c_Bus;
    std::string TSL2561_I2c_Gain;
    unsigned long tsl2561PreviousMillis = 0;
    unsigned long sensorInterval = 60000;

    static uint8_t addr = 0;
    static int bus = 1;
    static bool gain16 = false;
    static bool powered = false;

    static const uint8_t CMD = 0x80, WORD = 0x20;
    static const uint8_t REG_CONTROL = 0x00, REG_TIMING = 0x01, REG_DATA0 = 0x0C, REG_DATA1 = 0x0E;
    static const uint8_t INTEG_402MS = 0x02;
    static const uint8_t GAIN_16X = 0x10;
    // Adafruit constants for the 402 ms integration window (T/FN/CL package)
    static const uint16_t CLIP_402MS = 65000, AGC_THI_402MS = 63000, AGC_TLO_402MS = 500;

    static bool setGain(bool g16)
    {
        gain16 = g16;
        return I2C::writeReg(bus, addr, CMD | REG_TIMING, INTEG_402MS | (g16 ? GAIN_16X : 0));
    }

    static bool readWord(uint8_t reg, uint16_t& v)
    {
        uint8_t d[2];
        if (!I2C::readReg(bus, addr, CMD | WORD | reg, d, 2)) return false;
        v = d[0] | (d[1] << 8);
        return true;
    }

    // Datasheet / Adafruit calculateLux for the T package at 402 ms; 0 means clipped.
    static uint32_t calculateLux(uint16_t b0, uint16_t b1)
    {
        if (b0 > CLIP_402MS || b1 > CLIP_402MS) return 0;
        uint32_t chScale = 1 << 10;  // 402 ms: no integration scaling
        if (!gain16) chScale <<= 4;  // scale 1x readings up to 16x
        uint32_t ch0 = (b0 * chScale) >> 10;
        uint32_t ch1 = (b1 * chScale) >> 10;
        uint32_t ratio1 = ch0 ? (ch1 << 10) / ch0 : 0;
        uint32_t ratio = (ratio1 + 1) >> 1;
        uint32_t b, m;
        if (ratio <= 0x0040) { b = 0x01F2; m = 0x01BE; }
        else if (ratio <= 0x0080) { b = 0x0214; m = 0x02D1; }
        else if (ratio <= 0x00C0) { b = 0x023F; m = 0x037B; }
        else if (ratio <= 0x0100) { b = 0x0270; m = 0x03FE; }
        else if (ratio <= 0x0138) { b = 0x016F; m = 0x01FC; }
        else if (ratio <= 0x019A) { b = 0x00D2; m = 0x00FB; }
        else if (ratio <= 0x029A) { b = 0x0018; m = 0x0012; }
        else { b = 0; m = 0; }
        int64_t temp = (int64_t)ch0 * b - (int64_t)ch1 * m;
        if (temp < 0) temp = 0;
        temp += 1 << 13;  // round
        return (uint32_t)(temp >> 14);
    }

    // ponytail: the Adafruit path re-created the driver and blocked ~400 ms on every loop; now the
    // sensor is powered once in Setup and only sampled every sensorInterval.
    void Setup()
    {
        if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;

        if (TSL2561_I2c == "0x39") addr = 0x39;
        else if (TSL2561_I2c == "0x29") addr = 0x29;
        else if (TSL2561_I2c == "0x49") addr = 0x49;
        else return;
        bus = TSL2561_I2c_Bus;

        if (TSL2561_I2c_Gain != "auto" && TSL2561_I2c_Gain != "1x" && TSL2561_I2c_Gain != "16x") {
            Log.println("[TSL2561] Invalid gain");
            return;
        }

        powered = I2C::writeReg(bus, addr, CMD | REG_CONTROL, 0x03) && setGain(TSL2561_I2c_Gain == "16x");
        if (!powered) Log.println("[TSL2561] Couldn't find a sensor, check your wiring and I2C address!");
    }

    void ConnectToWifi(bool updating)
    {
        TSL2561_I2c_Bus = Settings::integer("TSL2561_I2c_Bus", 1, 2, DEFAULT_I2C_BUS, "I2C Bus");
        TSL2561_I2c = Settings::string("TSL2561_I2c", "", "I2C address (0x39, 0x49 or 0x29)");
        TSL2561_I2c_Gain = Settings::string("TSL2561_I2c_Gain", DEFAULT_TSL2561_I2C_GAIN, "Gain (auto, 1x or 16x)");
    }

    void SerialReport()
    {
        if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
        if (TSL2561_I2c.empty()) return;
        Log.print("TSL2561:      ");
        Log.println(TSL2561_I2c + " on bus " + toStr(TSL2561_I2c_Bus));
    }

    static bool readChannels(uint16_t& ch0, uint16_t& ch1)
    {
        return readWord(REG_DATA0, ch0) && readWord(REG_DATA1, ch1);
    }

    // Adafruit getLuminosity auto-range: switch gain when ch0 leaves the window and re-sample.
    static bool readLuminosity(uint16_t& ch0, uint16_t& ch1)
    {
        if (!readChannels(ch0, ch1)) return false;
        if (TSL2561_I2c_Gain != "auto") return true;
        bool changed = false;
        if (ch0 < AGC_TLO_402MS && !gain16) changed = setGain(true);
        else if (ch0 > AGC_THI_402MS && gain16) changed = setGain(false);
        if (!changed) return true;
        delay(403);  // one full integration cycle at the new gain
        return readChannels(ch0, ch1);
    }

    void Loop()
    {
        if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
        if (!powered) return;
        if (millis() - tsl2561PreviousMillis < sensorInterval) return;

        uint16_t ch0, ch1;
        if (!readLuminosity(ch0, ch1)) return;
        uint32_t lux = calculateLux(ch0, ch1);

        if (lux) {
            pub((roomsTopic + "/tsl2561_lux").c_str(), 0, 1, toStr((float)lux).c_str());
            tsl2561PreviousMillis = millis();
        } else {
            Log.println("[TSL2561] Sensor overloaded");
        }
    }

    bool SendDiscovery()
    {
        if (TSL2561_I2c.empty()) return true;

        return sendSensorDiscovery("TSL2561 Lux", EC_NONE, "illuminance", "lx");
    }
}

#endif
