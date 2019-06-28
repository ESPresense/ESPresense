<img src='./images/cover.jpg'>

# ESP32-mqtt-room
An ESP32 based presence detection node for use with the [Home Assistant](https://www.home-assistant.io/) [`mqtt_room` component](https://www.home-assistant.io/components/sensor.mqtt_room/) for localized device presence detection.

## Credits
This depends heavily on the hard work done by a number of people.
* pcbreflux [(GitHub)](https://github.com/pcbreflux) [(YouTube)](https://www.youtube.com/channel/UCvsMfEoIu_ZdBIgQVcY_AZA) - this code takes heavy inspiration from [ESP32_BLE_beaconscan](https://github.com/pcbreflux/espressif/tree/master/esp32/arduino/sketchbook/ESP32_BLE_beaconscan)
* Neil Kolban [(GitHub)](https://github.com/nkolban) [(YouTube)](https://www.youtube.com/channel/UChKn_BlaVrMrhEquPNI6HuQ) - provided the Bluetooth low enery libraries used
* Marvin Roger [(GitHub)](https://github.com/marvinroger/async-mqtt-client) - provided the async MQTT library
* Benoit Blanchon [(GitHub)](https://github.com/bblanchon/ArduinoJson) - provided the Arduino JSON library
* Me No Dev [(GitHub)](https://github.com/me-no-dev/AsyncTCP) - provided the async TCP library
* Kyle Gordon [(GitHub)](https://github.com/kylegordon) - PlatformIO support

### Requirements
You're going to need the following items:
* An [ESP32 development board](https://www.amazon.ca/gp/product/B07F1GWJ1N/ref=as_li_tl?ie=UTF8&camp=15121&creative=330641&creativeASIN=B07F1GWJ1N&linkCode=as2&tag=ptrsnja-20&linkId=7ccc8364c8a8961ac9053e03efa91268)
* A running instance of [Home Assistant](https://www.homeassistant.io)
* The [Atom](https://ide.atom.io/) IDE with the [PlatformIO](https://docs.platformio.org/en/latest/ide/atom.html) package installed.
* A [Bluetooth Low Energy beacon](https://www.amazon.ca/gp/product/B07S3DQM6N/ref=as_li_tl?ie=UTF8&camp=15121&creative=330641&creativeASIN=B07S3DQM6N&linkCode=as2&tag=ptrsnja-20&linkId=2f55c80199c88db853bd583182384f50) (either iBeacon or EddyStone), or
* An Android phone with a beacon app installed.

## Getting Started

### Flashing the Hardware (ESP32)
You can get started by cloning this repository to your local machine using git. Alternatively, you can [download the zip](https://github.com/jptrsn/ESP32-mqtt-room/archive/master.zip). To get the code onto your device, thanks to some great work by [kylegordon](https://github.com/kylegordon), you can now build and upload using PlatformIO.

### Define your settings
Create a copy of the `Settings.h` file, and rename it to `Settings_local.h` or something similar. Any variation of the name `Settings` followed by an underscore will be ignored by git. This will allow you to set your own configuration without the risk of any commits to the repository of sensitive information. It can also be helpful to keep a settings file for each device you will run, as they will differ.

### Flashing the device via USB
Open the project folder in Atom, using the `open project` option in the PlatformIO Home screen. Modify the settings in the `Settings_local.h` file to match your environment. Set the correct port in the `platformio.ini` file (or remove the line to use auto-detection), and upload to the board. Open the serial monitor after successful upload to check for success.

### Flashing/updating via OTA
It is possible to update the device using "Over the Air" (OTA) updates from the command line interface of PlatformIO. You will need to know the IP address of the device itself (check your router). From the command line, enter the command `platformio run -t upload --upload-port {{Device IP Address}}`. During the update process, you will see the on-board LED blinking slowly. Once the update has completed, you should see the device reconnect and update its telemetry.

## Home Assistant Configuration
See the section on [configuring Home Assistant](./home_assistant.md).

## Trackable Devices
### Beacon Hardware
See the section on [tracking beacon hardware](./beacons.md)

### Tracking Android Phone
[Read more here](./android.md) about getting your Android phone configured and tracked.

### Tracking iPhone
Unfortunately, Apple does not allow devices to advertise iBeacon data in the background. As I do not own an iPhone, I will not attempt to implement scanning and reporting by device name, but I welcome pull requests.
