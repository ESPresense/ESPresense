#pragma once

#include <Arduino.h>

class Display {
   public:
    static void Setup();
    static void ConnectToWifi();
    static void Loop();

    static void Status(const char *message);
};
