# RSSI calibration: the Legacy checkbox

Status: specification. No firmware, UI or Companion behavior changes in this PR.

## Today

```text
normalized_rssi = raw_rssi - rx_adj_rssi
distance        = 10 ^ ((rssi_1m - normalized_rssi) / (10 * absorption))
```

`rx_adj_rssi` is subtracted: +5 means "this node reads 5 dB strong". That's backwards from what
people expect of an adjustment, and every ESP32-S3 gets +20 by default whatever the board.

## The checkbox

A **Legacy** checkbox under Calibration, on by default, so nothing changes unless a node is
switched. Off ("modern") fixes the convention.

| | Legacy (default) | Modern |
| --- | --- | --- |
| Receiver setting | `rx_adj_rssi`, subtracted | `rx_offset`, **added** |
| Meaning of +5 | this node reads 5 dB strong | this node reads 5 dB weak, add 5 |
| Formula | `normalized = raw - rx_adj_rssi` | `normalized = raw + rx_offset` |
| Default | +20 on ESP32-S3, 0 elsewhere | 0 on every chip |
| Per-board offset | built into the S3 default | from the board's template |

The offset is applied exactly once in both modes; everything downstream (distance, close/left,
the Companion's fitting) works on normalized RSSI.

## A new setting name, not a flipped sign

Modern uses `rx_offset`; `rx_adj_rssi` keeps its meaning forever. A retained
`rx_adj_rssi/set` message, an older Companion, or a template written before the switch would
otherwise be read with the wrong sign, and nothing in the value says which convention it was
written in. With two names every value carries its meaning. A Legacy node ignores `rx_offset`
and a modern node ignores `rx_adj_rssi`; both are kept.

## Switching converts once and changes no distances

Turning Legacy off stores `rx_offset = -(effective rx_adj_rssi)`, including an unset S3's +20
(giving -20), then restarts. Turning it back on stores the matching `rx_adj_rssi`. A node
reports the same normalized RSSI and distances before and after the switch; explicit values
convert exactly.

## Per-board offsets live in templates

A receiver's offset belongs to the board and its antenna, not the chip. A board measured to
read weak or strong carries `"rx_offset": n` in its template on espresense.com; boards without a
measurement leave it out. The Companion's fitted per-node value still overrides it, as any
explicit setting does.

Measurements come from the HIL bench: four boards and a BLE flood transmitter in fixed
positions. A step that records each board's smoothed RSSI of the flood dongle gives a per-board
offset relative to the others, and repeated runs show how stable it is. An offset goes into a
template only from such a measurement, noting the board, antenna and reference on its page.

## Reporting and the Companion

Device reports keep `rxAdj`, always in the Legacy convention (modern reports `-rx_offset`), so
older Companion versions keep working, and add `legacy: true/false`. The node publishes
`legacy` and `rx_offset` as retained room settings. The Companion reads the mode and writes
whichever setting the node uses, showing the offset in the modern convention.

## Later

Once the Companion and the templates carry `rx_offset`, Legacy can default to off for new
installs (a node with no WiFi configured yet). Existing nodes stay in Legacy until switched.

## Tests

- **Normalization:** reference -65, raw -60: Legacy `rx_adj_rssi` +5 and modern `rx_offset` -5
  both give normalized -65 and 1 m.
- **Switch:** an unset S3 switched to modern stores `rx_offset` -20 and reports the same
  distances; switching back restores `rx_adj_rssi` 20; explicit values convert exactly, and
  each switch restarts once.
- **Names:** a Legacy node ignores `rx_offset/set` and a modern node ignores `rx_adj_rssi/set`;
  neither changes the active offset.
- **Reports:** `rxAdj` equals `-rx_offset` in modern mode, and `legacy` matches the checkbox.
