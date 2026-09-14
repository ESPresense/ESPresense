#ifdef SENSORS
#include "BME280.h"

#include "I2C.h"
#include "Settings.h"
#include "defaults.h"
#include "globals.h"
#include "mqtt.h"

namespace BME280
{
    std::string BME280_I2c;
    int BME280_I2c_Bus;
    unsigned long bme280PreviousMillis = 0;
    unsigned long sensorInterval = 60000;
    bool initialized = false;

    static uint8_t addr = 0;
    static int bus = 1;

    // Datasheet 4.2.2 trimming parameters
    static uint16_t dig_T1, dig_P1;
    static int16_t dig_T2, dig_T3, dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9;
    static uint8_t dig_H1, dig_H3;
    static int16_t dig_H2, dig_H4, dig_H5;
    static int8_t dig_H6;
    static int32_t t_fine;

    static bool readCalibration()
    {
        uint8_t c[26], h[7];
        if (!I2C::readReg(bus, addr, 0x88, c, sizeof(c))) return false;
        if (!I2C::readReg(bus, addr, 0xE1, h, sizeof(h))) return false;
        auto u16 = [&](int i) { return (uint16_t)(c[i] | (c[i + 1] << 8)); };
        auto s16 = [&](int i) { return (int16_t)u16(i); };
        dig_T1 = u16(0);  dig_T2 = s16(2);  dig_T3 = s16(4);
        dig_P1 = u16(6);  dig_P2 = s16(8);  dig_P3 = s16(10); dig_P4 = s16(12); dig_P5 = s16(14);
        dig_P6 = s16(16); dig_P7 = s16(18); dig_P8 = s16(20); dig_P9 = s16(22);
        dig_H1 = c[25];
        dig_H2 = (int16_t)(h[0] | (h[1] << 8));
        dig_H3 = h[2];
        dig_H4 = (int16_t)((h[3] << 4) | (h[4] & 0x0F));
        dig_H5 = (int16_t)((h[5] << 4) | (h[4] >> 4));
        dig_H6 = (int8_t)h[6];
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

    static float compensateH(int32_t adc_H)
    {
        int32_t v = t_fine - ((int32_t)76800);
        v = (((((adc_H << 14) - (((int32_t)dig_H4) << 20) - (((int32_t)dig_H5) * v)) + ((int32_t)16384)) >> 15) *
             (((((((v * ((int32_t)dig_H6)) >> 10) * (((v * ((int32_t)dig_H3)) >> 11) + ((int32_t)32768))) >> 10) + ((int32_t)2097152)) * ((int32_t)dig_H2) + 8192) >> 14));
        v = v - (((((v >> 15) * (v >> 15)) >> 7) * ((int32_t)dig_H1)) >> 4);
        if (v < 0) v = 0;
        if (v > 419430400) v = 419430400;
        return (float)(v >> 12) / 1024.0f;
    }

    void Setup()
    {
        if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;

        if (BME280_I2c == "0x76") addr = 0x76;
        else if (BME280_I2c == "0x77") addr = 0x77;
        else return;
        bus = BME280_I2c_Bus;

        uint8_t id = 0;
        bool ok = I2C::readReg8(bus, addr, 0xD0, id) && id == 0x60;
        if (ok) {
            I2C::writeReg(bus, addr, 0xE0, 0xB6);  // soft reset
            delay(10);
            uint8_t status = 1;
            for (int i = 0; i < 20 && I2C::readReg8(bus, addr, 0xF3, status) && (status & 0x01); i++) delay(10);  // NVM copy
            ok = readCalibration();
        }

        if (!ok) {
            Log.println("[BME280] Couldn't find a sensor, check your wiring and I2C address!");
            return;
        }

        // Forced mode, x1 oversampling for T/P/H, filter off, standby irrelevant.
        I2C::writeReg(bus, addr, 0xF2, 0x01);  // ctrl_hum osrs_h=1
        I2C::writeReg(bus, addr, 0xF5, 0x00);  // config: filter off
        I2C::writeReg(bus, addr, 0xF4, (1 << 5) | (1 << 2) | 0x00);  // ctrl_meas: osrs_t=1 osrs_p=1 sleep

        initialized = true;
    }

    void ConnectToWifi(bool updating)
    {
        BME280_I2c_Bus = Settings::integer("BME280_I2c_Bus", 1, 2, DEFAULT_I2C_BUS, "I2C Bus");
        BME280_I2c = Settings::string("BME280_I2c", "", "I2C address (0x76 or 0x77)");
    }

    void SerialReport()
    {
        if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
        if (BME280_I2c.empty()) return;
        Log.print("BME280:       ");
        Log.println(BME280_I2c + " on bus " + toStr(BME280_I2c_Bus));
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

        if (millis() - bme280PreviousMillis >= sensorInterval) {
            uint8_t d[8];
            if (takeForcedMeasurement() && I2C::readReg(bus, addr, 0xF7, d, sizeof(d))) {
                int32_t adc_P = ((int32_t)d[0] << 12) | ((int32_t)d[1] << 4) | (d[2] >> 4);
                int32_t adc_T = ((int32_t)d[3] << 12) | ((int32_t)d[4] << 4) | (d[5] >> 4);
                int32_t adc_H = ((int32_t)d[6] << 8) | d[7];

                float temperature = compensateT(adc_T);
                float humidity = compensateH(adc_H);
                float pressure = compensateP(adc_P) / 100.0F;

                pub((roomsTopic + "/bme280_temperature").c_str(), 0, 1, toStr(temperature).c_str());
                pub((roomsTopic + "/bme280_humidity").c_str(), 0, 1, toStr(humidity).c_str());
                pub((roomsTopic + "/bme280_pressure").c_str(), 0, 1, toStr(pressure).c_str());
            }

            bme280PreviousMillis = millis();
        }
    }

    bool SendDiscovery()
    {
        if (BME280_I2c.empty()) return true;

        return sendSensorDiscovery("BME280 Temperature", EC_NONE, "temperature", "°C")
            && sendSensorDiscovery("BME280 Humidity", EC_NONE, "humidity", "%")
            && sendSensorDiscovery("BME280 Pressure", EC_NONE, "pressure", "hPa");
    }
}

#endif
