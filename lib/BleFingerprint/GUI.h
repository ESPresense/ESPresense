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

#if defined M5STICK

#define LED_BUILTIN 10
#define LED_BUILTIN_ON 0

#define BUTTON 39
#define BUTTON_PRESSED 0

#elif defined M5ATOM

#define BUTTON 39
#define BUTTON_PRESSED 0

#elif defined HUZZAH32

#define LED_BUILTIN 13
#define LED_BUILTIN_ON 1

#else //DevKit / generic

#define LED_BUILTIN 2
#define LED_BUILTIN_ON 1

#endif

class BleFingerprint;

class GUI
{
public:
    void added(BleFingerprint *f);
    void removed(BleFingerprint *f);
    void close(BleFingerprint *f);
    void left(BleFingerprint *f);

    void erasing();
    void erased();

    void seenStart();
    void seenEnd();

    void updateProgress(unsigned int percent);
    void updateEnd();

    void connecting();
    void connected(bool wifi, bool mqtt);

    void status(const char *message, ...);
    void update();

private:
    bool init;
#ifdef M5STICK
    bool dirty;
    TFT_eSprite sprite = TFT_eSprite(&M5.Lcd);
#endif
};

extern GUI Display;

#endif
