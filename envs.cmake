# One row per firmware variant: FW_ENV -> compile definitions.
# The chip target and sdkconfig fragments for each variant live in build.sh.
set(FW_DEFS "FIRMWARE=\"${FW_ENV}\"" SENSORS)

if(FW_ENV MATCHES "-verbose$")
  list(APPEND FW_DEFS LOG_LEVEL_DEBUG)
endif()

if(FW_ENV MATCHES "^esp32c3")
  list(APPEND FW_DEFS ESP32C3)
elseif(FW_ENV MATCHES "^esp32c6")
  list(APPEND FW_DEFS ESP32C6)
elseif(FW_ENV MATCHES "^esp32s3")
  list(APPEND FW_DEFS ESP32S3)
else()
  list(APPEND FW_DEFS USE_ETHERNET)
endif()

if(FW_ENV STREQUAL "m5stickc")
  list(APPEND FW_DEFS M5STICK TFT_FONT=1)
elseif(FW_ENV STREQUAL "m5stickc-plus")
  list(APPEND FW_DEFS M5STICK PLUS TFT_FONT=2)
elseif(FW_ENV STREQUAL "m5atom")
  list(APPEND FW_DEFS M5ATOM MATRIX)
elseif(FW_ENV STREQUAL "macchina-a0")
  list(APPEND FW_DEFS MACCHINA_A0)
endif()

# VERSION / BRANCH come from CI: -DFW_VERSION=v4.1.0 -DFW_BRANCH=main
if(FW_VERSION)
  list(APPEND FW_DEFS "VERSION=\"${FW_VERSION}\"")
endif()
if(FW_BRANCH)
  list(APPEND FW_DEFS "BRANCH=\"${FW_BRANCH}\"")
endif()
