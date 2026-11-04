#pragma once

// UART mmWave presence radar: the "radar" setting on the hardware endpoint, a JSON object such as
//   {"model": "ld2410", "rx": 16}
// model: ld2410 (HLK-LD2410 / B / C), ld2450 (HLK-LD2450) or c4001 (DFRobot Gravity C4001,
// SEN0609 / SEN0610). Optional "baud": default 256000 for the Hi-Link modules, 9600 for the C4001.
// Only the module's TX goes to the node's RX pin; nothing is ever sent to the module, so its own
// settings (range, sensitivity, hold time, the C4001's run mode) stay as set with the vendor's
// tool (HLKRadarTool, DFRobot's library).
// All models publish radar (occupancy, ON/OFF on change); the LD2410 and LD2450 also publish
// radar_moving and radar_still. Numbers go out at most once a second while they change
// (distances in cm, speeds in mm/s):
//   ld2410: radar_moving_distance, radar_still_distance, radar_detection_distance,
//           radar_moving_energy, radar_still_energy (0-100)
//   ld2450: radar_targets (0-3), radar_target_<1-3>_x / _y / _speed / _distance
//           (y is straight out from the sensor, x is left/right; 0 for an empty slot)
//   c4001:  radar_distance, radar_speed, radar_energy, only in speed mode ($DFDMD lines)
// Presence also drives Motion-control LEDs. With no frames for 5 s the radar reports clear.
// UART: 2 on chips that have three, else 1, which the CSE7766 power monitor also uses; a node
// can't have both on an ESP32-C3/C6 (no plug has a radar).
namespace Radar {
void ConnectToWifi(bool updating);
void Setup();
void SerialReport();
void Loop();
bool SendDiscovery();
bool SendOnline();
}  // namespace Radar
