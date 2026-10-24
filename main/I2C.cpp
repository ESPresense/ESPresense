#ifdef SENSORS

#include "I2C.h"

#include <vector>

#include "Settings.h"
#include "defaults.h"
#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "esp_rom_sys.h"
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
int I2C_Bus_3_SDA = 0;
int I2C_Bus_3_SCL = 0;

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

// Bus 3 is bit-banged on any two pins, for boards with more I2C buses than the chip has controllers.
namespace soft {
static const int HALF_PERIOD_US = 5;     // ~100 kHz
static const int STRETCH_TIMEOUT_US = 5000;
static int sda = -1, scl = -1;
static bool up = false;

static void delay() { esp_rom_delay_us(HALF_PERIOD_US); }
static void sdaHigh() { gpio_set_level((gpio_num_t)sda, 1); }
static void sdaLow() { gpio_set_level((gpio_num_t)sda, 0); }
static void sclLow() { gpio_set_level((gpio_num_t)scl, 0); }
// Releasing SCL lets a slave stretch the clock; wait for it to actually go high.
static bool sclHigh() {
    gpio_set_level((gpio_num_t)scl, 1);
    for (int waited = 0; !gpio_get_level((gpio_num_t)scl); waited += 1) {
        if (waited > STRETCH_TIMEOUT_US) return false;
        esp_rom_delay_us(1);
    }
    return true;
}

static bool begin(int sdaPin, int sclPin) {
    sda = sdaPin;
    scl = sclPin;
    gpio_config_t cfg = {};
    cfg.pin_bit_mask = (1ULL << sda) | (1ULL << scl);
    cfg.mode = GPIO_MODE_INPUT_OUTPUT_OD;
    cfg.pull_up_en = GPIO_PULLUP_ENABLE;
    if (gpio_config(&cfg) != ESP_OK) return false;
    sdaHigh();
    sclHigh();
    up = true;
    return true;
}

static bool start() {
    sdaHigh();
    if (!sclHigh()) return false;
    delay();
    sdaLow();
    delay();
    sclLow();
    return true;
}

static void stop() {
    sdaLow();
    delay();
    sclHigh();
    delay();
    sdaHigh();
    delay();
}

// Returns true when the slave acknowledged.
static bool writeByte(uint8_t b) {
    for (int i = 7; i >= 0; i--) {
        gpio_set_level((gpio_num_t)sda, (b >> i) & 1);
        delay();
        if (!sclHigh()) return false;
        delay();
        sclLow();
    }
    sdaHigh();
    delay();
    if (!sclHigh()) return false;
    delay();
    bool ack = !gpio_get_level((gpio_num_t)sda);
    sclLow();
    return ack;
}

static uint8_t readByte(bool ack) {
    uint8_t b = 0;
    sdaHigh();
    for (int i = 0; i < 8; i++) {
        delay();
        sclHigh();
        delay();
        b = (b << 1) | gpio_get_level((gpio_num_t)sda);
        sclLow();
    }
    gpio_set_level((gpio_num_t)sda, ack ? 0 : 1);
    delay();
    sclHigh();
    delay();
    sclLow();
    sdaHigh();
    return b;
}

static bool probe(uint8_t addr) {
    bool ok = start() && writeByte(addr << 1);
    stop();
    return ok;
}

static bool sendBytes(uint8_t addr, const uint8_t* data, size_t len, bool withStop) {
    bool ok = start() && writeByte(addr << 1);
    for (size_t i = 0; ok && i < len; i++) ok = writeByte(data[i]);
    if (!ok || withStop) stop();
    return ok;
}

static bool receive(uint8_t addr, uint8_t* data, size_t len) {
    bool ok = start() && writeByte((addr << 1) | 1);
    if (ok)
        for (size_t i = 0; i < len; i++) data[i] = readByte(i + 1 < len);
    stop();
    return ok;
}
}  // namespace soft

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
    I2C_Bus_1_SDA = Settings::pinOutput("I2C_Bus_1_SDA", DEFAULT_I2C_BUS_1_SDA, "SDA pin");
    I2C_Bus_1_SCL = Settings::pinOutput("I2C_Bus_1_SCL", DEFAULT_I2C_BUS_1_SCL, "SCL pin");

    I2C_Bus_2_SDA = Settings::pinOutput("I2C_Bus_2_SDA", DEFAULT_I2C_BUS_2_SDA, "SDA pin");
    I2C_Bus_2_SCL = Settings::pinOutput("I2C_Bus_2_SCL", DEFAULT_I2C_BUS_2_SCL, "SCL pin");

    I2C_Bus_3_SDA = Settings::pinOutput("I2C_Bus_3_SDA", DEFAULT_I2C_BUS_3_SDA, "SDA pin");
    I2C_Bus_3_SCL = Settings::pinOutput("I2C_Bus_3_SCL", DEFAULT_I2C_BUS_3_SCL, "SCL pin");

    I2CDebug = Settings::checkbox("I2CDebug", false, "Debug I2C addreses. Look at the serial log to get the correct address");

    if (!I2C_Bus_1_Started && I2C_Bus_1_SDA != -1 && I2C_Bus_1_SCL != -1) {
        I2C_Bus_1_Started = startBus(0, I2C_NUM_0, I2C_Bus_1_SDA, I2C_Bus_1_SCL);
    }

