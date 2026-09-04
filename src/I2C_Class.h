#ifndef _I2C_DEVICE_BUS_
#define _I2C_DEVICE_BUS_

// Vendored from https://github.com/m5stack/M5Unit-ENV/blob/master/src/I2C_Class.h
// Required for BMP280m5 driver used by ENVIV sensor

#include "Arduino.h"
#include "Wire.h"

class I2C_Class {
   private:
    TwoWire* _wire;
    uint8_t _scl;
    uint8_t _sda;
    long _freq;

   public:
    void begin(TwoWire* wire, uint8_t sda, uint8_t scl, long freq = 100000);
    bool exist(uint8_t addr);

    bool writeBytes(uint8_t addr, uint8_t reg, uint8_t* buffer, uint8_t length);
    bool readBytes(uint8_t addr, uint8_t reg, uint8_t* buffer, uint8_t length);

    bool readU16(uint8_t addr, uint8_t reg_addr, uint16_t* value);
    bool writeU16(uint8_t addr, uint8_t reg_addr, uint16_t value);

    bool writeByte(uint8_t addr, uint8_t reg, uint8_t data);
    uint8_t readByte(uint8_t addr, uint8_t reg);
    bool writeBitOn(uint8_t addr, uint8_t reg, uint8_t data);
    bool writeBitOff(uint8_t addr, uint8_t reg, uint8_t data);
};

#endif