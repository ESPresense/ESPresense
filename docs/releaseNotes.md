# Release Notes

### V0.0.6
#### 2019-07-18

- Updated platform `espressif32` to 1.9.0
- Fix for issue #14 OTA updates
- Updated `.pioenvs` folder

### V0.0.5
#### 2019-03-19

- Updated ArduinoJSON dependency to version 6.

### V0.0.4
#### 2019-02-10

- Added `maxDistance` setting to `config_local.h`. Unit is a non-zero integer in meters.
- Fix for AsyncTCP update with breaking changes

### V0.0.3
#### 2018-12-10

- Improved stability for the watchdog timer issue.
- Self-recovery for most crashes.
- Configurable LWT topic
- Over-the-Air updates are now possible from the command line of PlatformIO.
- On-board LED as status indicator

### V0.0.2
#### 2018-12-10
- Improvements to prevent watchdog timeout as in #1.
- PlatformIO build and upload support.
