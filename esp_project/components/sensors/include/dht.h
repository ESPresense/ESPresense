#pragma once

#include <stdint.h>
#include "esp_err.h"
#include "driver/gpio.h"

class DHT {
public:
    enum Type {
        DHT11 = 11,
        DHT22 = 22
    };

    static esp_err_t init(gpio_num_t gpio_num, Type type = DHT22);
    static esp_err_t read_temperature(float* temperature);
    static esp_err_t read_humidity(float* humidity);
    
private:
    static gpio_num_t gpio_pin;
    static Type sensor_type;
    
    static esp_err_t read_data(uint8_t* data);
    static void delay_us(uint32_t us);
    static int wait_state_change(int level, int timeout_us);
};
