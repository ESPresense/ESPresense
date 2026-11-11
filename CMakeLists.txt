cmake_minimum_required(VERSION 3.16)

# FW_ENV picks the firmware variant (esp32, esp32c3-cdc, esp32s3, ...). build.sh maps it to
# the chip target and sdkconfig files; envs.cmake maps it to compile definitions.
set(FW_ENV "esp32" CACHE STRING "Firmware variant")
include(${CMAKE_CURRENT_LIST_DIR}/envs.cmake)

# Reference experiment only. Normal builds retain their IDF version and dependencies.
if(FW_ENV MATCHES "-csiref$")
    set(ESPECTRE_REFERENCE_DIR "${CMAKE_CURRENT_LIST_DIR}/.reference/espectre")
    if(NOT EXISTS "${ESPECTRE_REFERENCE_DIR}/CMakeLists.txt")
        message(FATAL_ERROR "Run python3 tools/prepare_csi_reference.py first")
    endif()
    list(APPEND EXTRA_COMPONENT_DIRS "${ESPECTRE_REFERENCE_DIR}")
endif()

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(espresense)
