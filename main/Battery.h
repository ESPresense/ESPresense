#pragma once
namespace Battery {
// Chip that measures the cell itself; its I2C bus setting turns it on. Read after ConnectToWifi.
enum class Gauge { AXP192, BQ27220 };
Gauge SelectedGauge();
// The I2C bus (1-3) the gauge chip is on, 0 when off (None).
int GaugeBus();
void ConnectToWifi(bool updating);
void Setup();
bool SendDiscovery();
void SendTelemetry();
}  // namespace Battery
