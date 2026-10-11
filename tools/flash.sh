#!/usr/bin/env bash
# Flash one variant, then capture the boot log (non-interactive, unlike `idf.py monitor`).
#   tools/flash.sh esp32s3 /dev/cu.usbmodem1101 [seconds=20]
# Log is shown and saved to build/<env>/serial.log.
set -euo pipefail
ENV="${1:?usage: tools/flash.sh <env> <port> [seconds]}"; PORT="${2:?port}"; SECS="${3:-20}"
cd "$(dirname "$0")/.."
if lsof "$PORT" >/dev/null 2>&1; then echo "$PORT is busy:"; lsof "$PORT"; exit 1; fi
. ~/esp/esp-idf/export.sh >/dev/null
# pipefail (set above) makes a failed build or flash stop here; only an empty grep is tolerated.
./build.sh "$ENV" -p "$PORT" flash | { grep -E "Hash of data verified|Hard resetting|rror" || true; }
python tools/serial_log.py "$PORT" "$SECS" | tee "build/$ENV/serial.log"
grep -m1 -E "IP address:|^IP: " "build/$ENV/serial.log" | sed 's/^.*: */IP: /' || echo "IP: not seen in log (try a longer capture)"
