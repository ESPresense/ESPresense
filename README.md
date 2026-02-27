# ESPresense

![GitHub release (latest by date)](https://img.shields.io/github/v/release/ESPresense/ESPresense)
![GitHub all releases](https://img.shields.io/github/downloads/ESPresense/ESPresense/total)
[![.github/workflows/main.yml](https://github.com/ESPresense/ESPresense/actions/workflows/build.yml/badge.svg)](https://github.com/ESPresense/ESPresense/actions/workflows/build.yml)


An ESP32 based presence detection node for use with the [Home Assistant](https://www.home-assistant.io/) [`mqtt_room` component](https://www.home-assistant.io/components/sensor.mqtt_room/) or [ESPresense-companion](https://github.com/ESPresense/ESPresense-companion) for indoor positioning

**Documentation:** https://espresense.com/

## WiFi AP fallback

ESPresense normally uses a captive portal (soft-AP) as a fallback for WiFi configuration.

If you want to **disable the AP entirely** (security / spectrum hygiene), set `disable_ap_mode: true` in the WiFi settings.

**Warning:** if AP mode is disabled and WiFi credentials are lost, the device cannot be recovered headlessly — you will need to reflash.

**Building:** [building](./BUILDING.md).

**Release Notes:** [changelog](./CHANGELOG.md).
