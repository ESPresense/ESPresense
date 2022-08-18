#pragma once

#include <Arduino.h>
#include "BleFingerprint.h"

class BleFingerprint;

class Display {
   public:
    static void Setup();
    static void ConnectToWifi();
    static void Loop();

    static void Added(BleFingerprint *f);
    static void Removed(BleFingerprint *f);
    static void Close(BleFingerprint *f);
    static void Left(BleFingerprint *f);

    static void Pir(bool value);
    static void Radar(bool value);

    static void Erasing();
    static void Erased();

    static void SeenStart();
    static void SeenEnd();

    static void UpdateStart();
    static void UpdateProgress(unsigned int percent);
    static void UpdateEnd();

    static void Connecting();
    static void Connected(bool wifi, bool mqtt);

    static void Status(const char *message, ...);

    static void PlusOne(BleFingerprint *f);
    static void MinusOne(BleFingerprint *f);

   private:
    static bool init;
#ifdef M5STICK
    static bool dirty;
#endif
};
