#ifdef SENSORS

#include "I2C.h"

#include <vector>

#include "Settings.h"
#include "defaults.h"
#include "driver/i2c_master.h"
#include "globals.h"
#include "soc/soc_caps.h"

namespace I2C {
static const int TIMEOUT_MS = 100;
static const uint32_t CLOCK_HZ = 100000;

bool I2CDebug = false;
int I2C_Bus_1_SDA = 0;
int I2C_Bus_1_SCL = 0;
int I2C_Bus_2_SDA = 0;
int I2C_Bus_2_SCL = 0;

static i2c_master_bus_handle_t buses[2] = {nullptr, nullptr};

struct Dev {
    int bus;
    uint8_t addr;
    i2c_master_dev_handle_t handle;
};
static std::vector<Dev> devs;  // one cached device handle per (bus, addr)

static i2c_master_bus_handle_t busHandle(int bus) {
    return (bus == 1 || bus == 2) ? buses[bus - 1] : nullptr;
}

static i2c_master_dev_handle_t dev(int bus, uint8_t addr) {
    auto bh = busHandle(bus);
    if (!bh) return nullptr;
    for (auto& d : devs)
        if (d.bus == bus && d.addr == addr) return d.handle;
    i2c_device_config_t cfg = {};
    cfg.dev_addr_length = I2C_ADDR_BIT_LEN_7;
    cfg.device_address = addr;
    cfg.scl_speed_hz = CLOCK_HZ;
    i2c_master_dev_handle_t h = nullptr;
    if (i2c_master_bus_add_device(bh, &cfg, &h) != ESP_OK) return nullptr;
    devs.push_back({bus, addr, h});
    return h;
}

static bool startBus(int idx, int port, int sda, int scl) {
    i2c_master_bus_config_t cfg = {};
    cfg.i2c_port = port;
    cfg.sda_io_num = (gpio_num_t)sda;
    cfg.scl_io_num = (gpio_num_t)scl;
    cfg.clk_source = I2C_CLK_SRC_DEFAULT;
    cfg.glitch_ignore_cnt = 7;
    cfg.flags.enable_internal_pullup = 1;
    esp_err_t err = i2c_new_master_bus(&cfg, &buses[idx]);
    if (err != ESP_OK) {
        buses[idx] = nullptr;
        log_e("I2C bus %d init failed: %s", idx + 1, esp_err_to_name(err));
        return false;
    }
    return true;
}

void ConnectToWifi(bool updating) {
    I2C_Bus_1_SDA = Settings::integer("I2C_Bus_1_SDA", -1, 48, DEFAULT_I2C_BUS_1_SDA, "SDA pin (-1 to disable)");
    I2C_Bus_1_SCL = Settings::integer("I2C_Bus_1_SCL", -1, 48, DEFAULT_I2C_BUS_1_SCL, "SCL pin (-1 to disable)");

    I2C_Bus_2_SDA = Settings::integer("I2C_Bus_2_SDA", -1, 48, DEFAULT_I2C_BUS_2_SDA, "SDA pin (-1 to disable)");
    I2C_Bus_2_SCL = Settings::integer("I2C_Bus_2_SCL", -1, 48, DEFAULT_I2C_BUS_2_SCL, "SCL pin (-1 to disable)");

    I2CDebug = Settings::checkbox("I2CDebug", false, "Debug I2C addreses. Look at the serial log to get the correct address");

    if (!I2C_Bus_1_Started && I2C_Bus_1_SDA != -1 && I2C_Bus_1_SCL != -1) {
        I2C_Bus_1_Started = startBus(0, I2C_NUM_0, I2C_Bus_1_SDA, I2C_Bus_1_SCL);
    }

#if HAS_I2C_BUS_2
    if (!I2C_Bus_2_Started && I2C_Bus_2_SDA != -1 && I2C_Bus_2_SCL != -1) {
        I2C_Bus_2_Started = startBus(1, I2C_NUM_1, I2C_Bus_2_SDA, I2C_Bus_2_SCL);
    }
#endif
}

void Setup() {
}

bool started(int bus) { return busHandle(bus) != nullptr; }

bool probe(int bus, uint8_t addr) {
    auto bh = busHandle(bus);
    return bh && i2c_master_probe(bh, addr, TIMEOUT_MS) == ESP_OK;
}

bool write(int bus, uint8_t addr, const uint8_t* data, size_t len) {
    auto h = dev(bus, addr);
    return h && i2c_master_transmit(h, data, len, TIMEOUT_MS) == ESP_OK;
}

bool read(int bus, uint8_t addr, uint8_t* data, size_t len) {
    auto h = dev(bus, addr);
    return h && i2c_master_receive(h, data, len, TIMEOUT_MS) == ESP_OK;
}

bool writeRead(int bus, uint8_t addr, const uint8_t* w, size_t wlen, uint8_t* r, size_t rlen) {
    auto h = dev(bus, addr);
    return h && i2c_master_transmit_receive(h, w, wlen, r, rlen, TIMEOUT_MS) == ESP_OK;
}

bool readReg(int bus, uint8_t addr, uint8_t reg, uint8_t* buf, size_t len) {
    return writeRead(bus, addr, &reg, 1, buf, len);
}

bool writeReg(int bus, uint8_t addr, uint8_t reg, uint8_t val) {
    uint8_t b[2] = {reg, val};
    return write(bus, addr, b, 2);
}

static int scan(int bus) {
    int n = 0;
    Log.printf("Scanning I2C for devices on Bus %d...\r\n", bus);
    for (uint8_t address = 1; address < 127; address++) {
        if (probe(bus, address)) {
            Log.printf("I2C device found on bus %d at address 0x%02X\r\n", bus, address);
            n++;
        }
    }
    return n;
}

void SerialReport() {
    if (I2C_Bus_1_Started)
        Log.printf("I2C Bus 1:    sda=%d scl=%d\r\n", I2C_Bus_1_SDA, I2C_Bus_1_SCL);
    if (I2C_Bus_2_Started)
        Log.printf("I2C Bus 2:    sda=%d scl=%d\r\n", I2C_Bus_2_SDA, I2C_Bus_2_SCL);

    if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
    if (!I2CDebug) return;

    int nDevices = 0;
    if (I2C_Bus_1_Started) nDevices += scan(1);
#if HAS_I2C_BUS_2
    if (I2C_Bus_2_Started) nDevices += scan(2);
#endif
    // ponytail: the new driver only reports ack/nack, so the Wire "Unknown error" branch is gone.
    if (nDevices == 0) {
        Log.println("No I2C devices found\r\n");
    }
}
}  // namespace I2C

#endif
