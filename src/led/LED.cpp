#include "LED.h"

#include "Logger.h"
#include "string_utils.h"

/**
 * @brief Apply any pending state changes to the LED hardware.
 *
 * This default implementation does nothing; platform-specific subclasses should
 * override this method to push brightness, color, and state changes to the
 * physical LED.
 */
void LED::update() {}
void LED::service() {}

/**
 * @brief Retrieves the current LED brightness.
 *
 * @return uint8_t Current brightness level (0–255).
 */
uint8_t LED::getBrightness() {
    return brightness;
}

/**
 * @brief Request a change to the LED's brightness and trigger an update when the requested value differs from the current one.
 *
 * If `p_brightness` is greater than zero the stored brightness value is updated to that value. In all cases where
 * `p_brightness` differs from the current brightness the LED's state is marked dirty and an update is triggered.
 *
 * @param p_brightness Desired brightness value (0-255). Values greater than 0 replace the stored brightness.
 * @return bool `true` if the requested brightness differs from the current brightness (and an update was triggered), `false` if the requested value equals the current brightness.
 */
bool LED::setBrightness(uint8_t p_brightness) {
    // Log.printf("LED::setBrightness(%d)\r\n", p_brightness);
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

/**
 * @brief Update the LED's RGBW color.
 *
 * If the provided RGWB components differ from the current values, the color is updated,
 * the LED is marked dirty, and update() is invoked.
 *
 * @param p_red Red component (0–255).
 * @param p_green Green component (0–255).
 * @param p_blue Blue component (0–255).
 * @param p_white White component (0–255).
 * @return bool `true` if the color was changed and applied, `false` if the provided color matches the current color.
 */
bool LED::setColor(uint8_t p_red, uint8_t p_green, uint8_t p_blue, uint8_t p_white) {
    if (p_red == color.red && p_green == color.green && p_blue == color.blue && p_white == color.white) {
        return false;
    }
    // Log.printf("LED::setColor(%d, %d, %d)\r\n", p_red, p_green, p_blue);
    color.red = p_red;
    color.green = p_green;
    color.blue = p_blue;
    color.white = p_white;
    dirty = true;
    update();
    return true;
}

/**
 * @brief Attempt to set the LED to white by adjusting color and brightness.
 *
 * @param p_white Brightness level from 0 to 255.
 * @return true if the color or brightness was changed, false otherwise.
 */
bool LED::setWhite(uint8_t p_white) {
    //Log.printf("LED::setWhite(%d)\r\n", p_white);
    if (!LED::setColor(255, 255, 255) && !LED::setBrightness(p_white)) return false;
    return true;
}

/**
 * @brief Retrieves the LED's color temperature.
 *
 * The function is currently unimplemented and always reports a color temperature of 0.
 *
 * @return uint16_t Color temperature in Kelvin; currently always `0`.
 */
uint16_t LED::getColorTemperature(void) {
    return 0;
}

/**
 * @brief Attempt to set the LED color temperature.
 *
 * Marks the LED state as dirty; color temperature adjustment is not applied by this implementation.
 *
 * @param p_colorTemperature Desired color temperature in mireds (or device-specific units).
 * @return true if the color temperature was applied, false otherwise.
 */
bool LED::setColorTemperature(uint16_t p_colorTemperature) {
    //Log.printf("LED::setColorTemperature(%d)\r\n", p_colorTemperature);
    dirty = true;
    return false;
}

/**
 * @brief Marks the LED state dirty and attempts to set an effect (effect application not supported).
 *
 * @param p_effect Name of the effect to apply.
 * @return bool `false` indicating the requested effect is not applied.
 */
bool LED::setEffect(const char *p_effect) {
    //Log.printf("LED::setEffect(%s)\r\n", p_effect);
    dirty = true;
    return false;
}

bool LED::getState() {
    return state;
}

/**
 * @brief Set the LED's on/off state.
 *
 * If the provided state differs from the current state, updates the internal state,
 * marks it dirty, calls update(), and reports that a change occurred.
 *
 * @param p_state `true` to turn the LED on, `false` to turn it off.
 * @return `true` if the state was changed, `false` if the provided state matched the current state.
 */
bool LED::setState(bool p_state) {
    if (state == p_state) return false;
    // Log.printf("LED::setState(%s)\r\n", p_state ? "true" : "false");
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
    // Format: SBBRRGGBBWW (S=state, B=brightness, R=red, G=green, B=blue, W=white)
    char stateStr[13];
    sprintf(stateStr, "%02X%02X%02X%02X%02X%02X",
            state ? 1 : 0,
            brightness,
            color.red,
            color.green,
            color.blue,
            color.white);
    return String(stateStr);
}

void LED::setStateString(const String& encoded) {
    bool hasState = encoded.length() == 12;
    if (!hasState && encoded.length() != 10) {
        return;
    }

    uint8_t const offset = hasState ? 2 : 0;
    // Parse hex values - each value is 2 hex digits
    uint8_t const savedState = hasState ? strtol(encoded.substring(0, 2).c_str(), NULL, 16) : 1;
    uint8_t const brightness = strtol(encoded.substring(offset + 0, offset + 2).c_str(), NULL, 16);
    uint8_t const r = strtol(encoded.substring(offset + 2, offset + 4).c_str(), NULL, 16);
    uint8_t const g = strtol(encoded.substring(offset + 4, offset + 6).c_str(), NULL, 16);
    uint8_t const b = strtol(encoded.substring(offset + 6, offset + 8).c_str(), NULL, 16);
    uint8_t const w = strtol(encoded.substring(offset + 8, offset + 10).c_str(), NULL, 16);

    if (hasRgbw()) {
        setColor(r, g, b, w);
    } else if (hasRgb()) {
        setColor(r, g, b);
    }

    setBrightness(brightness);
    setState(savedState > 0);
}
