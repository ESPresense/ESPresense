#pragma once

#include <string>
#include <vector>
#include "esp_gap_ble_api.h"
#include "esp_bt_defs.h"

class BleFingerprint {
public:
    static void init();
    static void start_scan();
    static void stop_scan();
    
    // Configuration
    static float maxDistance;
    static float absorption;
    static int8_t txRefRssi;
    static int8_t rxAdjRssi;
    
    // Filters
    static std::string query;
    static std::string include;
    static std::string exclude;
    static std::string knownMacs;
    static std::string knownIrks;
    static std::string countIds;

private:
    static void gap_callback(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t* param);
    static void process_advertisement(esp_ble_gap_cb_param_t* scan_result);
    static float calculate_distance(int rssi);
};
