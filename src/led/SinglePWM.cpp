#include "SinglePWM.h"

SinglePWM::SinglePWM(uint8_t index, ControlType controlType, bool inverted, int pin, uint16_t fadeTimeMs) : LED(index, controlType) {
    this->inverted = inverted;
    this->pin = pin;
    this->fadeTimeMs = fadeTimeMs;
}

void SinglePWM::init() {
    inited = true;
    pinMode(pin, OUTPUT);
    ledcSetup(LED::getIndex(), 5000, 12);
    ledcAttachPin(pin, getIndex());
}

void SinglePWM::update() {
    uint32_t const newTargetDuty = toDuty(LED::getState() ? LED::getBrightness() : 0);

    if (!inited) {
        init();
        writeDuty(newTargetDuty);
        startDuty = targetDuty = newTargetDuty;
        fadeActive = false;
        return;
    }

    if (newTargetDuty == targetDuty && !fadeActive) {
        return;
    }

    if (fadeTimeMs == 0) {
        writeDuty(newTargetDuty);
        startDuty = targetDuty = newTargetDuty;
        fadeActive = false;
        return;
    }

    startDuty = currentDuty;
    targetDuty = newTargetDuty;
    fadeStartMs = millis();
    fadeActive = true;
}

uint32_t SinglePWM::toDuty(uint32_t x) const {
    uint32_t duty = x >= 255 ? 4096 : (x <= 0 ? 0 : round(4096.0 * pow(10.0, 0.0055 * (x - 255.0))));
    if (inverted) duty = 4096 - duty;
    return duty;
}

void SinglePWM::writeDuty(uint32_t duty) {
    currentDuty = duty;
    ledcWrite(LED::getIndex(), duty);
}

void SinglePWM::service() {
    if (!fadeActive) return;

    uint32_t const elapsed = millis() - fadeStartMs;
    if (elapsed >= fadeTimeMs) {
        writeDuty(targetDuty);
        fadeActive = false;
        return;
    }

    int32_t const delta = (int32_t)targetDuty - (int32_t)startDuty;
    int32_t const nextDuty = (int32_t)startDuty + (delta * (int32_t)elapsed) / (int32_t)fadeTimeMs;
    writeDuty((uint32_t)constrain(nextDuty, 0, 4096));
}
