#pragma once

#include <stdint.h>
#include "esp_err.h"
#include "driver/i2c.h"

class BH1750 {
public:
    enum Mode {
        CONTINUOUS_HIGH_RES_MODE  = 0x10,
        CONTINUOUS_LOW_RES_MODE   = 0x13,
        ONE_TIME_HIGH_RES_MODE   = 0x20,
        ONE_TIME_LOW_RES_MODE    = 0x23
    };

    static esp_err_t init(i2c_port_t i2c_num, uint8_t addr = 0x23);
    static esp_err_t read_light(float* lux);
    static esp_err_t set_mode(Mode mode);
    
private:
    static i2c_port_t i2c_port;
    static uint8_t device_addr;
    static Mode current_mode;
    
    static esp_err_t write_command(uint8_t cmd);
};
