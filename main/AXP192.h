#pragma once
#include <cstdint>

// AXP192 power management IC (M5StickC / StickC Plus) on an I2C bus. The "batt_gauge_bus"
// hardware setting turns it on, with "batt_gauge" naming the chip (M5StickC: SDA 21 / SCL 22, often bus 3). Once the bus
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
