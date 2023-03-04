#include <Arduino.h>
#include <WS2812FX.h>

#include "LED.h"

class Addressable : public LED {
   public:
    Addressable(uint8_t index, ControlType controlType, int type, int pin, int cnt);
    void begin() override;
    void service() override;

    bool setColor(uint8_t p_red, uint8_t p_green, uint8_t p_blue) override;
    bool setWhite(uint8_t p_white) override;
    bool setBrightness(uint8_t brightness) override;
    bool setState(bool p_state) override;
    bool setEffect(const char* p_effect) override;
    bool hasRgb() override { return true; }
    bool hasRgbw() override;

   private:
    WS2812FX* ws2812fx = NULL;
    int type;
    int pin;
    int cnt;
    int cntrl;
};
