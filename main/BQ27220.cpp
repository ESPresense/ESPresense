#include "BQ27220.h"

#include "Battery.h"
#include "I2C.h"
#include "Settings.h"
#include "globals.h"

namespace BQ27220 {
static bool enabled = false, ready = false;

void ConnectToWifi(bool updating) {
    enabled = Battery::GaugeBus() && Battery::SelectedGauge() == Battery::Gauge::BQ27220;
}

bool Ready() { return ready; }

#ifdef SENSORS
static const uint8_t ADDR = 0x55;
// Standard commands, 16-bit little endian.
static const uint8_t CMD_VOLTAGE = 0x08;  // mV
static const uint8_t CMD_CURRENT = 0x0C;  // mA, signed, positive while charging
static const uint8_t CMD_SOC = 0x2C;      // %

static bool read16(uint8_t cmd, uint16_t& v) {
    uint8_t b[2];
    if (!I2C::readReg(Battery::GaugeBus(), ADDR, cmd, b, 2)) return false;
    v = b[0] | (b[1] << 8);
    return true;
}

// Check once bus is up: I2C starts with the settings, after ConnectToWifi ran. The T-Embed's
// peripheral power (GPIO 15) is switched on by an output, which may come up after this first
// try, so keep trying every few seconds before giving up.
void Loop() {
    static unsigned long lastTry = 0;
    static int tries = 0;
    if (!enabled || ready || !I2C::started(Battery::GaugeBus())) return;
    if (tries && millis() - lastTry < 3000) return;
    lastTry = millis();
    uint16_t mv;
    if (!read16(CMD_VOLTAGE, mv)) {
        if (++tries >= 10) {
            enabled = false;  // not there; stop retrying
            Log.println("BQ27220: no reply on I2C bus");
        }
        return;
    }
    Log.printf("BQ27220: found on I2C bus %d\n", Battery::GaugeBus());
    ready = true;
}

int BatteryMilliVolts() {
    uint16_t v;
    return ready && read16(CMD_VOLTAGE, v) ? v : -1;
}

int Percent() {
    uint16_t v;
    return ready && read16(CMD_SOC, v) ? (v > 100 ? 100 : v) : -1;
}

bool Charging() {
    uint16_t v;
    return ready && read16(CMD_CURRENT, v) && (int16_t)v > 0;
}
#else
void Loop() {}
int BatteryMilliVolts() { return -1; }
int Percent() { return -1; }
bool Charging() { return false; }
#endif
}  // namespace BQ27220
