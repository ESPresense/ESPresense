# ESP32-mqtt-room
An ESP32 based presence detection node for use with the [Home Assistant](https://www.home-assistant.io/) [`mqtt_room` component](https://www.home-assistant.io/components/sensor.mqtt_room/) for localized device presence detection.

This depends heavily on the hard work done by [pcbreflux](https://github.com/pcbreflux) and [nkolban](https://github.com/nkolban) both on GitHub and on their YouTube Channels. Specifically, it is a modified version of pcbreflux's [ESP32_BLE_beaconscan](https://github.com/pcbreflux/espressif/tree/master/esp32/arduino/sketchbook/ESP32_BLE_beaconscan).

## Getting Started

You can get started by cloning this repository to your local machine using git. Alternatively, you can [download the zip](https://github.com/jptrsn/ESP32-mqtt-room/archive/master.zip). To get the code onto your device, you can load it via the Arduino IDE or, thanks to some great work by [kylegordon](https://github.com/kylegordon), you can now build and upload using PlatformIO.

Open the folder in Atom, using the `open project` option in the PlatformIO Home screen. Modify the settings in the `Settings_Cl.h` file to match your environment. Set the correct port in the `platformio.ini` file (or remove the line to use auto-detection), and upload to the board. Open the serial monitor after successful upload to check for success.

#### Future Development Tasks
- [x] Implement basic BLE packet discovery
- [x] Implement iBeacon data packet parsing
- [x] Setup instructions in README.md
- [ ] Match configuration options to Room Assistant
- [ ] Implement Mi Flora data parsing and reporting
- [ ] Whitelist functionality
- [ ] Blacklist functionality
- [ ] Maximum distance limit
- [x] Scan interval Settings
- [ ] Configuration via Web UI
- [ ] Wifi Manager for managing access point credentials
- [ ] Implement Over-The-Air (OTA) updates
- [x] Build and upload via [PlatformIO](platformio.org)
