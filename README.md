# ESPresense

[![.github/workflows/main.yml](https://github.com/ESPresense/ESPresense/actions/workflows/main.yml/badge.svg)](https://github.com/ESPresense/ESPresense/actions/workflows/main.yml)

An ESP32 based presence detection node for use with the [Home Assistant](https://www.home-assistant.io/) [`mqtt_room` component](https://www.home-assistant.io/components/sensor.mqtt_room/) for localized device presence detection.

Differences from ESP32-room-assistant:
 * Uses a fingerprint instead of mac addresses so we can track randomized mac devices (like Apple Watches and iOS devices)
 * Fully supports using Tiles as trackers
 * Filters rssi via a 3 value media filter with a Kalman filter on top
 * Fully multithreaded, uses constant BLE scanning
 * Additional [AppDaemon app](https://github.com/ESPresense/ad-espresense-ips) will post process mqtt topics into ips topic (x,y,z) and (latt,long,elevation)

## Release Notes
View the [changelog here](./CHANGELOG.md).

## Credits

This project is a Fork/rewrite of ESP32-room-assistant (https://jptrsn.github.io/ESP32-mqtt-room/index.html#credits).  All credit for the intial codebase goes to them!
