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

void Addressable::begin() {
    if (ws2812fx == NULL) {
        ws2812fx = new WS2812FX(cnt, pin, getNeoPixelType(type), 1, 1);
        ws2812fx->init();
        ws2812fx->setColor(255, 255, 128);
        ws2812fx->setBrightness(64);
        ws2812fx->setMode(FX_MODE_STATIC);
        ws2812fx->start();
    }
}

void Addressable::service() {
    if (ws2812fx == NULL) begin();
    ws2812fx->service();
}

bool Addressable::setColor(uint8_t p_red, uint8_t p_green, uint8_t p_blue) {
    if (!LED::setColor(p_red, p_green, p_blue)) return false;
    if (ws2812fx == NULL) begin();
    ws2812fx->setColor(p_red, p_green, p_blue);
    ws2812fx->setBrightness(LED::getBrightness());
    LED::setState(true);
    return true;
}

bool Addressable::setBrightness(uint8_t p_brightness) {
    if (!LED::setBrightness(p_brightness)) return false;
    if (ws2812fx == NULL) begin();
    ws2812fx->setBrightness(map(p_brightness, 0, 255, 0, MAX_BRIGHTNESS));
    LED::setState(p_brightness > 0);
    return true;
}

bool Addressable::setState(bool p_state) {
    if (!LED::setState(p_state)) return false;
    if (ws2812fx == NULL) begin();
    ws2812fx->setBrightness(map(p_state ? LED::getBrightness() : 0, 0, 255, 0, MAX_BRIGHTNESS));
    return true;
}

bool Addressable::setWhite(uint8_t p_white) {
    Serial.printf("Addressable::setWhite: p_white=%d\n", p_white);
    if (ws2812fx == NULL) begin();
    ws2812fx->setColor(p_white, p_white, p_white);
    return true;
}

bool Addressable::setEffect(const char* p_effect) {
    // ws2812fx->setMode(p_effect);
    return true;
}

bool Addressable::hasRgbw()
{
    return this->type==1 || this->type==3;
}
