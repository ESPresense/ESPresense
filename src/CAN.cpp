#include "CAN.h"

#include <Arduino.h>

#include "defaults.h"
#include "globals.h"
#include "mqtt.h"

namespace CAN {

void Setup() {
#if MACCHINA_A0
    // This disables the CAN transceiver on the A0 board
    pinMode(GPIO_NUM_21, OUTPUT);
    digitalWrite(GPIO_NUM_21, HIGH);
#endif
}

bool SendDiscovery() {
    return true;
}
}  // namespace CAN
