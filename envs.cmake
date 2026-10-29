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
  list(APPEND FW_DEFS ESP32S3 USE_ETHERNET)
else()
  list(APPEND FW_DEFS USE_ETHERNET)
endif()

# VERSION / BRANCH / RUN_ID come from CI: -DFW_VERSION=v4.1.0 -DFW_BRANCH=main -DFW_RUN_ID=40676895203
if(FW_VERSION)
  list(APPEND FW_DEFS "VERSION=\"${FW_VERSION}\"")
endif()
if(FW_BRANCH)
  list(APPEND FW_DEFS "BRANCH=\"${FW_BRANCH}\"")
endif()
if(FW_RUN_ID)
  list(APPEND FW_DEFS "RUN_ID=\"${FW_RUN_ID}\"")
endif()
