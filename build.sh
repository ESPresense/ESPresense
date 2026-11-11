#!/usr/bin/env bash
# Build one firmware variant: ./build.sh <env> [idf.py args...]
#   ./build.sh esp32c3-cdc            -> build/esp32c3-cdc/espresense.bin
#   ./build.sh esp32 -p /dev/ttyUSB0 flash monitor
# Variant -> chip target and sdkconfig fragments. Compile definitions are in envs.cmake.
set -euo pipefail
ENV="${1:?usage: build.sh <env> [idf.py args]}"; shift
case "$ENV" in
  esp32c3*) TARGET=esp32c3 ;;
  esp32c6*) TARGET=esp32c6 ;;
  esp32s3*) TARGET=esp32s3 ;;
  *)        TARGET=esp32 ;;
esac
DEFAULTS="sdkconfig.defaults"
case "$ENV" in *-cdc|*-cdc-*)     DEFAULTS="$DEFAULTS;sdkconfig.cdc" ;; esac
case "$ENV" in *-verbose|*-verbose-*) DEFAULTS="$DEFAULTS;sdkconfig.verbose" ;; esac
case "$ENV" in *-csiref) DEFAULTS="$DEFAULTS;sdkconfig.csiref" ;; esac
[ $# -eq 0 ] && set -- build
# The env var wins over any IDF_TARGET a CI image pre-set (esp-idf-ci-action defaults to esp32).
export IDF_TARGET="$TARGET"
# Component requirements are also expanded in an IDF subprocess where CMake
# cache variables are unavailable; carry this explicit variant gate through it.
export ESPRESENSE_CSI_REFERENCE=0
case "$ENV" in *-csiref) export ESPRESENSE_CSI_REFERENCE=1 ;; esac
exec idf.py -B "build/$ENV" -DSDKCONFIG="build/$ENV/sdkconfig" \
  -DSDKCONFIG_DEFAULTS="$DEFAULTS" -DFW_ENV="$ENV" \
  ${FW_VERSION:+-DFW_VERSION="$FW_VERSION"} ${FW_BRANCH:+-DFW_BRANCH="$FW_BRANCH"} \
  ${FW_RUN_ID:+-DFW_RUN_ID="$FW_RUN_ID"} "$@"
