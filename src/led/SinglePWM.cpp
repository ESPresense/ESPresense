#include "SinglePWM.h"

SinglePWM::SinglePWM(uint8_t index, ControlType controlType, bool inverted, int pin) : LED(index, controlType) {
    this->inverted = inverted;
    this->pin = pin;
}

void SinglePWM::init() {
    inited = true;
    pinMode(pin, OUTPUT);
#ifdef ARDUINO_V3
    ledcAttach(pin, 5000, 12);
#else
    ledcSetup(LED::getIndex(), 5000, 12);
    ledcAttachPin(pin, getIndex());
#endif
}

void SinglePWM::update() {
    setDuty(LED::getState() ? LED::getBrightness() : 0);
}

void SinglePWM::setDuty(uint32_t x) {
    if (!inited) init();
    uint32_t duty = x >= 255 ? 4096 : (x <= 0 ? 0 : round(4096.0 * pow(10.0, 0.0055 * (x - 255.0))));
    if (inverted) duty = 4096 - duty;
#ifdef ARDUINO_V3
    ledcWrite(pin, duty);
#else
    ledcWrite(LED::getIndex(), duty);
#endif
}

void SinglePWM::service() {
}
