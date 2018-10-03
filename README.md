# ESP32-mqtt-room
An ESP32 based presence detection node for use with the Home Assistant mqtt_room component for localized device presence detection.

This depends heavily on the hard work done by [pcbreflux](https://github.com/pcbreflux) and [nkolban](https://github.com/nkolban) both on GitHub and on their YouTube Channels. Specifically, it is a modified version of pcbreflux's [ESP32_BLE_beaconscan](https://github.com/pcbreflux/espressif/tree/master/esp32/arduino/sketchbook/ESP32_BLE_beaconscan).

## Getting Started
1. You can get started by cloning this repository to your local machine using git. Alternatively, you can [download the zip](https://github.com/jptrsn/ESP32-mqtt-room/archive/master.zip).
2. Make a copy of the `Settings.h` file, and rename it `Settings_local.h`. Fill in the required information in your local settings file (the local file is ignored by GitHub, so you won't upload your sensitive information).

### Configuration
3. Settings are defined in `Settings_local.h` which is not included in the repo by default. Make a copy of `Settings.h` and rename it `Settings_local.h` in the same folder. Fill in your required settings.

### Build and Upload
4. Open the `ESP32-mqtt-room.ino` file in the Arduino IDE, and set the board variant to ESP32. You'll need to use the large partition scheme to get it to fit your board.
5. Compile and upload. Ensure that you've got all required libraries installed (and do not use the beta version of any of them).

_**TBC**_

#### Future Development Tasks
- [x] Implement basic BLE packet discovery
- [x] Implement iBeacon data packet parsing
- [ ] Setup instructions in README.md
- [ ] Match configuration options to Room Assistant
- [ ] Implement Mi Flora data parsing and reporting
- [ ] Whitelist functionality
- [ ] Blacklist functionality
- [ ] Maximum distance limit
- [ ] Scan interval Settings
- [ ] Configuration via Web UI
- [ ] Wifi Manager for managing access point credentials
- [ ] Implement Over-The-Air (OTA) updates
- [ ] Build and upload via [PlatformIO](platformio.org)
