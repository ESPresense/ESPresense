#include "bme280.h"
#include "esp_log.h"

static const char* TAG = "BME280";

// Static member initialization
i2c_port_t BME280::i2c_port = I2C_NUM_0;
uint8_t BME280::device_addr = 0x76;

// Calibration data initialization
uint16_t BME280::dig_T1 = 0;
int16_t  BME280::dig_T2 = 0;
int16_t  BME280::dig_T3 = 0;
uint16_t BME280::dig_P1 = 0;
int16_t  BME280::dig_P2 = 0;
int16_t  BME280::dig_P3 = 0;
int16_t  BME280::dig_P4 = 0;
int16_t  BME280::dig_P5 = 0;
int16_t  BME280::dig_P6 = 0;
int16_t  BME280::dig_P7 = 0;
int16_t  BME280::dig_P8 = 0;
int16_t  BME280::dig_P9 = 0;
uint8_t  BME280::dig_H1 = 0;
int16_t  BME280::dig_H2 = 0;
uint8_t  BME280::dig_H3 = 0;
int16_t  BME280::dig_H4 = 0;
int16_t  BME280::dig_H5 = 0;
int8_t   BME280::dig_H6 = 0;

esp_err_t BME280::init(i2c_port_t i2c_num, uint8_t addr) {
    i2c_port = i2c_num;
    device_addr = addr;

    // Read chip ID
    uint8_t chip_id;
    esp_err_t ret = read_registers(0xD0, &chip_id, 1);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read chip ID");
        return ret;
    }

    if (chip_id != 0x60) {
        ESP_LOGE(TAG, "Unexpected chip ID: 0x%02x", chip_id);
        return ESP_ERR_INVALID_RESPONSE;
    }

    // Read calibration data
    ret = read_calibration_data();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read calibration data");
        return ret;
    }

    // Configure sensor
    // Normal mode, temperature and pressure oversampling x1
    write_register(0xF4, 0x27);
    // Humidity oversampling x1
    write_register(0xF2, 0x01);

    return ESP_OK;
}

esp_err_t BME280::read_registers(uint8_t reg, uint8_t* data, size_t len) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (device_addr << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg, true);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (device_addr << 1) | I2C_MASTER_READ, true);
    if (len > 1) {
        i2c_master_read(cmd, data, len - 1, I2C_MASTER_ACK);
    }
    i2c_master_read_byte(cmd, data + len - 1, I2C_MASTER_NACK);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(i2c_port, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

esp_err_t BME280::write_register(uint8_t reg, uint8_t value) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (device_addr << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg, true);
    i2c_master_write_byte(cmd, value, true);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(i2c_port, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

esp_err_t BME280::read_calibration_data() {
    uint8_t buffer[26];
    esp_err_t ret;

    // Read temperature and pressure calibration data
    ret = read_registers(0x88, buffer, 26);
    if (ret != ESP_OK) return ret;

    dig_T1 = (buffer[1] << 8) | buffer[0];
    dig_T2 = (buffer[3] << 8) | buffer[2];
    dig_T3 = (buffer[5] << 8) | buffer[4];
    dig_P1 = (buffer[7] << 8) | buffer[6];
    dig_P2 = (buffer[9] << 8) | buffer[8];
    dig_P3 = (buffer[11] << 8) | buffer[10];
    dig_P4 = (buffer[13] << 8) | buffer[12];
    dig_P5 = (buffer[15] << 8) | buffer[14];
    dig_P6 = (buffer[17] << 8) | buffer[16];
    dig_P7 = (buffer[19] << 8) | buffer[18];
    dig_P8 = (buffer[21] << 8) | buffer[20];
    dig_P9 = (buffer[23] << 8) | buffer[22];
    dig_H1 = buffer[25];

    // Read humidity calibration data
    ret = read_registers(0xE1, buffer, 7);
    if (ret != ESP_OK) return ret;

    dig_H2 = (buffer[1] << 8) | buffer[0];
    dig_H3 = buffer[2];
    dig_H4 = (buffer[3] << 4) | (buffer[4] & 0x0F);
    dig_H5 = (buffer[5] << 4) | (buffer[4] >> 4);
    dig_H6 = buffer[6];

    return ESP_OK;
}

esp_err_t BME280::read_temperature(float* temp) {
    uint8_t data[3];
    esp_err_t ret = read_registers(0xFA, data, 3);
    if (ret != ESP_OK) return ret;

    int32_t adc_T = (data[0] << 16) | (data[1] << 8) | data[2];
    adc_T >>= 4;

    int32_t var1 = ((((adc_T >> 3) - ((int32_t)dig_T1 << 1))) * ((int32_t)dig_T2)) >> 11;
    int32_t var2 = (((((adc_T >> 4) - ((int32_t)dig_T1)) * ((adc_T >> 4) - ((int32_t)dig_T1))) >> 12) * ((int32_t)dig_T3)) >> 14;
    
    int32_t t_fine = var1 + var2;
    *temp = (t_fine * 5 + 128) >> 8;
    *temp /= 100.0f;

    return ESP_OK;
}

esp_err_t BME280::read_pressure(float* pressure) {
    uint8_t data[3];
    esp_err_t ret = read_registers(0xF7, data, 3);
    if (ret != ESP_OK) return ret;

    int32_t adc_P = (data[0] << 16) | (data[1] << 8) | data[2];
    adc_P >>= 4;

    // Pressure compensation calculation
    int64_t var1, var2, p;
    var1 = ((int64_t)t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)dig_P6;
    var2 = var2 + ((var1 * (int64_t)dig_P5) << 17);
    var2 = var2 + (((int64_t)dig_P4) << 35);
    var1 = ((var1 * var1 * (int64_t)dig_P3) >> 8) + ((var1 * (int64_t)dig_P2) << 12);
    var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)dig_P1) >> 33;
    if (var1 == 0) return ESP_FAIL;

    p = 1048576 - adc_P;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = (((int64_t)dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t)dig_P8) * p) >> 19;
    p = ((p + var1 + var2) >> 8) + (((int64_t)dig_P7) << 4);

    *pressure = (float)p / 256.0f;
    *pressure /= 100.0f; // Convert to hPa

    return ESP_OK;
}

esp_err_t BME280::read_humidity(float* humidity) {
    uint8_t data[2];
    esp_err_t ret = read_registers(0xFD, data, 2);
    if (ret != ESP_OK) return ret;

    int32_t adc_H = (data[0] << 8) | data[1];

    int32_t v_x1_u32r = (t_fine - ((int32_t)76800));
    v_x1_u32r = (((((adc_H << 14) - (((int32_t)dig_H4) << 20) - (((int32_t)dig_H5) * v_x1_u32r)) +
                   ((int32_t)16384)) >> 15) * (((((((v_x1_u32r * ((int32_t)dig_H6)) >> 10) *
                   (((v_x1_u32r * ((int32_t)dig_H3)) >> 11) + ((int32_t)32768))) >> 10) +
                   ((int32_t)2097152)) * ((int32_t)dig_H2) + 8192) >> 14));
    v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * ((int32_t)dig_H1)) >> 4));
    v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
    v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);

    *humidity = (float)(v_x1_u32r >> 12) / 1024.0f;

    return ESP_OK;
}
