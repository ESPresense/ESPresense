#!/usr/bin/env bash
# Host-side unit tests for logic that needs no hardware (Unity from the IDF tree, ArduinoJson
# from the managed components). Run from the repo root inside an ESP-IDF environment.
set -euo pipefail
cd "$(dirname "$0")/.."
: "${IDF_PATH:?source export.sh first}"
UNITY="$IDF_PATH/components/unity/unity/src"
JSON=managed_components/bblanchon__arduinojson/src
[ -d "$JSON" ] || (cd test/deps && idf.py -DIDF_TARGET=esp32 reconfigure >/dev/null 2>&1 && cp -r managed_components ../../)
OUT=build/native; mkdir -p "$OUT"
cc -c -I"$UNITY" "$UNITY/unity.c" -o "$OUT/unity.o"
cc -c test/unity_stubs.c -o "$OUT/stubs.o"
rc=0
for t in test/test_native_*/*.cpp; do
  name=$(basename "$(dirname "$t")")
  g++ -std=gnu++17 -Wall -Wextra -I"$UNITY" -I"$JSON" -Imain "$t" "$OUT/unity.o" "$OUT/stubs.o" -o "$OUT/$name" || { rc=1; continue; }
  "$OUT/$name" || rc=1
done
exit $rc
