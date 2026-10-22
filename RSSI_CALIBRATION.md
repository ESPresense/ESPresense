# RSSI calibration in v5: an additive receiver offset

Status: specification for v5.0.0. No firmware, UI or Companion behavior changes in this PR.

## Why

Up to now the receiver adjustment is subtracted:

```text
normalized_rssi = raw_rssi - rx_adj_rssi
```

so +5 means "this node reads 5 dB strong". That's backwards from what people expect of an
adjustment, and every ESP32-S3 gets +20 by default whatever the board. v5.0.0 already converts
v4 settings on first boot (LEDs, inputs, I2C pins), so it's the moment to fix the convention
once, with no mode to support afterwards.

## The v5 convention

```text
normalized_rssi = raw_rssi + rx_offset_rssi
distance        = 10 ^ ((rssi_1m - normalized_rssi) / (10 * absorption))
```

- **`rx_offset_rssi`**, integer dB, -100 to 100, **added**: +5 means "this node reads 5 dB weak, add
  5". Label: **Receiver offset (dB)**.
- **Default 0 on every chip.** No chip-wide S3 offset.
- Applied exactly once; everything downstream (distance, close/left, the Companion's fitting)
  works on normalized RSSI.

`rx_offset_rssi` is a new name rather than `rx_adj_rssi` with a flipped sign. It follows the
same `<rx|tx>_<what>_rssi` pattern as `tx_ref_rssi`, its counterpart on the transmit side. A retained
`rx_adj_rssi/set` message, an older Companion, or an old template would otherwise be read with
the wrong sign, and nothing in a bare number says which convention it was written in.

## First boot on v5

One first-boot step (`main/SettingsMigration.h`, run right after the settings filesystem mounts)
converts everything v5 stores differently, once; a `v5_migrated` marker keeps it from running
again. For calibration:

- If the node was configured before (WiFi is set) and `rx_offset_rssi` isn't stored, store
  `rx_offset_rssi = -(effective rx_adj_rssi)`: the stored value, or the old default when none was
  stored. Every existing node keeps its distances. An S3 that relied on the old +20 gets -20.
- A fresh install (no WiFi configured yet) starts at `rx_offset_rssi` 0 and gets any per-board offset
  from its template.
- The old `rx_adj_rssi` file is left in place, unused, so a downgrade still finds it.

The same step converts v4's hardware settings, for a node with any of them stored: the LED
settings (`led_1_pin`, ...) into the `leds` list (an LED turned off stays off), PIR/radar/button/
switch into the `inputs` list, and, for a node with an I2C sensor on bus 1 and no bus 1 pins, v4's
default pins for its chip (21/22, or 19/18 on the C3).

## Compatibility

- **`rx_adj_rssi/set` keeps working for now, as a deprecated alias:** the node stores
  `rx_offset_rssi = -value`, and an empty payload resets `rx_offset_rssi` to 0. A Companion that hasn't
  been updated keeps calibrating v5 nodes correctly. Log a deprecation line on use; remove the
  alias in a later major version.
- **Reports keep `rxAdj` in the old convention** (`rxAdj = -rx_offset_rssi`), so the Companion's
  existing `rssi + rxAdj - candidate` arithmetic keeps working. They add `rxOffset`.
- **The node publishes `rx_offset_rssi`** as a retained room setting. It stops publishing
  `rx_adj_rssi`, so the Companion can tell which convention a node uses from what it reports.
- **Templates use `rx_offset_rssi`.** A template carrying `rx_adj_rssi` is converted the same way as
  the MQTT alias.

## Per-board offsets live in templates

A receiver's offset belongs to the board and its antenna, not the chip. A board measured to
read weak or strong carries `"rx_offset_rssi": n` in its template on espresense.com; boards without a
measurement leave it out. The Companion's fitted per-node value overrides it, as any explicit
setting does.

Measurements come from the HIL bench: four boards and a BLE flood transmitter in fixed
positions. A step that records each board's smoothed RSSI of the flood dongle gives a per-board
offset relative to the others, and repeated runs show how stable it is. An offset goes into a
template only from such a measurement, noting the board, antenna and reference on its page.

## The Companion

When a node reports `rx_offset_rssi`, the Companion writes `rx_offset_rssi` and shows the offset in the new
convention; otherwise it keeps writing `rx_adj_rssi` (v4 and v5 betas). Its fitting keeps
working on `rxAdj` throughout, so this can ship after v5.0.0.

## Documentation

The upgrade guide says the receiver adjustment is now an additive offset and was converted
automatically, so distances don't change. The calibration guide describes `rx_offset_rssi` and gets
absorption's direction right: a higher absorption gives a shorter distance beyond 1 m.

## Tests

- **Normalization:** reference -65: raw -60 with `rx_offset_rssi` -5, and raw -70 with `rx_offset_rssi`
  +5, both give normalized -65 and 1 m.
- **First boot:** an S3 configured with no stored `rx_adj_rssi` gets `rx_offset_rssi` -20; a stored
  `rx_adj_rssi` 7 gets -7; a fresh install gets 0; an existing `rx_offset_rssi` is never touched;
  distances are unchanged across the upgrade.
- **Alias:** `rx_adj_rssi/set` 7 stores `rx_offset_rssi` -7; an empty payload resets `rx_offset_rssi`
  to 0; a template with `rx_adj_rssi` converts the same way.
- **Reports:** `rxAdj` equals `-rx_offset_rssi` and `rxOffset` equals `rx_offset_rssi`.
