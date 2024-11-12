#include "main.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"
#include "esp_mac.h"
#include "cJSON.h"
#include "sensor_manager.h"

static const char *TAG = "ESPresense";

// Global variables initialization
TimerHandle_t reconnectTimer = nullptr;
TaskHandle_t scanTaskHandle = nullptr;
unsigned long updateStartedMillis = 0;
unsigned long lastTeleMillis = 0;
int reconnectTries = 0;
int teleFails = 0;
int reportFailed = 0;
bool online = false;
bool sentDiscovery = false;
UBaseType_t bleStack = 0;

// Configuration
std::string room;
std::string mqttHost = DEFAULT_MQTT_HOST;
std::string mqttUser = DEFAULT_MQTT_USER;
std::string mqttPass = DEFAULT_MQTT_PASSWORD;
uint16_t mqttPort = DEFAULT_MQTT_PORT;

// Settings
bool discovery = true;
bool publishTele = true;
bool publishRooms = false;
bool publishDevices = true;

static void scan_task(void* arg) {
    while (true) {
        BleFingerprint::start_scan();
        vTaskDelay(pdMS_TO_TICKS(100)); // Small delay between scans
    }
}

static void initialize_nvs(void)
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);
}

static void initialize_wifi(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(example_connect());
}

static void load_config(void)
{
    nvs_handle_t nvs;
    esp_err_t err = nvs_open("config", NVS_READWRITE, &nvs);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error opening NVS handle");
        return;
    }

    // Read room name or generate default
    size_t required_size;
    err = nvs_get_str(nvs, "room", nullptr, &required_size);
    if (err == ESP_OK) {
        char* room_buf = new char[required_size];
        nvs_get_str(nvs, "room", room_buf, &required_size);
        room = room_buf;
        delete[] room_buf;
    } else {
        uint8_t mac[6];
        esp_read_mac(mac, ESP_MAC_WIFI_STA);
        char room_buf[18];
        snprintf(room_buf, sizeof(room_buf), "esp-%02x%02x%02x", mac[3], mac[4], mac[5]);
        room = room_buf;
    }

    nvs_close(nvs);
}

static void initialize_sensors(void)
{
    esp_err_t ret = SensorManager::init(I2C_NUM_0, GPIO_NUM_21, GPIO_NUM_22);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize I2C");
        return;
    }

    // Initialize BME280 if available
    ret = SensorManager::init_bme280();
    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "BME280 not found");
    }

    // Initialize BH1750 if available
    ret = SensorManager::init_bh1750();
    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "BH1750 not found");
    }

    // Initialize DHT22 if available
    ret = SensorManager::init_dht(GPIO_NUM_23, DHT::DHT22);
    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "DHT22 not found");
    }
}

void app_main(void)
{
    ESP_LOGI(TAG, "ESPresense starting...");
    
    // Initialize NVS
    initialize_nvs();
    
    // Load configuration
    load_config();
    
    // Initialize WiFi
    initialize_wifi();
    
    // Initialize sensors
    initialize_sensors();
    
    // Initialize BLE
    esp_err_t ret = esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT);
    if (ret) {
        ESP_LOGE(TAG, "Bluetooth memory release failed: %s", esp_err_to_name(ret));
        return;
    }
    
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ret = esp_bt_controller_init(&bt_cfg);
    if (ret) {
        ESP_LOGE(TAG, "Initialize controller failed: %s", esp_err_to_name(ret));
        return;
    }

    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (ret) {
        ESP_LOGE(TAG, "Enable controller failed: %s", esp_err_to_name(ret));
        return;
    }

    ret = esp_bluedroid_init();
    if (ret) {
        ESP_LOGE(TAG, "Init bluedroid failed: %s", esp_err_to_name(ret));
        return;
    }

    ret = esp_bluedroid_enable();
    if (ret) {
        ESP_LOGE(TAG, "Enable bluedroid failed: %s", esp_err_to_name(ret));
        return;
    }

    // Initialize BLE scanning
    BleFingerprint::init();
    
    // Create BLE scan task
    xTaskCreate(scan_task, "scan_task", 4096, nullptr, 5, &scanTaskHandle);
    
    // Initialize MQTT
    MqttHandler::init(mqttHost, mqttPort, mqttUser, mqttPass);
}

bool sendTelemetry(unsigned int totalSeen, unsigned int totalFpSeen, 
                  unsigned int totalFpQueried, unsigned int totalFpReported, 
                  unsigned int count) 
{
    if (!MqttHandler::is_connected()) {
        ESP_LOGW(TAG, "MQTT not connected");
        return false;
    }

    uint64_t now = esp_timer_get_time() / 1000; // Convert to milliseconds
    
    if (now - lastTeleMillis < 15000) {
        return false;
    }
    
    lastTeleMillis = now;
    
    // Create JSON document using cJSON
    cJSON *doc = cJSON_CreateObject();
    
    // Add telemetry data
    char ip[16];
    esp_netif_ip_info_t ip_info;
    esp_netif_get_ip_info(esp_netif_get_handle_from_ifkey("WIFI_STA_DEF"), &ip_info);
    sprintf(ip, IPSTR, IP2STR(&ip_info.ip));
    cJSON_AddStringToObject(doc, "ip", ip);
    
    cJSON_AddNumberToObject(doc, "uptime", esp_timer_get_time() / 1000000);
    
    int8_t rssi;
    esp_wifi_get_rssi(&rssi);
    cJSON_AddNumberToObject(doc, "rssi", rssi);
    
    // Add sensor data if available
    float value;
    if (SensorManager::read_temperature(&value, "bme280") == ESP_OK) {
        cJSON_AddNumberToObject(doc, "temperature", value);
    }
    if (SensorManager::read_humidity(&value, "bme280") == ESP_OK) {
        cJSON_AddNumberToObject(doc, "humidity", value);
    }
    if (SensorManager::read_pressure(&value) == ESP_OK) {
        cJSON_AddNumberToObject(doc, "pressure", value);
    }
    if (SensorManager::read_light(&value) == ESP_OK) {
        cJSON_AddNumberToObject(doc, "light", value);
    }
    
    if (count > 0) {
        cJSON_AddNumberToObject(doc, "count", count);
    }
    if (totalSeen > 0) {
        cJSON_AddNumberToObject(doc, "adverts", totalSeen);
    }
    
    char *json_str = cJSON_Print(doc);
    bool success = MqttHandler::send_telemetry(json_str);
    
    cJSON_Delete(doc);
    free(json_str);
    
    return success;
}

