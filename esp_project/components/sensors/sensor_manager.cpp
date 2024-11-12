#include "sensor_manager.h"
#include "esp_log.h"

static const char* TAG = "SensorManager";

// Static member initialization
i2c_port_t SensorManager::i2c_num = I2C_NUM_0;
bool SensorManager::bme280_initialized = false;
bool SensorManager::bh1750_initialized = false;
bool SensorManager::dht_initialized = false;

esp_err_t SensorManager::init(i2c_port_t i2c_port, gpio_num_t sda, gpio_num_t scl, uint32_t freq) {
    i2c_num = i2c_port;
    return init_i2c(sda, scl, freq);
}

esp_err_t SensorManager::init_i2c(gpio_num_t sda, gpio_num_t scl, uint32_t freq) {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = sda,
        .scl_io_num = scl,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master = {
            .clk_speed = freq
        }
    };
    
    esp_err_t ret = i2c_param_config(i2c_num, &conf);
    if (ret != ESP_OK) return ret;
    
    return i2c_driver_install(i2c_num, conf.mode, 0, 0, 0);
}

esp_err_t SensorManager::init_bme280(uint8_t addr) {
    esp_err_t ret = BME280::init(i2c_num, addr);
    if (ret == ESP_OK) {
        bme280_initialized = true;
        ESP_LOGI(TAG, "BME280 initialized successfully");
    }
    return ret;
}

esp_err_t SensorManager::init_bh1750(uint8_t addr) {
    esp_err_t ret = BH1750::init(i2c_num, addr);
    if (ret == ESP_OK) {
        bh1750_initialized = true;
        ESP_LOGI(TAG, "BH1750 initialized successfully");
    }
    return ret;
}

esp_err_t SensorManager::init_dht(gpio_num_t gpio_num, DHT::Type type) {
    esp_err_t ret = DHT::init(gpio_num, type);
    if (ret == ESP_OK) {
        dht_initialized = true;
        ESP_LOGI(TAG, "DHT sensor initialized successfully");
    }
    return ret;
}

esp_err_t SensorManager::read_temperature(float* temp, const char* sensor) {
    if (strcmp(sensor, "bme280") == 0) {
        if (!bme280_initialized) {
            ESP_LOGE(TAG, "BME280 not initialized");
            return ESP_ERR_INVALID_STATE;
        }
        return BME280::read_temperature(temp);
    } else if (strcmp(sensor, "dht") == 0) {
        if (!dht_initialized) {
            ESP_LOGE(TAG, "DHT not initialized");
            return ESP_ERR_INVALID_STATE;
        }
        return DHT::read_temperature(temp);
    }
    
    ESP_LOGE(TAG, "Unknown sensor type for temperature reading");
    return ESP_ERR_INVALID_ARG;
}

esp_err_t SensorManager::read_humidity(float* humidity, const char* sensor) {
    if (strcmp(sensor, "bme280") == 0) {
        if (!bme280_initialized) {
            ESP_LOGE(TAG, "BME280 not initialized");
            return ESP_ERR_INVALID_STATE;
        }
        return BME280::read_humidity(humidity);
    } else if (strcmp(sensor, "dht") == 0) {
        if (!dht_initialized) {
            ESP_LOGE(TAG, "DHT not initialized");
            return ESP_ERR_INVALID_STATE;
        }
        return DHT::read_humidity(humidity);
    }
    
    ESP_LOGE(TAG, "Unknown sensor type for humidity reading");
    return ESP_ERR_INVALID_ARG;
}

esp_err_t SensorManager::read_pressure(float* pressure) {
    if (!bme280_initialized) {
        ESP_LOGE(TAG, "BME280 not initialized");
        return ESP_ERR_INVALID_STATE;
    }
    return BME280::read_pressure(pressure);
}

esp_err_t SensorManager::read_light(float* lux) {
    if (!bh1750_initialized) {
        ESP_LOGE(TAG, "BH1750 not initialized");
        return ESP_ERR_INVALID_STATE;
    }
    return BH1750::read_light(lux);
}
