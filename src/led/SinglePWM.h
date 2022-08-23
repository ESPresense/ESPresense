#include <Arduino.h>

#include "LED.h"

class SinglePWM : public LED {
   public:
    SinglePWM(uint8_t index, ControlType controlType, int type, int pin, int cnt);
    void begin() override;
    void service() override;

    bool setState(bool state) override;
    bool setBrightness(uint8_t brightness) override;

   private:
    void setDuty(uint32_t value);
    int type;
    int pin;
    int cnt;
    int cntrl;
};
