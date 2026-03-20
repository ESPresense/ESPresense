#ifdef SENSORS
#include "BMP280m5.h"

// Vendored and adapted from https://github.com/m5stack/M5Unit-ENV/blob/master/src/BMP280.cpp
// Modified: renamed to BMP280m5 to avoid conflicts with existing Adafruit BMP280 driver

bool BMP280m5::begin(TwoWire* wire, uint8_t addr, uint8_t sda, uint8_t scl, long freq) {
    _wire = wire;
    _addr = addr;
    _wire->end();
    _wire->begin(sda, scl, freq);
    wire->beginTransmission(addr);
    if (wire->endTransmission() != 0) {
        return false;
    }
    readCoefficients();
    setSampling();
    return true;
}

bool BMP280m5::begin(TwoWire* wire, uint8_t addr) {
    _wire = wire;
    _addr = addr;
    wire->beginTransmission(addr);
    if (wire->endTransmission() != 0) {
        return false;
    }
    readCoefficients();
    setSampling();
    return true;
}

float BMP280m5::readTemperature() {
    int32_t var1, var2;

    int32_t adc_T = read24(BMP280_REGISTER_TEMPDATA);
    adc_T >>= 4;

    var1 = ((((adc_T >> 3) - ((int32_t)_bmp280m5_calib.dig_T1 << 1))) *
            ((int32_t)_bmp280m5_calib.dig_T2)) >> 11;

    var2 = (((((adc_T >> 4) - ((int32_t)_bmp280m5_calib.dig_T1)) *
              ((adc_T >> 4) - ((int32_t)_bmp280m5_calib.dig_T1))) >> 12) *
            ((int32_t)_bmp280m5_calib.dig_T3)) >> 14;

    t_fine = var1 + var2;

    float T = (t_fine * 5 + 128) >> 8;
    cTemp   = T / 100;
    return cTemp;
}

float BMP280m5::readPressure() {
    int64_t var1, var2, p;

    // Must call readTemperature first to set t_fine
    readTemperature();

    int32_t adc_P = read24(BMP280_REGISTER_PRESSUREDATA);
    adc_P >>= 4;

    var1 = ((int64_t)t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)_bmp280m5_calib.dig_P6;
    var2 = var2 + ((var1 * (int64_t)_bmp280m5_calib.dig_P5) << 17);
    var2 = var2 + (((int64_t)_bmp280m5_calib.dig_P4) << 35);
    var1 = ((var1 * var1 * (int64_t)_bmp280m5_calib.dig_P3) >> 8) +
           ((var1 * (int64_t)_bmp280m5_calib.dig_P2) << 12);
    var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)_bmp280m5_calib.dig_P1) >> 33;

    if (var1 == 0) {
        return 0;
    }

    p    = 1048576 - adc_P;
    p    = (((p << 31) - var2) * 3125) / var1;
    var1 = (((int64_t)_bmp280m5_calib.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t)_bmp280m5_calib.dig_P8) * p) >> 19;

    p        = ((p + var1 + var2) >> 8) + (((int64_t)_bmp280m5_calib.dig_P7) << 4);
    pressure = (float)p / 256;
    return pressure;
}

void BMP280m5::setSampling(sensor_mode mode, sensor_sampling tempSampling,
                           sensor_sampling pressSampling, sensor_filter filter,
                           standby_duration duration) {
    _measReg.mode   = mode;
    _measReg.osrs_t = tempSampling;
    _measReg.osrs_p = pressSampling;
    _configReg.filter = filter;
    _configReg.t_sb   = duration;
    write8(BMP280_REGISTER_CONFIG, _configReg.get());
    write8(BMP280_REGISTER_CONTROL, _measReg.get());
}

void BMP280m5::readCoefficients() {
    _bmp280m5_calib.dig_T1 = read16_LE(BMP280_REGISTER_DIG_T1);
    _bmp280m5_calib.dig_T2 = readS16_LE(BMP280_REGISTER_DIG_T2);
    _bmp280m5_calib.dig_T3 = readS16_LE(BMP280_REGISTER_DIG_T3);
    _bmp280m5_calib.dig_P1 = read16_LE(BMP280_REGISTER_DIG_P1);
    _bmp280m5_calib.dig_P2 = readS16_LE(BMP280_REGISTER_DIG_P2);
    _bmp280m5_calib.dig_P3 = readS16_LE(BMP280_REGISTER_DIG_P3);
    _bmp280m5_calib.dig_P4 = readS16_LE(BMP280_REGISTER_DIG_P4);
    _bmp280m5_calib.dig_P5 = readS16_LE(BMP280_REGISTER_DIG_P5);
    _bmp280m5_calib.dig_P6 = readS16_LE(BMP280_REGISTER_DIG_P6);
    _bmp280m5_calib.dig_P7 = readS16_LE(BMP280_REGISTER_DIG_P7);
    _bmp280m5_calib.dig_P8 = readS16_LE(BMP280_REGISTER_DIG_P8);
    _bmp280m5_calib.dig_P9 = readS16_LE(BMP280_REGISTER_DIG_P9);
}

bool BMP280m5::takeForcedMeasurement() {
    if (_measReg.mode == MODE_FORCED) {
        write8(BMP280_REGISTER_CONTROL, _measReg.get());
        while (read8(BMP280_REGISTER_STATUS) & 0x08) delay(1);
        return true;
    }
    return false;
}

void BMP280m5::reset() {
    write8(BMP280_REGISTER_SOFTRESET, MODE_SOFT_RESET_CODE);
}

uint8_t BMP280m5::getStatus() {
    return read8(BMP280_REGISTER_STATUS);
}

void BMP280m5::write8(byte reg, byte value) {
    _wire->beginTransmission(_addr);
    _wire->write(reg);
    _wire->write(value);
    _wire->endTransmission();
}

uint8_t BMP280m5::read8(byte reg) {
    _wire->beginTransmission(_addr);
    _wire->write(reg);
    _wire->endTransmission();
    _wire->requestFrom(_addr, (uint8_t)1);
    return _wire->read();
}

uint16_t BMP280m5::read16(byte reg) {
    uint8_t buffer[2];
    _wire->beginTransmission(_addr);
    _wire->write(reg);
    _wire->endTransmission();
    _wire->requestFrom(_addr, (uint8_t)2);
    buffer[0] = _wire->read();
    buffer[1] = _wire->read();
    return uint16_t(buffer[0]) << 8 | uint16_t(buffer[1]);
}

uint16_t BMP280m5::read16_LE(byte reg) {
    uint16_t temp = read16(reg);
    return (temp >> 8) | (temp << 8);
}

int16_t BMP280m5::readS16(byte reg) {
    return (int16_t)read16(reg);
}

int16_t BMP280m5::readS16_LE(byte reg) {
    return (int16_t)read16_LE(reg);
}

uint32_t BMP280m5::read24(byte reg) {
    uint8_t buffer[3];
    _wire->beginTransmission(_addr);
    _wire->write(reg);
    _wire->endTransmission();
    _wire->requestFrom(_addr, (uint8_t)3);
    buffer[0] = _wire->read();
    buffer[1] = _wire->read();
    buffer[2] = _wire->read();
    return uint32_t(buffer[0]) << 16 | uint32_t(buffer[1]) << 8 | uint32_t(buffer[2]);
}

#endif