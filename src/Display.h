#pragma once

#include <Arduino.h>
#include "BleFingerprint.h"

class BleFingerprint;

class Display {
   public:
    static void Setup();
    static void ConnectToWifi();
    static void Loop();

    static void Status(const char *message, ...);

   private:
    static bool init;
#ifdef M5STICK
    static bool dirty;
#endif
};
