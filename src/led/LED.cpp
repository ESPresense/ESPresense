#include "LED.h"

#include "string_utils.h"

void LED::begin() {}
void LED::service() {}

uint8_t LED::getBrightness(void) {
    return brightness;
}

bool LED::setBrightness(uint8_t p_brightness) {
    // Serial.printf("LED::setBrightness(%d)\n", p_brightness);
    if (p_brightness == brightness) return false;
    if (p_brightness > 0)
        brightness = p_brightness;
    else
        LED::setState(false);
    return true;
}

const Color LED::getColor(void) {
    return color;
}

bool LED::setColor(uint32_t color) {
    return LED::setColor((color & 0xFF0000) >> 16, (color & 0x00FF00) >> 8, (color & 0x0000FF));
}

bool LED::setColor(uint8_t p_red, uint8_t p_green, uint8_t p_blue) {
    if (p_red == color.red && p_green == color.green && p_blue == color.blue) {
        return false;
    }
    // Serial.printf("LED::setColor(%d, %d, %d)\n", p_red, p_green, p_blue);
    color.red = p_red;
    color.green = p_green;
    color.blue = p_blue;
    return true;
}

bool LED::setWhite(uint8_t p_white) {
    Serial.printf("LED::setWhite(%d)\n", p_white);
    return false;
}

uint16_t LED::getColorTemperature(void) {
    return 0;
}

bool LED::setColorTemperature(uint16_t p_colorTemperature) {
    Serial.printf("LED::setColorTemperature(%d)\n", p_colorTemperature);
    return false;
}

bool LED::setEffect(const char *p_effect) {
    Serial.printf("LED::setEffect(%s)\n", p_effect);
    return false;
}

bool LED::getState(void) {
    return state;
}

bool LED::setState(bool p_state) {
    if (state == p_state) return false;
    // Serial.printf("LED::setState(%s)\n", p_state ? "true" : "false");
    state = p_state;
    return true;
}

const String LED::getName() {
    return Sprintf("LED %d", index);
}

LED::LED(uint8_t index, ControlType controlType) {
    this->index = index;
    this->controlType = controlType;
}

const String LED::getId() {
    return Sprintf("led_%d", index);
}
