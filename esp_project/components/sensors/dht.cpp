#include "dht.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

static const char* TAG = "DHT";

// Static member initialization
gpio_num_t DHT::gpio_pin = GPIO_NUM_NC;
DHT::Type DHT::sensor_type = DHT::DHT22;

esp_err_t DHT::init(gpio_num_t gpio_num, Type type) {
    gpio_pin = gpio_num;
    sensor_type = type;

    // Configure GPIO
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << gpio_num),
        .mode = GPIO_MODE_INPUT_OUTPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    
    esp_err_t ret = gpio_config(&io_conf);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure GPIO");
        return ret;
    }

    // Initial state is high
    gpio_set_level(gpio_pin, 1);
    vTaskDelay(pdMS_TO_TICKS(1000)); // Wait for sensor to stabilize

    return ESP_OK;
}

void DHT::delay_us(uint32_t us) {
    uint32_t start = esp_timer_get_time();
    while (esp_timer_get_time() - start < us);
}

int DHT::wait_state_change(int level, int timeout_us) {
    int us_waited = 0;
    while (gpio_get_level(gpio_pin) == level) {
        if (us_waited > timeout_us) return -1;
        delay_us(1);
        us_waited++;
    }
    return us_waited;
}

esp_err_t DHT::read_data(uint8_t* data) {
    uint8_t bits[5] = {0};
    uint8_t cnt = 7;
    uint8_t idx = 0;

    // Clear data array
    for (int i = 0; i < 5; i++) data[i] = 0;

    // Send start signal
    gpio_set_direction(gpio_pin, GPIO_MODE_OUTPUT);
    gpio_set_level(gpio_pin, 0);
    delay_us(sensor_type == DHT11 ? 18000 : 1000); // Hold low for 18ms (DHT11) or 1ms (DHT22)
    gpio_set_level(gpio_pin, 1);
    delay_us(40);
    
    gpio_set_direction(gpio_pin, GPIO_MODE_INPUT);

    // Read sensor response
    if (wait_state_change(0, 80) == -1) return ESP_ERR_TIMEOUT;
    if (wait_state_change(1, 80) == -1) return ESP_ERR_TIMEOUT;

    // Read data
    for (int i = 0; i < 40; i++) {
        if (wait_state_change(0, 50) == -1) return ESP_ERR_TIMEOUT;
        int us = wait_state_change(1, 70);
        if (us == -1) return ESP_ERR_TIMEOUT;
        
        if (us > 40) {
            bits[idx] |= (1 << cnt);
        }
        if (cnt == 0) {
            cnt = 7;
            idx++;
        } else {
            cnt--;
        }
    }

    // Verify checksum
    if (bits[4] != ((bits[0] + bits[1] + bits[2] + bits[3]) & 0xFF)) {
        ESP_LOGE(TAG, "Checksum failed");
        return ESP_ERR_INVALID_CRC;
    }

    for (int i = 0; i < 5; i++) data[i] = bits[i];
    return ESP_OK;
}

esp_err_t DHT::read_temperature(float* temperature) {
    uint8_t data[5];
    esp_err_t ret = read_data(data);
    if (ret != ESP_OK) return ret;

    if (sensor_type == DHT11) {
        *temperature = data[2];
        if (data[3] & 0x80) {
            *temperature = -(*temperature);
        }
    } else {
        *temperature = ((data[2] & 0x7F) << 8) + data[3];
        *temperature *= 0.1;
        if (data[2] & 0x80) {
            *temperature = -(*temperature);
        }
    }

    return ESP_OK;
}

esp_err_t DHT::read_humidity(float* humidity) {
    uint8_t data[5];
    esp_err_t ret = read_data(data);
    if (ret != ESP_OK) return ret;

    if (sensor_type == DHT11) {
        *humidity = data[0];
    } else {
        *humidity = (data[0] << 8) + data[1];
        *humidity *= 0.1;
    }

    return ESP_OK;
}
