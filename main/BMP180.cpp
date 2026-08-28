#ifdef SENSORS
#include "BMP180.h"

#include "I2C.h"
#include "Settings.h"
#include "defaults.h"
#include "globals.h"
#include "mqtt.h"

namespace BMP180
{
    long BMP180_status;
    std::string BMP180_I2c;
    int BMP180_I2c_Bus;
    unsigned long BMP180PreviousMillis = 0;
    unsigned long sensorInterval = 60000;
    bool initialized = false;

    static const uint8_t ADDR = 0x77;
    static const int OSS = 1;  // BMP085_STANDARD
    static int bus = 1;

    // Datasheet 3.4 calibration coefficients (EEPROM 0xAA..0xBF, big endian)
    static int16_t ac1, ac2, ac3, b1, b2, mb, mc, md;
    static uint16_t ac4, ac5, ac6;

    static bool readCalibration()
    {
        uint8_t c[22];
        if (!I2C::readReg(bus, ADDR, 0xAA, c, sizeof(c))) return false;
        auto u16 = [&](int i) { return (uint16_t)((c[i] << 8) | c[i + 1]); };
        auto s16 = [&](int i) { return (int16_t)u16(i); };
        ac1 = s16(0); ac2 = s16(2); ac3 = s16(4);
        ac4 = u16(6); ac5 = u16(8); ac6 = u16(10);
        b1 = s16(12); b2 = s16(14); mb = s16(16); mc = s16(18); md = s16(20);
        return true;
    }

    static bool readRawTemperature(int32_t& ut)
    {
        if (!I2C::writeReg(bus, ADDR, 0xF4, 0x2E)) return false;
        delay(5);
        uint8_t d[2];
        if (!I2C::readReg(bus, ADDR, 0xF6, d, 2)) return false;
        ut = (d[0] << 8) | d[1];
        return true;
    }

    static bool readRawPressure(int32_t& up)
    {
        if (!I2C::writeReg(bus, ADDR, 0xF4, 0x34 + (OSS << 6))) return false;
        delay(8);  // oss=1 conversion time 7.5 ms
        uint8_t d[3];
        if (!I2C::readReg(bus, ADDR, 0xF6, d, 3)) return false;
        up = (((int32_t)d[0] << 16) | ((int32_t)d[1] << 8) | d[2]) >> (8 - OSS);
        return true;
    }

    static int32_t computeB5(int32_t ut)
    {
        int32_t x1 = ((ut - (int32_t)ac6) * (int32_t)ac5) >> 15;
        int32_t x2 = ((int32_t)mc << 11) / (x1 + (int32_t)md);
        return x1 + x2;
    }

    static bool readSensor(float& temperature, float& pressurePa)
    {
        int32_t ut, up;
        if (!readRawTemperature(ut) || !readRawPressure(up)) return false;

        int32_t b5 = computeB5(ut);
        temperature = (float)((b5 + 8) >> 4) / 10.0f;

        int32_t b6 = b5 - 4000;
        int32_t x1 = ((int32_t)b2 * ((b6 * b6) >> 12)) >> 11;
        int32_t x2 = ((int32_t)ac2 * b6) >> 11;
        int32_t x3 = x1 + x2;
        int32_t b3 = ((((int32_t)ac1 * 4 + x3) << OSS) + 2) / 4;

        x1 = ((int32_t)ac3 * b6) >> 13;
        x2 = ((int32_t)b1 * ((b6 * b6) >> 12)) >> 16;
        x3 = ((x1 + x2) + 2) >> 2;
        uint32_t b4 = ((uint32_t)ac4 * (uint32_t)(x3 + 32768)) >> 15;
        uint32_t b7 = ((uint32_t)up - b3) * (uint32_t)(50000UL >> OSS);

        int32_t p = b7 < 0x80000000 ? (b7 * 2) / b4 : (b7 / b4) * 2;
        x1 = (p >> 8) * (p >> 8);
        x1 = (x1 * 3038) >> 16;
        x2 = (-7357 * p) >> 16;
        p = p + ((x1 + x2 + 3791) >> 4);
        pressurePa = (float)p;
        return true;
    }

    void Setup()
    {
        if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;

        if (BMP180_I2c != "0x77") return;
        bus = BMP180_I2c_Bus;

        uint8_t id = 0;
        BMP180_status = I2C::readReg8(bus, ADDR, 0xD0, id) && id == 0x55 && readCalibration();

        if (!BMP180_status) {
            Log.println("[BMP180] Couldn't find a sensor, check your wiring and I2C address!");
        } else {
            initialized = true;
        }
    }

    void ConnectToWifi(bool updating)
    {
        BMP180_I2c_Bus = Settings::integer("BMP180_I2c_Bus", 1, 2, DEFAULT_I2C_BUS, "I2C Bus");
        BMP180_I2c = Settings::string("BMP180_I2c", "", "I2C address (0x77)");
    }

    void SerialReport()
    {
        if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
        if (BMP180_I2c.empty()) return;
        Log.print("BMP180:       ");
        Log.println(BMP180_I2c + " on bus " + toStr(BMP180_I2c_Bus));
    }

    void Loop()
    {
        if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
        if (!initialized) return;

        if (BMP180PreviousMillis == 0 || millis() - BMP180PreviousMillis >= sensorInterval) {
            float temperature, pressurePa;
            if (readSensor(temperature, pressurePa)) {
                float pressure = pressurePa / 100.0F;
                pub((roomsTopic + "/bmp180_temperature").c_str(), 0, 1, toStr(temperature).c_str());
                pub((roomsTopic + "/bmp180_pressure").c_str(), 0, 1, toStr(pressure).c_str());
            }

            BMP180PreviousMillis = millis();
        }
    }

    bool SendDiscovery()
    {
        if (BMP180_I2c.empty()) return true;

        return sendSensorDiscovery("BMP180 Temperature", EC_NONE, "temperature", "°C")
            && sendSensorDiscovery("BMP180 Pressure", EC_NONE, "pressure", "hPa");
    }
}

#endif
