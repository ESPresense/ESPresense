#pragma once
#ifdef SENSORS
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <string>

#include "soc/soc_caps.h"
// Second HP I2C controller? (C6 counts its LP I2C in SOC_I2C_NUM but has no I2C_NUM_1.)
#if defined(SOC_HP_I2C_NUM)
#define HAS_I2C_BUS_2 (SOC_HP_I2C_NUM > 1)
#else
#define HAS_I2C_BUS_2 (SOC_I2C_NUM > 1)
#endif

// Three I2C master buses: 1 and 2 use driver/i2c_master.h, 3 is bit-banged. bus is 1, 2 or 3; a bus that is not started
// fails every call. Sensors talk raw registers through these; no per-sensor driver library.
namespace I2C {
void ConnectToWifi(bool updating);
void Setup();
void SerialReport();

// Choose a bus for a device: 0 None, 1 / 2 hardware controllers, 3 the software (bit-banged) one.
// Sensors stop at bus 2 (maxBus); the battery gauge chips may also use bus 3.
// A device is on when its bus is not None; check started(bus) before talking to it.
int busSetting(const char* key, const char* label = "I2C Bus", int maxBus = 2);

// The address a sensor setting names: empty picks the first of the chip's addresses, otherwise
// the text ("0x77") must be one of them.
bool pickAddress(const std::string& setting, std::initializer_list<uint8_t> allowed, uint8_t& out);

bool started(int bus);
bool probe(int bus, uint8_t addr);
bool write(int bus, uint8_t addr, const uint8_t* data, size_t len);
bool read(int bus, uint8_t addr, uint8_t* data, size_t len);
bool writeRead(int bus, uint8_t addr, const uint8_t* w, size_t wlen, uint8_t* r, size_t rlen);
bool readReg(int bus, uint8_t addr, uint8_t reg, uint8_t* buf, size_t len);
bool writeReg(int bus, uint8_t addr, uint8_t reg, uint8_t val);
inline bool readReg8(int bus, uint8_t addr, uint8_t reg, uint8_t& v) { return readReg(bus, addr, reg, &v, 1); }
}  // namespace I2C
#endif
