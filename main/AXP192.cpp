#include "AXP192.h"

#include <soc/soc_caps.h>

#include "I2C.h"
#include "util.h"

#include "Settings.h"
#include "globals.h"

namespace AXP192 {
static bool enabled = false, ready = false;

void ConnectToWifi(bool updating) {
    enabled = Settings::checkbox("axp192", false, "AXP192 power chip on I2C bus 2 (M5StickC)");
}

bool Ready() { return ready; }

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

static void configure() {
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

// Configure once bus 2 is up: I2C starts with the settings, after the old Setup() ran.
void Loop() {
    if (!enabled || ready || !I2C::started(BUS)) return;
    uint8_t status;
    if (!I2C::readReg8(BUS, ADDR, 0x00, status)) {
        enabled = false;  // not there; don't retry every loop
        Log.println("AXP192: no reply on I2C bus 2");
        return;
    }
    configure();
    ready = true;
}

int BatteryMilliVolts() {
    if (!ready || !(Read8bit(0x01) & 0x20)) return -1;  // bit 5: battery present
    // 12 bits in 0x78 (high 8) / 0x79 (low 4), 1.1 mV per step.
    int raw = (Read8bit(0x78) << 4) | (Read8bit(0x79) & 0x0F);
    return raw * 11 / 10;
}

bool Charging() { return ready && (Read8bit(0x01) & 0x40); }  // bit 6: charging
#else
void Loop() {}
int BatteryMilliVolts() { return -1; }
bool Charging() { return false; }
#endif
}  // namespace AXP192
