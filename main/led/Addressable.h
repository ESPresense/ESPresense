#ifndef ADDRESSABLE_H
#define ADDRESSABLE_H

#include "LED.h"
#include "led_strip.h"

class Addressable : public LED {
public:
    Addressable(uint8_t index, ControlType controlType, int type, int pin, int cnt);
    void update() override;
    void service() override;
    bool hasRgb() override { return true; }
    bool hasRgbw() override;

private:
    int type;
    int pin;
    int cnt;
    led_strip_handle_t strip = nullptr;
    uint8_t mapBrightness(uint8_t brightness);
};

#endif // ADDRESSABLE_H
