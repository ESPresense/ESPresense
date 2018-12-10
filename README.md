# ESP32-mqtt-room
An ESP32 based presence detection node for use with the [Home Assistant](https://www.home-assistant.io/) [`mqtt_room` component](https://www.home-assistant.io/components/sensor.mqtt_room/) for localized device presence detection.

This depends heavily on the hard work done by [pcbreflux](https://github.com/pcbreflux) and [nkolban](https://github.com/nkolban) both on GitHub and on their YouTube Channels. Specifically, it is a modified version of pcbreflux's [ESP32_BLE_beaconscan](https://github.com/pcbreflux/espressif/tree/master/esp32/arduino/sketchbook/ESP32_BLE_beaconscan).

## Getting Started
You can get started by cloning this repository to your local machine using git. Alternatively, you can [download the zip](https://github.com/jptrsn/ESP32-mqtt-room/archive/master.zip). To get the code onto your device, you can load it via the Arduino IDE or, thanks to some great work by [kylegordon](https://github.com/kylegordon), you can now build and upload using PlatformIO.

Create a copy of the `Settings.h` file, and rename it to `Settings_local.h`. This will allow you to set your own configuration without the risk of any commits to the repository of sensitive information.

Open the folder in Atom, using the `open project` option in the PlatformIO Home screen. Modify the settings in the `Settings_local.h` file to match your environment. Set the correct port in the `platformio.ini` file (or remove the line to use auto-detection), and upload to the board. Open the serial monitor after successful upload to check for success.

## Home Assistant Configuration
Once the device is running, it is important to configure Home Assistant to use the information from the MQTT topic to determine what devices to track. You can read the full documentation [on the Home Assistant website](https://www.home-assistant.io/components/sensor.mqtt_room/). It is critical that you configure your device IDs to include the Major version, but ignore Minor version (i.e. set it to 0). This is to match the configuration used on the ESP32.

### Sample Configuration.yaml Entry
Here is an example of how an entry into your `configuration.yaml` file should look:
```
sensor:
  - platform: mqtt_room
    device_id: "fda50693a4e24fb1afcfc6eb07647825-5-0" # Note that major version must match, but any minor version will be ignored
    name: 'iBeacon Room Presence'
    state_topic: 'room_presence'
    timeout: 60
    away_timeout: 120
```

## Trackable Devices
### iBeacon Hardware
Generic beacon hardware should be compatible, provided it meets a beacon standard (such as iBeacon or Eddystone). There are plenty of models based on the Nordic Semiconductor NRF51822 chip available from sellers such as [eBay](https://www.ebay.com/sch/i.html?_nkw=nrf51822+ibeacon) or [Ali Express](https://www.aliexpress.com/wholesale?SearchText=nrf51822+ibeacon).

### Tracking Android Phone
In order to track your Android phone as a BLE Beacon, you will need to emulate Beacon adertisement packets from your phone. This is the opposite to what most beacon apps do (i.e. scanning for beacons instead of emulating one). I have had success using the [Beacon Scope](https://play.google.com/store/apps/details?id=com.davidgyoungtech.beaconscanner) app. Configure a transmitter, and specify the device ID in your Home Assistant configuration files.

### Tracking iPhone
Unfortunately, Apple does not allow devices to advertise iBeacon data in the background.

#### Future Development Tasks
- [x] Implement basic BLE packet discovery
- [x] Implement iBeacon data packet parsing
- [x] Setup instructions in README.md
- [ ] Address watchdog issue
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
