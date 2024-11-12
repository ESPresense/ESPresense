#pragma once

#include "esp_err.h"
#include "driver/i2c.h"
#include "driver/gpio.h"
#include "bme280.h"
#include "bh1750.h"
#include "dht.h"

class SensorManager {
public:
    static esp_err_t init(i2c_port_t i2c_port = I2C_NUM_0, 
                         gpio_num_t sda = GPIO_NUM_21,
                         gpio_num_t scl = GPIO_NUM_22,
                         uint32_t freq = 100000);
                         
    // Sensor initialization methods
    static esp_err_t init_bme280(uint8_t addr = 0x76);
    static esp_err_t init_bh1750(uint8_t addr = 0x23);
    static esp_err_t init_dht(gpio_num_t gpio_num, DHT::Type type = DHT::DHT22);
    
    // Sensor reading methods
    static esp_err_t read_temperature(float* temp, const char* sensor = "bme280");
    static esp_err_t read_humidity(float* humidity, const char* sensor = "bme280");
    static esp_err_t read_pressure(float* pressure);
    static esp_err_t read_light(float* lux);
    
private:
    static i2c_port_t i2c_num;
    static bool bme280_initialized;
    static bool bh1750_initialized;
    static bool dht_initialized;
    
    static esp_err_t init_i2c(gpio_num_t sda, gpio_num_t scl, uint32_t freq);
};
