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
#endif

#if defined M5STICK

#define LED_BUILTIN 10
#define LED_BUILTIN_ON 0

#define BUTTON 39
#define BUTTON_PRESSED 0

#elif defined M5ATOM

#define LED_BUILTIN 10
#define LED_BUILTIN_ON 0

#define BUTTON 39
#define BUTTON_PRESSED 0

#elif defined HUZZAH32

#define LED_BUILTIN 13
#define LED_BUILTIN_ON 1

#else //DevKit / generic

#define LED_BUILTIN 2
#define LED_BUILTIN_ON 1

#endif

class GUI
{
public:
    void added(BleFingerprint *f);
    void removed(BleFingerprint *f);
    void close(BleFingerprint *f);
    void left(BleFingerprint *f);

    void seenStart();
    void seenEnd();
    void updateProgress(unsigned int percent) { digitalWrite(LED_BUILTIN, percent % 2); }
    void updateEnd() { digitalWrite(LED_BUILTIN, !LED_BUILTIN_ON); }

    void status(const char *message, ...);
    void connected(bool wifi, bool mqtt);
    void update();

private:
    bool init;
    bool dirty;
#ifdef M5STICK
    TFT_eSprite sprite = TFT_eSprite(&M5.Lcd);
#endif
};

extern GUI Display;

#endif
