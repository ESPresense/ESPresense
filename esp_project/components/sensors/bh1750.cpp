#include "bh1750.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char* TAG = "BH1750";

// Static member initialization
i2c_port_t BH1750::i2c_port = I2C_NUM_0;
uint8_t BH1750::device_addr = 0x23;
BH1750::Mode BH1750::current_mode = BH1750::CONTINUOUS_HIGH_RES_MODE;

esp_err_t BH1750::init(i2c_port_t i2c_num, uint8_t addr) {
    i2c_port = i2c_num;
    device_addr = addr;

    // Power on the sensor
    esp_err_t ret = write_command(0x01);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to power on sensor");
        return ret;
    }

    // Set default mode
    ret = set_mode(CONTINUOUS_HIGH_RES_MODE);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set mode");
        return ret;
    }

    return ESP_OK;
}

esp_err_t BH1750::write_command(uint8_t cmd) {
    i2c_cmd_handle_t cmd_handle = i2c_cmd_link_create();
    i2c_master_start(cmd_handle);
    i2c_master_write_byte(cmd_handle, (device_addr << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd_handle, cmd, true);
    i2c_master_stop(cmd_handle);
    esp_err_t ret = i2c_master_cmd_begin(i2c_port, cmd_handle, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd_handle);
    return ret;
}

esp_err_t BH1750::set_mode(Mode mode) {
    esp_err_t ret = write_command(mode);
    if (ret == ESP_OK) {
        current_mode = mode;
        // Wait for measurement to be ready
        // High resolution mode needs 120ms, low resolution needs 16ms
        vTaskDelay(pdMS_TO_TICKS(mode == CONTINUOUS_HIGH_RES_MODE ? 120 : 16));
    }
    return ret;
}

esp_err_t BH1750::read_light(float* lux) {
    uint8_t data[2];
    
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (device_addr << 1) | I2C_MASTER_READ, true);
    i2c_master_read(cmd, data, 2, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(i2c_port, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read sensor data");
        return ret;
    }

    uint16_t level = (data[0] << 8) | data[1];
    *lux = level / 1.2f; // Convert to lux according to datasheet

    return ESP_OK;
}
