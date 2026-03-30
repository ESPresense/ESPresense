# ESPresense

![GitHub release (latest by date)](https://img.shields.io/github/v/release/ESPresense/ESPresense)
![GitHub all releases](https://img.shields.io/github/downloads/ESPresense/ESPresense/total)
[![.github/workflows/main.yml](https://github.com/ESPresense/ESPresense/actions/workflows/build.yml/badge.svg)](https://github.com/ESPresense/ESPresense/actions/workflows/build.yml)


An ESP32 based presence detection node for use with the [Home Assistant](https://www.home-assistant.io/) [`mqtt_room` component](https://www.home-assistant.io/components/sensor.mqtt_room/) or [ESPresense-companion](https://github.com/ESPresense/ESPresense-companion) for indoor positioning

**Documentation:** https://espresense.com/

**Building:** [building](./BUILDING.md).

**Release Notes:** [changelog](./CHANGELOG.md).

## Battery voltage telemetry (ADC)

Battery telemetry is now available for any board with an ADC pin.

- By default this is **off** unless configured (`battery_adc_pin = -1`)
- Existing `macchina-a0` behavior stays enabled by default using board-specific defaults
- Telemetry is published on `espresense/rooms/<room>/telemetry` as:
  - `mV` (battery millivolts)
  - `batt` (optional battery percent when min/max configured)
  - `charging` (optional `ON`/`OFF` when charging threshold configured)

### Settings (Hardware page)

- `battery_adc_pin` — ADC GPIO pin (`-1` disables feature)
- `battery_adc_multiplier` — multiplier to convert ADC reading to millivolts
- `battery_adc_offset` — offset added to ADC counts before multiplier
- `battery_min_mv` — empty battery voltage for `%` calculation (`0` disables `%`)
- `battery_max_mv` — full battery voltage for `%` calculation (`0` disables `%`)
- `battery_charging_mv` — threshold for `charging` sensor (`0` disables charging sensor)

### Calibration guidance

1. Measure battery voltage with a multimeter.
2. Compare with ESPresense `mV` telemetry.
3. Adjust `battery_adc_multiplier` (and optionally `battery_adc_offset`) until they match.
4. Set `battery_min_mv` and `battery_max_mv` for your battery chemistry to enable `%`.

### Limitations / guardrails

- ADC pin must be ADC-capable; invalid pins disable the feature.
- On classic ESP32, ADC2 pins are rejected (WiFi uses ADC2). Use ADC1 pins (GPIO 32-39).
- This feature assumes external voltage divider hardware where needed; do not exceed the MCU ADC input range.
