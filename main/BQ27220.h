#pragma once

// BQ27220 battery fuel gauge (LILYGO T-Embed) address 0x55. The "batt_gauge_bus" hardware
// setting turns it on, with "batt_gauge" naming the chip; the board's SDA / SCL pins are set on the I2C bus 1 settings
// (T-Embed: SDA 8 / SCL 18). Battery voltage, charge percentage and charging state come from
// the gauge instead of an ADC pin.
namespace BQ27220 {
void ConnectToWifi(bool updating);
void Loop();
// True once the chip answered.
bool Ready();
// Battery voltage in mV, or -1 when not ready.
int BatteryMilliVolts();
// State of charge in percent as computed by the gauge, or -1 when not ready.
int Percent();
bool Charging();
}  // namespace BQ27220
