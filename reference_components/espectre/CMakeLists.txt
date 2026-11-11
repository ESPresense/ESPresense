# esp_wifi's PRIVATE esp_coex dependency does not export its header to the SDK.
# Configure the SDK's public hook here, including during IDF's requirements
# subprocess where parent CMake cache variables are unavailable. No SDK edits.
set(ESPECTRE_SDK_ADDITIONAL_REQUIRES "")
if(NOT "$ENV{CSI_REFERENCE_COEX_DEPENDENCY}" STREQUAL "0")
    list(APPEND ESPECTRE_SDK_ADDITIONAL_REQUIRES esp_coex)
endif()
include("${CMAKE_CURRENT_LIST_DIR}/../../.reference/espectre/CMakeLists.txt")
