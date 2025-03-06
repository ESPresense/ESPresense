#include "LED.h"

#include "string_utils.h"

void LED::update() {}
void LED::service() {}

uint8_t LED::getBrightness() {
    return brightness;
}

bool LED::setBrightness(uint8_t p_brightness) {
    // Serial.printf("LED::setBrightness(%d)\r\n", p_brightness);
    if (p_brightness == brightness) return false;
    if (p_brightness > 0)
        brightness = p_brightness;
    dirty = true;
    update();
    return true;
}

const Color LED::getColor() {
    return color;
}

bool LED::setColor(uint32_t color) {
    return LED::setColor((color & 0xFF0000) >> 16, (color & 0x00FF00) >> 8, (color & 0x0000FF));
}

bool LED::setColor(uint8_t p_red, uint8_t p_green, uint8_t p_blue, uint8_t p_white) {
    if (p_red == color.red && p_green == color.green && p_blue == color.blue && p_white == color.white) {
        return false;
    }
    // Serial.printf("LED::setColor(%d, %d, %d)\r\n", p_red, p_green, p_blue);
    color.red = p_red;
    color.green = p_green;
    color.blue = p_blue;
    color.white = p_white;
    dirty = true;
    update();
    return true;
}

bool LED::setWhite(uint8_t p_white) {
    //Serial.printf("LED::setWhite(%d)\r\n", p_white);
    if (!LED::setColor(255, 255, 255) && !LED::setBrightness(p_white)) return false;
    return true;
}

uint16_t LED::getColorTemperature(void) {
    return 0;
}

bool LED::setColorTemperature(uint16_t p_colorTemperature) {
    //Serial.printf("LED::setColorTemperature(%d)\r\n", p_colorTemperature);
    dirty = true;
    return false;
}

bool LED::setEffect(const char *p_effect) {
    //Serial.printf("LED::setEffect(%s)\r\n", p_effect);
    dirty = true;
    return false;
}

bool LED::getState() {
    return state;
}

bool LED::setState(bool p_state) {
    if (state == p_state) return false;
    // Serial.printf("LED::setState(%s)\r\n", p_state ? "true" : "false");
    state = p_state;
    dirty = true;
    update();
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

const String LED::getStateFilename() {
    return Sprintf("/led_%d_state", index);
}

const String LED::getStateString() {
    // Format: BBRGGBBWW (B=brightness, R=red, G=green, B=blue, W=white)
    char stateStr[11];
    sprintf(stateStr, "%02X%02X%02X%02X%02X",
            brightness,
            color.red,
            color.green,
            color.blue,
            color.white);
    return String(stateStr);
}

void LED::setStateString(const String& encoded) {
    if (encoded.length() == 10) {
        // Parse hex values - each value is 2 hex digits
        uint8_t const brightness = strtol(encoded.substring(0, 2).c_str(), NULL, 16);
        uint8_t const r = strtol(encoded.substring(2, 4).c_str(), NULL, 16);
        uint8_t const g = strtol(encoded.substring(4, 6).c_str(), NULL, 16);
        uint8_t const b = strtol(encoded.substring(6, 8).c_str(), NULL, 16);
        uint8_t const w = strtol(encoded.substring(8, 10).c_str(), NULL, 16);

        if (hasRgbw()) {
            setColor(r, g, b, w);
        } else if (hasRgb()) {
            setColor(r, g, b);
        }

        setBrightness(brightness);
    }
}
