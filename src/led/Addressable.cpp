#include "Addressable.h"
#include "defaults.h"

Addressable::Addressable(uint8_t index, ControlType controlType, int type, int pin, int cnt) : LED(index, controlType) {
    this->type = type;
    this->pin = pin;
    this->cnt = cnt;
}

neoPixelType getNeoPixelType(int type) {
    switch (type) {
        case 0:
            return NEO_GRB + NEO_KHZ800;
        case 1:
            return NEO_GRBW + NEO_KHZ800;
        case 2:
            return NEO_RGB + NEO_KHZ800;
        case 3:
            return NEO_RGBW + NEO_KHZ800;
    }
    return NEO_GRB + NEO_KHZ800;
}

void Addressable::update() {
    if (pixels == nullptr) {
        pixels = new Adafruit_NeoPixel(cnt, pin, getNeoPixelType(type));
        pixels->begin();
        pixels->clear();
    }

    Color color = LED::getColor();
    uint32_t pixelColor = pixels->Color(color.red, color.green, color.blue);
    uint8_t brightness = mapBrightness(LED::getBrightness());

    pixels->setBrightness(brightness);

    if (LED::getState()) {
        // Fill all pixels with the same color for quick operation
        for (int i = 0; i < cnt; i++) {
            pixels->setPixelColor(i, pixelColor);
        }
    } else {
        // Turn off all pixels
        pixels->clear();
    }

    // Send the updated pixel colors to the hardware
    pixels->show();
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
    return (uint8_t)min(result, (long)MAX_BRIGHTNESS);
}

bool Addressable::hasRgbw() {
    return this->type == 1 || this->type == 3;
}