#if HAS_I2C_BUS_2
    if (!I2C_Bus_2_Started && I2C_Bus_2_SDA != -1 && I2C_Bus_2_SCL != -1) {
        I2C_Bus_2_Started = startBus(1, I2C_NUM_1, I2C_Bus_2_SDA, I2C_Bus_2_SCL);
    }
#endif

    if (!I2C_Bus_3_Started && I2C_Bus_3_SDA != -1 && I2C_Bus_3_SCL != -1) {
        I2C_Bus_3_Started = soft::begin(I2C_Bus_3_SDA, I2C_Bus_3_SCL);
    }
}

int busSetting(const char* key, const char* label, int maxBus) {
    std::vector<std::string> names = {"None", "Bus 1", "Bus 2", "Bus 3"};
    names.resize(maxBus + 1);
    long b = Settings::dropdown(key, names, 0, label);
    return b >= 1 && b <= maxBus ? (int)b : 0;  // a stale -1 (SHT's old "disabled") is None
}

bool pickAddress(const std::string& setting, std::initializer_list<uint8_t> allowed, uint8_t& out) {
    if (setting.empty()) {
        out = *allowed.begin();
        return true;
    }
    long v = strtol(setting.c_str(), nullptr, 0);
    for (uint8_t a : allowed)
        if (a == v) {
            out = a;
            return true;
        }
    return false;
}

void Setup() {
}

bool started(int bus) { return bus == 3 ? soft::up : busHandle(bus) != nullptr; }

bool probe(int bus, uint8_t addr) {
    if (bus == 3) return soft::up && soft::probe(addr);
    auto bh = busHandle(bus);
    return bh && i2c_master_probe(bh, addr, TIMEOUT_MS) == ESP_OK;
}

bool write(int bus, uint8_t addr, const uint8_t* data, size_t len) {
    if (bus == 3) return soft::up && soft::sendBytes(addr, data, len, true);
    auto h = dev(bus, addr);
    return h && i2c_master_transmit(h, data, len, TIMEOUT_MS) == ESP_OK;
}

bool read(int bus, uint8_t addr, uint8_t* data, size_t len) {
    if (bus == 3) return soft::up && soft::receive(addr, data, len);
    auto h = dev(bus, addr);
    return h && i2c_master_receive(h, data, len, TIMEOUT_MS) == ESP_OK;
}

bool writeRead(int bus, uint8_t addr, const uint8_t* w, size_t wlen, uint8_t* r, size_t rlen) {
    if (bus == 3) return soft::up && soft::sendBytes(addr, w, wlen, false) && soft::receive(addr, r, rlen);
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
    Log.printf("Scanning I2C for devices on Bus %d...\n", bus);
    for (uint8_t address = 1; address < 127; address++) {
        if (probe(bus, address)) {
            Log.printf("I2C device found on bus %d at address 0x%02X\n", bus, address);
            n++;
        }
    }
    return n;
}

void SerialReport() {
    if (I2C_Bus_1_Started)
        Log.printf("I2C Bus 1:    sda=%d scl=%d\n", I2C_Bus_1_SDA, I2C_Bus_1_SCL);
    if (I2C_Bus_2_Started)
        Log.printf("I2C Bus 2:    sda=%d scl=%d\n", I2C_Bus_2_SDA, I2C_Bus_2_SCL);

    if (I2C_Bus_3_Started)
        Log.printf("I2C Bus 3:    sda=%d scl=%d (software)\n", I2C_Bus_3_SDA, I2C_Bus_3_SCL);

    if (!I2C_Bus_1_Started && !I2C_Bus_2_Started && !I2C_Bus_3_Started) return;
    if (!I2CDebug) return;

    int nDevices = 0;
    if (I2C_Bus_1_Started) nDevices += scan(1);
#if HAS_I2C_BUS_2
    if (I2C_Bus_2_Started) nDevices += scan(2);
#endif
    if (I2C_Bus_3_Started) nDevices += scan(3);
    // ponytail: the new driver only reports ack/nack, so the Wire "Unknown error" branch is gone.
    if (nDevices == 0) {
        Log.println("No I2C devices found\n");
    }
}
}  // namespace I2C

#endif
