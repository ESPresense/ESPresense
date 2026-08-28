#include "AXP192.h"

#include <soc/soc_caps.h>

#include "I2C.h"
#include "util.h"

// M5StickC power management IC on I2C bus 2 (SDA 21 / SCL 22, started by I2C::ConnectToWifi
// from the M5STICK defaults in defaults.h).
namespace AXP192 {
#if HAS_I2C_BUS_2
static const int BUS = 2;
static const uint8_t ADDR = 0x34;

uint8_t Read8bit(uint8_t Addr) {
    uint8_t v = 0;
    I2C::readReg8(BUS, ADDR, Addr, v);
    return v;
}

void Write1Byte(uint8_t Addr, uint8_t Data) {
    I2C::writeReg(BUS, ADDR, Addr, Data);
}

void SetLDO2(bool State) {
    uint8_t buf = Read8bit(0x12);
    if (State == true)
        buf = (1 << 2) | buf;
    else
        buf = ~(1 << 2) & buf;
    Write1Byte(0x12, buf);
}

void Setup() {
    if (!I2C::started(BUS)) {
        log_e("AXP192: I2C bus 2 not started");
        return;
    }
    Write1Byte(0x28, 0xcc);
    Write1Byte(0x82, 0xff);
    Write1Byte(0x33, 0xc0);
    Write1Byte(0x82, 0xff);
    Write1Byte(0x12, Read8bit(0x12) | 0x4D);
    Write1Byte(0x36, 0x0C);
    Write1Byte(0x91, 0xF0);
    Write1Byte(0x90, 0x02);
    Write1Byte(0x30, 0x80);
    Write1Byte(0x39, 0xfc);
    Write1Byte(0x35, 0xa2);
    Write1Byte(0x32, 0x46);
    Write1Byte(0x28, 0xec);
}

void Loop() {
}
#else
void Setup() {}
void Loop() {}
#endif
}  // namespace AXP192
