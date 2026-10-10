#pragma once
#include <cstdint>

// AXP192 power management IC (M5StickC / StickC Plus) on I2C bus 2. The "axp192" hardware
// setting turns it on; the board template also sets I2C bus 2 to SDA 21 / SCL 22. Once the bus
// is up it sets charging (4.2 V), the power-key timing and the supply rails, and the battery
// reading comes from its fuel gauge instead of an ADC pin.
namespace AXP192 {
void ConnectToWifi(bool updating);
void Loop();
// True once the chip answered and was configured.
bool Ready();
// Battery voltage in mV, or -1 when not ready or no battery is present.
int BatteryMilliVolts();
bool Charging();
}  // namespace AXP192
