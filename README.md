# ESP32-mqtt-room
An ESP32 based presence detection node for use with the Home Assistant mqtt_room component for localized device presence detection.

This depends heavily on the hard work done by [pcbreflux](https://github.com/pcbreflux) and [nkolban](https://github.com/nkolban) both on GitHub and on their YouTube Channels.

Specifically, it is a modified version of pcbreflux's [ESP32_BLE_beaconscan](https://github.com/pcbreflux/espressif/tree/master/esp32/arduino/sketchbook/ESP32_BLE_beaconscan)

## Configuration
Settings are defined in `Settings_local.h` which is not included in the repo by default. You can make a copy of `Settings.h` and rename it `Settings_local.h` in the same folder.
