#include "SinglePWM.h"

SinglePWM::SinglePWM(uint8_t index, ControlType controlType, int type, int pin, int cnt) : LED(index, controlType) {
    this->type = type;
    this->pin = pin;
    this->cnt = cnt;
}

void SinglePWM::Setup() {
    ledcSetup(LED::getIndex(), 12000, 12);
    ledcAttachPin(pin, getIndex());
    setDuty(LED::getBrightness());
}

void SinglePWM::setDuty(uint32_t value) {
    uint32_t duty = (type & 0x1 == 1) ? map(value, 0, 255, 0, 4095) : map(value, 0, 255, 4095, 0);
    ledcWrite(LED::getIndex(), duty);
}

void SinglePWM::Loop() {
}

bool SinglePWM::setState(bool state) {
    if (!LED::setState(state)) return false;
    setDuty(state ? LED::getBrightness() : 0);
    return true;
}

bool SinglePWM::setBrightness(uint8_t brightness) {
    if (!LED::setBrightness(brightness)) return false;
    setDuty(brightness);
    return true;
}
