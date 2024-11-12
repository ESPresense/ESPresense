#include "ble_fingerprint.h"
#include "esp_log.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gatt_defs.h"
#include <cmath>

static const char* TAG = "BLE_FINGER";

// Static member initialization
float BleFingerprint::maxDistance = 10.0f;
float BleFingerprint::absorption = 2.0f;
int8_t BleFingerprint::txRefRssi = -59;
int8_t BleFingerprint::rxAdjRssi = 0;
std::string BleFingerprint::query;
std::string BleFingerprint::include;
std::string BleFingerprint::exclude;
std::string BleFingerprint::knownMacs;
std::string BleFingerprint::knownIrks;
std::string BleFingerprint::countIds;

void BleFingerprint::init() {
    // Configure scan parameters
    esp_ble_scan_params_t scan_params = {
        .scan_type = BLE_SCAN_TYPE_ACTIVE,
        .own_addr_type = BLE_ADDR_TYPE_PUBLIC,
        .scan_filter_policy = BLE_SCAN_FILTER_ALLOW_ALL,
        .scan_interval = 0x50,
        .scan_window = 0x30,
        .scan_duplicate = BLE_SCAN_DUPLICATE_DISABLE
    };
    
    esp_err_t status = esp_ble_gap_set_scan_params(&scan_params);
    if (status != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set scan parameters: %s", esp_err_to_name(status));
        return;
    }

    // Register GAP callback
    esp_ble_gap_register_callback(gap_callback);
}

void BleFingerprint::start_scan() {
    esp_err_t status = esp_ble_gap_start_scanning(0); // Scan indefinitely
    if (status != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start scanning: %s", esp_err_to_name(status));
    }
}

void BleFingerprint::stop_scan() {
    esp_err_t status = esp_ble_gap_stop_scanning();
    if (status != ESP_OK) {
        ESP_LOGE(TAG, "Failed to stop scanning: %s", esp_err_to_name(status));
    }
}

void BleFingerprint::gap_callback(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t* param) {
    switch (event) {
        case ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT:
            ESP_LOGI(TAG, "Scan parameters set");
            break;
            
        case ESP_GAP_BLE_SCAN_START_COMPLETE_EVT:
            if (param->scan_start_cmpl.status != ESP_BT_STATUS_SUCCESS) {
                ESP_LOGE(TAG, "Scan start failed");
            }
            break;
            
        case ESP_GAP_BLE_SCAN_RESULT_EVT:
            if (param->scan_rst.search_evt == ESP_GAP_SEARCH_INQ_RES_EVT) {
                process_advertisement(param);
            }
            break;
            
        default:
            break;
    }
}

void BleFingerprint::process_advertisement(esp_ble_gap_cb_param_t* scan_result) {
    // Calculate distance based on RSSI
    float distance = calculate_distance(scan_result->scan_rst.rssi);
    
    if (distance > maxDistance) {
        return; // Device too far away
    }
    
    // Convert BDA to string for MAC address
    char bda_str[18];
    sprintf(bda_str, "%02x:%02x:%02x:%02x:%02x:%02x",
            scan_result->scan_rst.bda[0], scan_result->scan_rst.bda[1],
            scan_result->scan_rst.bda[2], scan_result->scan_rst.bda[3],
            scan_result->scan_rst.bda[4], scan_result->scan_rst.bda[5]);
            
    ESP_LOGI(TAG, "Device found - MAC: %s, RSSI: %d, Distance: %.2f",
             bda_str, scan_result->scan_rst.rssi, distance);
             
    // TODO: Implement filtering based on query, include, exclude patterns
    // TODO: Implement device tracking and MQTT reporting
}

float BleFingerprint::calculate_distance(int rssi) {
    // Calculate distance using the log-distance path loss model
    float ratio = (txRefRssi - (rssi + rxAdjRssi)) / (10.0 * absorption);
    return pow(10, ratio);
}
