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
    void setup();

    void added(BleFingerprint *f);
    void removed(BleFingerprint *f, long age);
    void close(BleFingerprint *f);
    void left(BleFingerprint *f);

    void erasing();
    void erased();

    void seenStart();
    void seenEnd();

    void updateStart();
    void updateProgress(unsigned int percent);
    void updateEnd();

    void connecting();
    void connected(bool wifi, bool mqtt);

    void status(const char *message, ...);
    void blit();

    void setStatusLed(bool enabled) { _statusLed = enabled; }

private:
    bool _statusLed = true;
    void begin();

    bool init;
#ifdef M5STICK
    bool dirty;
    TFT_eSprite sprite = TFT_eSprite(&M5.Lcd);
#endif
};

extern GUI Display;

#endif
