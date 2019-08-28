# Release Notes

### [V0.0.6](https://github.com/jptrsn/ESP32-mqtt-room/releases/tag/v0.0.6)
#### 2019-07-18

- Updated platform `espressif32` to 1.9.0
- Fix for issue #14 OTA updates
- Updated `.pioenvs` folder

**A quick note about the update:**

If you want to update from a previous version to `v0.0.6`, you may have trouble connecting to the ESP32 over the air. If you see the error message in the console that looks like this:
> 13:53:51 [DEBUG]: Options: {'timeout': 10, 'esp_ip': '192.168.1.209', 'host_port': 27562, 'image': '.pio\\build\\esp32\\firmware.bin', 'host_ip': '0.0.0.0', 'auth': '', 'esp_port': 3232, 'spiffs': False, 'debug': True, 'progress': True}
13:53:51 [INFO]: Starting on 0.0.0.0:27562
13:53:51 [INFO]: Upload size: 1568912
Sending invitation to 192.168.1.209 ..........
13:55:31 [ERROR]: No response from the ESP
*** [upload] Error 1

then you can add the following to your `platformio.ini` file:

```
upload_flags =
  --port=3232
```
Save the file and run the upload command again. Once you've successfully uploaded `v0.0.6` to the device, you'll need to remove this modification, as `v0.0.6` listens on the default port `8266`

### [V0.0.5](https://github.com/jptrsn/ESP32-mqtt-room/releases/tag/v0.0.5)
#### 2019-03-19

- Updated ArduinoJSON dependency to version 6.

### [V0.0.4](https://github.com/jptrsn/ESP32-mqtt-room/releases/tag/v0.0.4)
#### 2019-02-10

- Added `maxDistance` setting to `config_local.h`. Unit is a non-zero integer in meters.
- Fix for AsyncTCP update with breaking changes

### [V0.0.3](https://github.com/jptrsn/ESP32-mqtt-room/releases/tag/v0.0.3)
#### 2018-12-10

- Improved stability for the watchdog timer issue.
- Self-recovery for most crashes.
- Configurable LWT topic
- Over-the-Air updates are now possible from the command line of PlatformIO.
- On-board LED as status indicator

### [V0.0.1](https://github.com/jptrsn/ESP32-mqtt-room/releases/tag/v0.0.1)
#### 2018-12-10
- Improvements to prevent watchdog timeout as in #1.
- PlatformIO build and upload support.
