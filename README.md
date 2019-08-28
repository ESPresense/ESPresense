See the pretty documentation at [https://jptrsn.github.io/ESP32-mqtt-room/](https://jptrsn.github.io/ESP32-mqtt-room/)

# ESP32-mqtt-room
An ESP32 based presence detection node for use with the [Home Assistant](https://www.home-assistant.io/) [`mqtt_room` component](https://www.home-assistant.io/components/sensor.mqtt_room/) for localized device presence detection.

## Release Notes
View the [release notes here](./releaseNotes).

## Credits
This project would not have been possible without the help of a variety of [folks on GitHub and YouTube.](https://jptrsn.github.io/ESP32-mqtt-room/index.html#credits)

## Getting Started
A full list of instructions is available in the [documentation](https://jptrsn.github.io/ESP32-mqtt-room/index.html#getting-started).

## Home Assistant Configuration
See the [documentation for configuring Home Assistant.](https://jptrsn.github.io/ESP32-mqtt-room/home_assistant.html)

## Trackable Devices
* [Beacon Hardware](https://jptrsn.github.io/ESP32-mqtt-room/beacons.html)
* [Android devices](https://jptrsn.github.io/ESP32-mqtt-room/android.html)
* [iOS devices](https://jptrsn.github.io/ESP32-mqtt-room/index.html#tracking-iphone)
* [Other devices](https://jptrsn.github.io/ESP32-mqtt-room/beacons.html#non-beacon-hardware)

## Development Tasks

### To do
See the [list of open issues tagged as enhancement](https://github.com/jptrsn/ESP32-mqtt-room/labels/enhancement). You can also view the state of development tasks [by viewing the development tasks project board](https://github.com/jptrsn/ESP32-mqtt-room/projects/1).

### Completed
- [x] Implement basic BLE packet discovery
- [x] Implement iBeacon data packet parsing
- [x] Setup instructions in README.md
- [x] Address watchdog issue
- [x] Maximum distance limit (rounded to two decimal places)
- [x] Scan interval Settings
- [x] Implement Over-The-Air (OTA) updates
- [x] Build and upload via [PlatformIO](platformio.org)
