#include "I2C_Class.h"

void I2C_Class::begin(TwoWire *wire, uint8_t sda, uint8_t scl, long freq) {
    _wire = wire;
    _sda  = sda;
    _scl  = scl;
    _freq = freq;
    _wire->end();
    _wire->begin(static_cast<int>(_sda), _scl, freq);
}

bool I2C_Class::exist(uint8_t addr) {
    int error;
    _wire->beginTransmission(addr);
    error = _wire->endTransmission();
    if (error == 0) {
        return true;
    }
    return false;
}

bool I2C_Class::writeBytes(uint8_t addr, uint8_t reg, uint8_t *buffer,
                           uint8_t length) {
    _wire->beginTransmission(addr);
    _wire->write(reg);
    _wire->write(buffer, length);
    if (_wire->endTransmission() == 0) return true;
    return false;
}

bool I2C_Class::readBytes(uint8_t addr, uint8_t reg, uint8_t *buffer,
                          uint8_t length) {
    uint8_t index = 0;
    _wire->beginTransmission(addr);
    _wire->write(reg);
    _wire->endTransmission();
    if (_wire->requestFrom(addr, length)) {
        for (uint8_t i = 0; i < length; i++) {
            buffer[index++] = _wire->read();
        }
        return true;
    }
    return false;
}

bool I2C_Class::readU16(uint8_t addr, uint8_t reg_addr, uint16_t *value) {
    uint8_t read_buf[2] = {0x00, 0x00};
    bool result         = readBytes(addr, reg_addr, read_buf, 2);
    *value              = (read_buf[0] << 8) | read_buf[1];
    return result;
}

bool I2C_Class::writeU16(uint8_t addr, uint8_t reg_addr, uint16_t value) {
    uint8_t write_buf[2];
    write_buf[0] = value >> 8;
    write_buf[1] = value & 0xff;
    return writeBytes(addr, reg_addr, write_buf, 2);
}

bool I2C_Class::writeByte(uint8_t addr, uint8_t reg, uint8_t data) {
    _wire->beginTransmission(addr);
    _wire->write(reg);
    _wire->write(data);
    if (_wire->endTransmission() == 0) return true;
    return false;
}

uint8_t I2C_Class::readByte(uint8_t addr, uint8_t reg) {
    _wire->beginTransmission(addr);
    _wire->write(reg);
    _wire->endTransmission();

    if (_wire->requestFrom(addr, 1)) {
        return _wire->read();
    }
    return 0;
}

bool I2C_Class::writeBitOn(uint8_t addr, uint8_t reg, uint8_t data) {
    uint8_t temp;
    uint8_t write_back;
    temp       = readByte(addr, reg);
    write_back = (temp | data);
    return (writeByte(addr, reg, write_back));
}

bool I2C_Class::writeBitOff(uint8_t addr, uint8_t reg, uint8_t data) {
    uint8_t temp;
    uint8_t write_back;
    temp       = readByte(addr, reg);
    write_back = (temp & (~data));
    return (writeByte(addr, reg, write_back));
}
