#ifndef GUI_h
#define GUI_h

#include <Arduino.h>

#ifdef M5STICK
#ifdef PLUS
#include <M5StickCPlus.h>
#else
#include <M5StickC.h>
#endif
#endif

class GUI
{
public:
    void close(const char *mac, const char *id);
    void left(const char *mac, const char *id);
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

#endif

extern GUI Display;
