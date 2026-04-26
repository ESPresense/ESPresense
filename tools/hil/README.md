# HIL helper scripts

Auxiliary scripts that run alongside the firmware-tester image in
[`.woodpecker/hil.yml`](../../.woodpecker/hil.yml). The firmware-tester image
itself ships the long-running monitor (`/scripts/hil_monitor.py`) and the
WiFi-provisioning helper (`/scripts/improv_wifi.py`); scripts here are
ESPresense-repo-local extensions that run before those, against the same
serial device.

## `check_no_bootloop.py`

Fast post-flash boot-loop watchdog. Reproduces issue
[#2244](https://github.com/ESPresense/ESPresense/issues/2244) ("ESP32-C3
boot loop after app entry on ESPresense C3 builds").

After `pio run -t upload`, this script captures ~30s of serial output and
fails fast if it sees:

- Any panic / Guru Meditation / Backtrace / `abort()` report.
- Two or more ROM bootloader `rst:` lines in the window (= the device
  rebooted more than once after the post-upload reset).
- Three or more bootloader entry headers (defense in depth against firmware
  that crashes before any panic line is printed).

A failure aborts the rest of the HIL pipeline (`improv_wifi.py`,
`hil_monitor.py`) for that board so the duration budget isn't burned on a
build that is known to be broken, and the captured log is dumped to stdout
plus written to `/tmp/${FIRMWARE_ENV}-bootlog.txt` for forensic upload.

### Run locally

```sh
python3 tools/hil/check_no_bootloop.py --port /dev/ttyUSB0 --window 30
```

### Tests

```sh
python3 tools/hil/test_check_no_bootloop.py
```

The unit tests cover the heuristic over canned serial captures (healthy
boot, the literal #2244 boot-loop trace, panic with backtrace, Guru
Meditation). They don't touch a serial device.
