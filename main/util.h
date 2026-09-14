#pragma once
// Small native replacements for the Arduino conveniences the codebase grew up with.
// Everything here is a thin inline over ESP-IDF; nothing Arduino is linked.
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#include "driver/gpio.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

inline unsigned long millis() { return (unsigned long)(esp_timer_get_time() / 1000); }
inline unsigned long micros() { return (unsigned long)esp_timer_get_time(); }
inline void delay(uint32_t ms) { vTaskDelay(ms ? pdMS_TO_TICKS(ms) : 1); }
inline void yield() { vTaskDelay(1); }
inline uint32_t freeHeap() { return esp_get_free_heap_size(); }
inline uint32_t maxAllocHeap() { return heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT); }
uint64_t getNowMs();

// Arduino's ESP.getEfuseMac() >> 24: the base MAC's bytes 3..5 as a little-endian int.
// Topics and HA unique_ids are built from it, so it has to stay bit-identical.
inline uint32_t chipId() {
    uint8_t mac[6] = {0};
    esp_efuse_mac_get_default(mac);
    return (uint32_t)mac[3] | ((uint32_t)mac[4] << 8) | ((uint32_t)mac[5] << 16);
}
#define CHIPID chipId()
#define ESPMAC (Sprintf("%06x", (unsigned)CHIPID))

// asprintf mallocs *s and returns -1 on OOM leaving *s indeterminate; only touch s on success.
#define Sprintf(f, ...) ({ char* s = nullptr; std::string r; if (asprintf(&s, f, __VA_ARGS__) >= 0 && s) { r = s; free(s); } r; })
#define Stdprintf Sprintf

// Arduino String(x) formatting equivalents.
inline std::string toStr(int v) { return std::to_string(v); }
inline std::string toStr(long v) { return std::to_string(v); }
inline std::string toStr(unsigned v) { return std::to_string(v); }
inline std::string toStr(unsigned long v) { return std::to_string(v); }
inline std::string toStr(bool v) { return v ? "1" : "0"; }
inline std::string toStr(float v, int decimals = 2) { return Sprintf("%.*f", decimals, (double)v); }
inline std::string toStr(double v, int decimals = 2) { return Sprintf("%.*f", decimals, v); }
inline long toInt(const std::string& s) { return strtol(s.c_str(), nullptr, 10); }
inline float toFloat(const std::string& s) { return strtof(s.c_str(), nullptr); }
inline bool startsWith(const std::string& s, const char* p) { return s.compare(0, strlen(p), p) == 0; }
inline bool endsWith(const std::string& s, const char* p) { size_t n = strlen(p); return s.size() >= n && s.compare(s.size() - n, n, p) == 0; }

// GPIO. Same names as before so the sensor/switch code reads unchanged; gpio driver underneath.
enum PinMode { INPUT, OUTPUT, INPUT_PULLUP, INPUT_PULLDOWN };
constexpr int LOW = 0, HIGH = 1;
inline void pinMode(int pin, PinMode mode) {
    gpio_config_t io = {};
    io.pin_bit_mask = 1ULL << pin;
    io.mode = mode == OUTPUT ? GPIO_MODE_OUTPUT : GPIO_MODE_INPUT;
    io.pull_up_en = mode == INPUT_PULLUP ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE;
    io.pull_down_en = mode == INPUT_PULLDOWN ? GPIO_PULLDOWN_ENABLE : GPIO_PULLDOWN_DISABLE;
    io.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&io);
}
inline int digitalRead(int pin) { return gpio_get_level((gpio_num_t)pin); }
inline void digitalWrite(int pin, int level) { gpio_set_level((gpio_num_t)pin, level); }

#define log_e(f, ...) ESP_LOGE("app", f, ##__VA_ARGS__)
#define log_w(f, ...) ESP_LOGW("app", f, ##__VA_ARGS__)
#define log_i(f, ...) ESP_LOGI("app", f, ##__VA_ARGS__)
#define log_d(f, ...) ESP_LOGD("app", f, ##__VA_ARGS__)
