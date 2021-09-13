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

#ifdef M5STICK

#define LED_BUILTIN 10
#define LED_BUILTIN_ON 0

#define BUTTON 39
#define BUTTON_PRESSED 0

#else
#if M5ATOM

#define LED_BUILTIN 10
#define LED_BUILTIN_ON 0

#define BUTTON 39
#define BUTTON_PRESSED 0

#else // Huzzah32 or DevKit

#define LED_BUILTIN 13
#define LED_BUILTIN_ON 1

#define BUTTON 15
#define BUTTON_PRESSED 1

#endif
#endif

#ifdef M5STICK

#else

#endif

class GUI
{
public:
    void seenStart();
    void seenEnd();
    void updateProgress(unsigned int percent) { digitalWrite(LED_BUILTIN, percent % 2); }
    void updateEnd() { digitalWrite(LED_BUILTIN, !LED_BUILTIN_ON); }
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

extern GUI Display;

#endif
