#pragma once
#ifdef SENSORS
#include <cstddef>
#include <cstdint>

#include "soc/soc_caps.h"
// Second HP I2C controller? (C6 counts its LP I2C in SOC_I2C_NUM but has no I2C_NUM_1.)
#if defined(SOC_HP_I2C_NUM)
#define HAS_I2C_BUS_2 (SOC_HP_I2C_NUM > 1)
#else
#define HAS_I2C_BUS_2 (SOC_I2C_NUM > 1)
#endif

// Two I2C master buses (driver/i2c_master.h). bus is 1 or 2; a bus that is not started
// fails every call. Sensors talk raw registers through these; no per-sensor driver library.
namespace I2C {
void ConnectToWifi(bool updating);
void Setup();
void SerialReport();

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
