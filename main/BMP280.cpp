#ifdef SENSORS
#include "BMP280.h"

#include "I2C.h"
#include "Settings.h"
#include "defaults.h"
#include "globals.h"
#include "mqtt.h"

namespace BMP280
{
    long BMP280_status;
    std::string BMP280_I2c;
    int BMP280_I2c_Bus;
    unsigned long BMP280PreviousMillis = 0;
    unsigned long sensorInterval = 60000;
    bool initialized = false;

    static uint8_t addr = 0;
    static int bus = 1;

    // Datasheet 3.11.2 trimming parameters
    static uint16_t dig_T1, dig_P1;
    static int16_t dig_T2, dig_T3, dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9;
    static int32_t t_fine;

    static bool readCalibration()
    {
        uint8_t c[24];
        if (!I2C::readReg(bus, addr, 0x88, c, sizeof(c))) return false;
        auto u16 = [&](int i) { return (uint16_t)(c[i] | (c[i + 1] << 8)); };
        auto s16 = [&](int i) { return (int16_t)u16(i); };
        dig_T1 = u16(0);  dig_T2 = s16(2);  dig_T3 = s16(4);
        dig_P1 = u16(6);  dig_P2 = s16(8);  dig_P3 = s16(10); dig_P4 = s16(12); dig_P5 = s16(14);
        dig_P6 = s16(16); dig_P7 = s16(18); dig_P8 = s16(20); dig_P9 = s16(22);
        return true;
    }

    static float compensateT(int32_t adc_T)
    {
        int32_t var1 = ((((adc_T >> 3) - ((int32_t)dig_T1 << 1))) * ((int32_t)dig_T2)) >> 11;
        int32_t var2 = (((((adc_T >> 4) - ((int32_t)dig_T1)) * ((adc_T >> 4) - ((int32_t)dig_T1))) >> 12) * ((int32_t)dig_T3)) >> 14;
        t_fine = var1 + var2;
        return (float)((t_fine * 5 + 128) >> 8) / 100.0f;
    }

    static float compensateP(int32_t adc_P)  // Pa
    {
        int64_t var1 = ((int64_t)t_fine) - 128000;
        int64_t var2 = var1 * var1 * (int64_t)dig_P6;
        var2 = var2 + ((var1 * (int64_t)dig_P5) << 17);
        var2 = var2 + (((int64_t)dig_P4) << 35);
        var1 = ((var1 * var1 * (int64_t)dig_P3) >> 8) + ((var1 * (int64_t)dig_P2) << 12);
        var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)dig_P1) >> 33;
        if (var1 == 0) return 0;
        int64_t p = 1048576 - adc_P;
        p = (((p << 31) - var2) * 3125) / var1;
        var1 = (((int64_t)dig_P9) * (p >> 13) * (p >> 13)) >> 25;
        var2 = (((int64_t)dig_P8) * p) >> 19;
        p = ((p + var1 + var2) >> 8) + (((int64_t)dig_P7) << 4);
        return (float)p / 256.0f;
    }

    void Setup()
    {
        if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;

        if (BMP280_I2c == "0x76") addr = 0x76;
        else if (BMP280_I2c == "0x77") addr = 0x77;
        else return;
        bus = BMP280_I2c_Bus;

        uint8_t id = 0;
        BMP280_status = I2C::readReg8(bus, addr, 0xD0, id) && id == 0x58;
        if (BMP280_status) {
            I2C::writeReg(bus, addr, 0xE0, 0xB6);  // soft reset
            delay(10);
            BMP280_status = readCalibration();
        }

        if (!BMP280_status) {
            Log.println("[BMP280] Couldn't find a sensor, check your wiring and I2C address!");
            return;
        }
        initialized = true;

        // Forced mode, x1 oversampling for T/P, filter off.
        I2C::writeReg(bus, addr, 0xF5, 0x00);
        I2C::writeReg(bus, addr, 0xF4, (1 << 5) | (1 << 2) | 0x00);
    }

    void ConnectToWifi(bool updating)
    {
        BMP280_I2c_Bus = Settings::integer("BMP280_I2c_Bus", 1, 2, DEFAULT_I2C_BUS, "I2C Bus");
        BMP280_I2c = Settings::string("BMP280_I2c", "", "I2C address (0x76 or 0x77)");
    }

    void SerialReport()
    {
        if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
        if (BMP280_I2c.empty()) return;
        Log.print("BMP280:       ");
        Log.println(BMP280_I2c + " on bus " + toStr(BMP280_I2c_Bus));
    }

    static bool takeForcedMeasurement()
    {
        if (!I2C::writeReg(bus, addr, 0xF4, (1 << 5) | (1 << 2) | 0x01)) return false;
        uint8_t status = 0x08;
        for (int i = 0; i < 50; i++) {
            if (!I2C::readReg8(bus, addr, 0xF3, status)) return false;
            if (!(status & 0x08)) return true;
            delay(2);
        }
        return false;
    }

    void Loop()
    {
        if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
        if (!initialized) return;

        if (BMP280PreviousMillis == 0 || millis() - BMP280PreviousMillis >= sensorInterval) {
            uint8_t d[6];
            if (takeForcedMeasurement() && I2C::readReg(bus, addr, 0xF7, d, sizeof(d))) {
                int32_t adc_P = ((int32_t)d[0] << 12) | ((int32_t)d[1] << 4) | (d[2] >> 4);
                int32_t adc_T = ((int32_t)d[3] << 12) | ((int32_t)d[4] << 4) | (d[5] >> 4);

                float temperature = compensateT(adc_T);
                float pressure = compensateP(adc_P) / 100.0F;

                pub((roomsTopic + "/bmp280_temperature").c_str(), 0, 1, toStr(temperature).c_str());
                pub((roomsTopic + "/bmp280_pressure").c_str(), 0, 1, toStr(pressure).c_str());
            }

            BMP280PreviousMillis = millis();
        }
    }

    bool SendDiscovery()
    {
        if (BMP280_I2c.empty()) return true;

        return sendSensorDiscovery("BMP280 Temperature", EC_NONE, "temperature", "°C")
            && sendSensorDiscovery("BMP280 Pressure", EC_NONE, "pressure", "hPa");
    }
}

#endif
