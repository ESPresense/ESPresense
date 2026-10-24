#pragma once
namespace Battery {
// Chip that measures the cell itself (the "Battery gauge" setting); read after ConnectToWifi.
enum class Gauge { None, AXP192, BQ27220 };
Gauge SelectedGauge();
// The I2C bus (1-3) the gauge chip is on, 0 when none is chosen. Bus 3 is for the AXP192 only.
int GaugeBus();
void ConnectToWifi(bool updating);
void Setup();
bool SendDiscovery();
void SendTelemetry();
}  // namespace Battery
