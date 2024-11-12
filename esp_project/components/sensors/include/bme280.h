#pragma once

#include <stdint.h>
#include "esp_err.h"
#include "driver/i2c.h"

class BME280 {
public:
    static esp_err_t init(i2c_port_t i2c_num, uint8_t addr = 0x76);
    static esp_err_t read_temperature(float* temp);
    static esp_err_t read_humidity(float* humidity);
    static esp_err_t read_pressure(float* pressure);
    
private:
    static i2c_port_t i2c_port;
    static uint8_t device_addr;
    
    // Calibration data
    static uint16_t dig_T1;
    static int16_t  dig_T2;
    static int16_t  dig_T3;
    static uint16_t dig_P1;
    static int16_t  dig_P2;
    static int16_t  dig_P3;
    static int16_t  dig_P4;
    static int16_t  dig_P5;
    static int16_t  dig_P6;
    static int16_t  dig_P7;
    static int16_t  dig_P8;
    static int16_t  dig_P9;
    static uint8_t  dig_H1;
    static int16_t  dig_H2;
    static uint8_t  dig_H3;
    static int16_t  dig_H4;
    static int16_t  dig_H5;
    static int8_t   dig_H6;
    
    static esp_err_t read_calibration_data();
    static esp_err_t read_registers(uint8_t reg, uint8_t* data, size_t len);
    static esp_err_t write_register(uint8_t reg, uint8_t value);
};
