#ifndef GUI_h
#define GUI_h

#include "BleFingerprint.h"
#include <Arduino.h>

#ifdef M5STICK
#ifdef PLUS
#include <M5StickCPlus.h>
#else
#include <M5StickC.h>
#endif
#elif defined M5ATOM
#include <M5Atom.h>
#endif

class BleFingerprint;

class GUI
{
public:
    static void setup();

    static void added(BleFingerprint *f);
    static void removed(BleFingerprint *f);
    static void close(BleFingerprint *f);
    static void left(BleFingerprint *f);

    static void erasing();
    static void erased();

    static void seenStart();
    static void seenEnd();

    static void updateStart();
    static void updateProgress(unsigned int percent);
    static void updateEnd();

    static void connecting();
    static void connected(bool wifi, bool mqtt);

    static void status(const char *message, ...);
    static void blit();

    static bool statusLed;

    static void plusOne(BleFingerprint *f);
    static void minusOne(BleFingerprint *f);

private:
    static void begin();

    static bool init;
#ifdef M5STICK
    static bool dirty;
    static TFT_eSprite sprite;
#endif
};
#endif
