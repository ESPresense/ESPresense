file(GLOB_RECURSE APP_SRCS *.cpp)
idf_component_register(SRCS ${APP_SRCS}
                       INCLUDE_DIRS "." "led"
                       REQUIRES driver esp_wifi esp_netif esp_event nvs_flash spiffs esp_http_server
                                esp_http_client esp_https_ota app_update mqtt esp_timer bt mbedtls
                                lwip esp_adc esp_eth vfs utils filtering)
target_compile_definitions(${COMPONENT_LIB} PUBLIC ${FW_DEFS})
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-missing-field-initializers -Wno-unused-parameter)
# Build timestamp for the /json "ver" field of unversioned builds.
string(TIMESTAMP BUILD_TS "%Y-%m-%dT%H:%M:%S")
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/build_timestamp_value.h "#define BUILD_TIMESTAMP \"${BUILD_TS}\"\n")
target_include_directories(${COMPONENT_LIB} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
