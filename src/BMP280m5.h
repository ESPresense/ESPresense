#pragma once
#ifdef SENSORS

// Vendored and adapted from https://github.com/m5stack/M5Unit-ENV/blob/master/src/BMP280.h
// Modified: renamed to BMP280m5 to avoid conflicts with existing Adafruit BMP280 driver

#include "Arduino.h"
#include "Wire.h"
#include "I2C_Class.h"

#define BMP280M5_I2C_ADDR 0x76

enum {
    BMP280_REGISTER_DIG_T1       = 0x88,
    BMP280_REGISTER_DIG_T2       = 0x8A,
    BMP280_REGISTER_DIG_T3       = 0x8C,
    BMP280_REGISTER_DIG_P1       = 0x8E,
    BMP280_REGISTER_DIG_P2       = 0x90,
    BMP280_REGISTER_DIG_P3       = 0x92,
    BMP280_REGISTER_DIG_P4       = 0x94,
    BMP280_REGISTER_DIG_P5       = 0x96,
    BMP280_REGISTER_DIG_P6       = 0x98,
    BMP280_REGISTER_DIG_P7       = 0x9A,
    BMP280_REGISTER_DIG_P8       = 0x9C,
    BMP280_REGISTER_DIG_P9       = 0x9E,
    BMP280_REGISTER_CHIPID       = 0xD0,
    BMP280_REGISTER_VERSION      = 0xD1,
    BMP280_REGISTER_SOFTRESET    = 0xE0,
    BMP280_REGISTER_CAL26        = 0xE1,
    BMP280_REGISTER_STATUS       = 0xF3,
    BMP280_REGISTER_CONTROL      = 0xF4,
    BMP280_REGISTER_CONFIG       = 0xF5,
    BMP280_REGISTER_PRESSUREDATA = 0xF7,
    BMP280_REGISTER_TEMPDATA     = 0xFA,
};

typedef struct {
    uint16_t dig_T1;
    int16_t  dig_T2;
    int16_t  dig_T3;
    uint16_t dig_P1;
    int16_t  dig_P2;
    int16_t  dig_P3;
    int16_t  dig_P4;
    int16_t  dig_P5;
    int16_t  dig_P6;
    int16_t  dig_P7;
    int16_t  dig_P8;
    int16_t  dig_P9;
} bmp280m5_calib_data;

class BMP280m5 {
   public:
    enum sensor_sampling {
        SAMPLING_NONE = 0x00,
        SAMPLING_X1   = 0x01,
        SAMPLING_X2   = 0x02,
        SAMPLING_X4   = 0x03,
        SAMPLING_X8   = 0x04,
        SAMPLING_X16  = 0x05
    };

    enum sensor_mode {
        MODE_SLEEP          = 0x00,
        MODE_FORCED         = 0x01,
        MODE_NORMAL         = 0x03,
        MODE_SOFT_RESET_CODE = 0xB6
    };

    enum sensor_filter {
        FILTER_OFF = 0x00,
        FILTER_X2  = 0x01,
        FILTER_X4  = 0x02,
        FILTER_X8  = 0x03,
        FILTER_X16 = 0x04
    };

    enum standby_duration {
        STANDBY_MS_1    = 0x00,
        STANDBY_MS_63   = 0x01,
        STANDBY_MS_125  = 0x02,
        STANDBY_MS_250  = 0x03,
        STANDBY_MS_500  = 0x04,
        STANDBY_MS_1000 = 0x05,
        STANDBY_MS_2000 = 0x06,
        STANDBY_MS_4000 = 0x07
    };

    bool begin(TwoWire* wire, uint8_t addr, uint8_t sda, uint8_t scl, long freq = 400000U);
    bool begin(TwoWire* wire, uint8_t addr = BMP280M5_I2C_ADDR);

    float readTemperature();
    float readPressure();
    bool  takeForcedMeasurement();
    void  setSampling(sensor_mode mode              = MODE_NORMAL,
                      sensor_sampling tempSampling  = SAMPLING_X16,
                      sensor_sampling pressSampling = SAMPLING_X16,
                      sensor_filter filter          = FILTER_OFF,
                      standby_duration duration     = STANDBY_MS_1);
    void    reset();
    uint8_t getStatus();

    float pressure = 0;
    float cTemp    = 0;

   private:
    struct config {
        config() : t_sb(STANDBY_MS_1), filter(FILTER_OFF), none(0), spi3w_en(0) {}
        unsigned int t_sb     : 3;
        unsigned int filter   : 3;
        unsigned int none     : 1;
        unsigned int spi3w_en : 1;
        unsigned int get() { return (t_sb << 5) | (filter << 2) | spi3w_en; }
    };

    struct ctrl_meas {
        ctrl_meas() : osrs_t(SAMPLING_NONE), osrs_p(SAMPLING_NONE), mode(MODE_SLEEP) {}
        unsigned int osrs_t : 3;
        unsigned int osrs_p : 3;
        unsigned int mode   : 2;
        unsigned int get() { return (osrs_t << 5) | (osrs_p << 2) | mode; }
    };

    void     readCoefficients();
    void     write8(byte reg, byte value);
    uint8_t  read8(byte reg);
    uint16_t read16(byte reg);
    uint32_t read24(byte reg);
    int16_t  readS16(byte reg);
    uint16_t read16_LE(byte reg);
    int16_t  readS16_LE(byte reg);

    int32_t             t_fine;
    bmp280m5_calib_data _bmp280m5_calib;
    config              _configReg;
    ctrl_meas           _measReg;
    uint8_t             _addr;
    TwoWire*            _wire;
    I2C_Class           _i2c;
};

#endif