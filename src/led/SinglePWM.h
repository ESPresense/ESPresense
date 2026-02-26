#include <Arduino.h>

#include "LED.h"

class SinglePWM : public LED {
   public:

    SinglePWM(uint8_t index, ControlType controlType, bool inverted, int pin, uint16_t fadeTimeMs = 0);
    void update() override;
    void service() override;

   private:
    void init();
    uint32_t toDuty(uint32_t value) const;
    void writeDuty(uint32_t duty);
    bool inverted;
    int pin;
    uint16_t fadeTimeMs;
    uint32_t currentDuty = 0;
    uint32_t startDuty = 0;
    uint32_t targetDuty = 0;
    uint32_t fadeStartMs = 0;
    bool fadeActive = false;
    bool inited = false;
};
