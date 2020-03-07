![Cover Image](./images/cover.jpg)

An ESP32 based presence detection node for use with the [Home Assistant](https://www.home-assistant.io/) [`mqtt_room` component](https://www.home-assistant.io/components/sensor.mqtt_room/) for localized device presence detection.

# Contents
1. [Requirements](#requirements)
2. [Getting Started](#getting-started)
    1. [Flashing the Hardware](#flashing-the-hardware)
    2. [Define Settings](#define-your-settings)
    3. [Flashing the device via USB](#flashing-the-device-via-usb)
    4. [Flashing/updating via OTA](#flashing-via-ota)
3. [Home Assistant Configuration](#home-assistant-configuration)
4. [Trackable Devices](#trackable-devices)
    1. [Beacons](#beacons)
    2. [Generic BLE Devices](#generic-ble-devices)
    3. [Android Phones](#tracking-android-phone)
    4. [iPhones](#tracking-iphone)
5. [Sensors](#sensors)
6. [Credits](#credits)
7. [Troubleshooting](#troubleshooting)
8. [Release Notes](#release-notes)


## Requirements
You're going to need the following items:
* An [ESP32 development board](https://www.amazon.ca/gp/product/B07F1GWJ1N/ref=as_li_tl?ie=UTF8&camp=15121&creative=330641&creativeASIN=B07F1GWJ1N&linkCode=as2&tag=ptrsnja-20&linkId=7ccc8364c8a8961ac9053e03efa91268)
* A running instance of [Home Assistant](https://www.homeassistant.io)
* The [Atom](https://ide.atom.io/) IDE with the [PlatformIO](https://docs.platformio.org/en/latest/ide/atom.html) package installed.
* A [Bluetooth Low Energy beacon](https://www.amazon.ca/gp/product/B07S3DQM6N/ref=as_li_tl?ie=UTF8&camp=15121&creative=330641&creativeASIN=B07S3DQM6N&linkCode=as2&tag=ptrsnja-20&linkId=2f55c80199c88db853bd583182384f50) (either iBeacon or EddyStone), or
* An Android phone with a beacon app installed.

## Getting Started

### Flashing the Hardware
You can get started by cloning this repository to your local machine using git. Alternatively, you can [download the zip](https://github.com/jptrsn/ESP32-mqtt-room/archive/master.zip). To get the code onto your device, thanks to some great work by [kylegordon](https://github.com/kylegordon), you can now build and upload using PlatformIO.

### Define your settings
Create a copy of the `Settings.h` file, and rename it to `Settings_local.h` or something similar. Any variation of the name `Settings` followed by an underscore will be ignored by git. This will allow you to set your own configuration without the risk of any commits to the repository of sensitive information. It can also be helpful to keep a settings file for each device you will run, as they will differ.

### Flashing the device via USB
Open the project folder in Atom, using the `open project` option in the PlatformIO Home screen. Modify the settings in the `Settings_local.h` file to match your environment. Set the correct port in the `platformio.ini` file (or remove the line to use auto-detection), and upload to the board. Open the serial monitor after successful upload to check for success.

### Flashing via OTA
It is possible to update the device using "Over the Air" (OTA) updates from the command line interface of PlatformIO. You will need to know the IP address of the device itself (check your router). From the command line, enter the command `platformio run -t upload --upload-port {{Device IP Address}}`. During the update process, you will see the on-board LED blinking slowly. Once the update has completed, you should see the device reconnect and update its telemetry.

## Home Assistant Configuration
See the section on [configuring Home Assistant](./home_assistant).

## Trackable Devices
### Beacons
See the section on [tracking beacons](./beacons)

### Generic BLE Devices
Some other devices that advertise as BLE (such as the Mi Flora plant sensors or the Mi Band fitness tracker) can also be tracked, as long as you can scan the device and see its hardware ID. See the section on [tracking generic BLE devices](./generic_ble).

### Tracking Android Phone
[Read more here](./android) about getting your Android phone configured and tracked.

### Tracking iPhone
Unfortunately, Apple does not allow devices to advertise iBeacon data in the background. As I do not own an iPhone, I will not attempt to implement scanning and reporting by device name, but I welcome pull requests.

## Sensors
It is now possible to use the HTU21D i²c temperature and humidity sensor. By enabling and defining the `htuSensorTopic` property, the device will bring in the required file and connect to the sensor. Be sure to use the built-in hardware i2c bus pins, SDA  on GPIO21 and SCL on GPIO22. While pull-up resistors are recommended, if you have short wires connecting the sensor to the board, you should be fine. Values will be published to the topic you defined, with `/temperature` appended for temperature measurements, and `/humidity` appended for humidity measurements.

## Credits
This depends heavily on the hard work done by a number of people.
* **pcbreflux** [(GitHub)](https://github.com/pcbreflux) [(YouTube)](https://www.youtube.com/channel/UCvsMfEoIu_ZdBIgQVcY_AZA) - this code takes heavy inspiration from [ESP32_BLE_beaconscan](https://github.com/pcbreflux/espressif/tree/master/esp32/arduino/sketchbook/ESP32_BLE_beaconscan)
* **Neil Kolban** [(GitHub)](https://github.com/nkolban) [(YouTube)](https://www.youtube.com/channel/UChKn_BlaVrMrhEquPNI6HuQ) - provided the [Bluetooth low enery libraries](https://github.com/nkolban/esp32-snippets) used
* **Marvin Roger** [(GitHub)](https://github.com/marvinroger) - provided the [Async MQTT library](http://marvinroger.viewdocs.io/async-mqtt-client/)
* **Benoit Blanchon** [(GitHub)](https://github.com/bblanchon) [(YouTube)](https://www.youtube.com/channel/UC8HZRqN4wfytHfRGMLUQWkQ) - provided the [Arduino JSON library](https://arduinojson.org/)
* **Me No Dev** [(GitHub)](https://github.com/me-no-dev) - provided the [Async TCP library](https://github.com/me-no-dev/AsyncTCP)
* **Kyle Gordon** [(GitHub)](https://github.com/kylegordon) - wrote the necessary code for PlatformIO support

## Troubleshooting
If you're having difficulty getting things set up and work properly, check out [the troubleshooting section](./troubleshooting) before you open an issue. It will help you determine what information you need to provide to identify and fix what's gone wrong.

## Release Notes
If you're having difficulty updating your device to the newest version, or you're just curious, [check out the release notes.](./releaseNotes)
