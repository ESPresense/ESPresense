cmake_minimum_required(VERSION 3.16)

# FW_ENV picks the firmware variant (esp32, esp32c3-cdc, m5stickc, ...). build.sh maps it to
# the chip target and sdkconfig files; envs.cmake maps it to compile definitions.
set(FW_ENV "esp32" CACHE STRING "Firmware variant")
include(${CMAKE_CURRENT_LIST_DIR}/envs.cmake)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(espresense)
