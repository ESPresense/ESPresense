# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

- Fix GUI of m5stickc

## [1.5.4]

- Fix beacons that send BOTH manufacture data and service uuids (#41)

## [1.5.3]

- Fix m5stickc init flag

## [1.5.2]

- Fix Close message not being logged out serial
- Don't trust inaccurate Apple txPower

## [1.5.1]

- Breaking Change: iBeacon id's were backwards, they are now correct and match HA Companion
- Bump NimBLE to 1.3.1
- Add support for blinking m5atom led, red=no wifi, blue=no mqtt, blinking white is packet seen
- Fix rssi@1m when multiple packets from same mac

## [1.5.0]

- Nothing changed, intended changes are in 1.5.1

## [1.4.4]

- Additional sonos fingerprinting
- Remove button handling from esp32 flavors (#34)

## [1.4.3]

- Add mifit fingerprint

## [1.4.2]

- Fix fingerprinting of KeySmart tiles

## [1.4.1]

- Fix errant voltage reporting

## [1.4.0]

- Fingerprints are now 3 tier: primary ids > publicMac > secondard ids.  A single mac can have multiple advertisements sent (especially certain beacons).  So it might take a few packets before the id settles into the correct one.
- Volts and temperature are now reported from Eddystone TLM advertisements
- Names should show up more reliably now as well

## [1.3.1]

- Turn off NTP by default

## [1.3.0]

- More changes to fingerprinting, id will be different if the device is sonos or samsung.  Those instead of saying md:<id> now use the manufacture name (samsung or sonos) as the prefix with the mac at the end
- Windows 10 we fingerprint as microsoft:<last 23 bytes in hex of manufacture data>. See https://petsymposium.org/2019/files/papers/issue3/popets-2019-0036.pdf for why, still not useful for a consistent fingerprint

## [1.2.0]

- Prefer mac address as ID if we have a static MAC and only md fingerprint
- Breaking: If you're using a md:*:* fingerprint, verify that it hasn't switched to the mac

## [1.1.2]

- Add support for clearing wifi setting via button on DevKit (as long as it's on GPIO15)

## [1.1.1]

- Fix fingerprinting of udm-pro
- Fix semaphore error

## [1.1.0]

- Go back to active scanning by default (so we can get the names of things)
- Send telemetry only every 15s
- Allow for disabling types of mqtt topics
- M5Sticks now have a UI showing devices that are close

## [1.0.1]

- Added esp32-noupdate

## [1.0.0] - 2021-08-20

- Initial
