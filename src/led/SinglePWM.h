#include <Arduino.h>

#include "LED.h"

class SinglePWM : public LED {
   public:

    SinglePWM(uint8_t index, ControlType controlType, bool inverted, int pin);
    void update() override;
    void service() override;

   private:
    void init();
    void setDuty(uint32_t value);
    bool inverted;
    int pin;
    bool inited = false;
};
