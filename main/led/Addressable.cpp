#include "Addressable.h"

#include <algorithm>

#include "Logger.h"
#include "defaults.h"

Addressable::Addressable(uint8_t index, ControlType controlType, int type, int pin, int cnt) : LED(index, controlType) {
    this->type = type;
    this->pin = pin;
    this->cnt = cnt;
}

// type index 0..3 = GRB, GRBW, RGB, RGBW (same order the Adafruit_NeoPixel build used).
static led_strip_config_t getStripConfig(int type, int pin, int cnt) {
    led_strip_config_t cfg = {};
    cfg.strip_gpio_num = pin;
    cfg.max_leds = cnt;
    switch (type) {
        case 1:
            cfg.led_model = LED_MODEL_SK6812;
            cfg.color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_GRBW;
            break;
        case 2:
            cfg.led_model = LED_MODEL_WS2812;
            cfg.color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_RGB;
            break;
        case 3:
            cfg.led_model = LED_MODEL_SK6812;
            cfg.color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_RGBW;
            break;
        case 0:
        default:
            cfg.led_model = LED_MODEL_WS2812;
            cfg.color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_GRB;
            break;
    }
    cfg.flags.invert_out = false;
    return cfg;
}

// Adafruit_NeoPixel::setBrightness scaled every component by (c * (b + 1)) >> 8 (b = 255 → unscaled).
static inline uint8_t scale(uint8_t c, uint8_t b) {
    return b == 255 ? c : (uint8_t)(((uint16_t)c * (b + 1)) >> 8);
}

void Addressable::update() {
    if (strip == nullptr) {
        led_strip_config_t cfg = getStripConfig(type, pin, cnt);
        led_strip_rmt_config_t rmt = {};
        rmt.clk_src = RMT_CLK_SRC_DEFAULT;
        rmt.resolution_hz = 10 * 1000 * 1000;  // 10 MHz, 0.1 µs per tick
        rmt.flags.with_dma = false;
        esp_err_t err = led_strip_new_rmt_device(&cfg, &rmt, &strip);
        if (err != ESP_OK) {
            Log.printf("LED %d: led_strip init failed: %s\r\n", getIndex(), esp_err_to_name(err));
            strip = nullptr;
            return;
        }
        led_strip_clear(strip);
    }

    Color color = LED::getColor();
    uint8_t brightness = mapBrightness(LED::getBrightness());

    if (LED::getState()) {
        uint8_t r = scale(color.red, brightness);
        uint8_t g = scale(color.green, brightness);
        uint8_t b = scale(color.blue, brightness);
        // Fill all pixels with the same color for quick operation.
        // White stays 0 like the old pixels->Color(r, g, b) call did.
        for (int i = 0; i < cnt; i++) {
            if (hasRgbw())
                led_strip_set_pixel_rgbw(strip, i, r, g, b, 0);
            else
                led_strip_set_pixel(strip, i, r, g, b);
        }
        led_strip_refresh(strip);
    } else {
        // Turn off all pixels (clear() also flushes to the hardware)
        led_strip_clear(strip);
    }
}

void Addressable::service() {
    // No need for continuous service in this implementation
    // This method is now a no-op, but kept for API compatibility
}

uint8_t Addressable::mapBrightness(uint8_t brightness) {
    // Special case for zero brightness
    if (brightness == 0) return 0;

    // For non-zero values, ensure we have at least brightness level 1
    // and map the rest of the range proportionally
    long const result = 1 + ((long)(brightness - 1) * (MAX_BRIGHTNESS - 1)) / 254;

    // Ensure we stay within byte range
    return (uint8_t)std::min(result, (long)MAX_BRIGHTNESS);
}

bool Addressable::hasRgbw() {
    return this->type == 1 || this->type == 3;
}
