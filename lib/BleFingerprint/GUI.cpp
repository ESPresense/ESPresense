#include "GUI.h"

GUI Display;

void GUI::seenStart()
{
    digitalWrite(LED_BUILTIN, LED_BUILTIN_ON);
}

void GUI::seenEnd()
{
    digitalWrite(LED_BUILTIN, !LED_BUILTIN_ON);
}

void GUI::connected(bool wifi = false, bool mqtt = false)
{
    status("Wifi: %s Mqtt: %s", (wifi ? "no" : "yes"), (wifi ? "no" : "yes"));
}

void GUI::close(BleFingerprint *f)
{
    Serial.printf("%d Close | MAC: %s, ID: %-50s\n", xPortGetCoreID(), f->getMac().c_str(), f->getId().c_str());
    status("C: %s", f->getId());
}

void GUI::added(BleFingerprint *f)
{
    Serial.printf("%d New   | MAC: %s, ID: %-50s\n", xPortGetCoreID(), f->getMac().c_str(), f->getId().c_str());
}

void GUI::removed(BleFingerprint *f)
{
    Serial.printf("%d Del   | MAC: %s, ID: %-50s\n", xPortGetCoreID(), f->getMac().c_str(), f->getId().c_str());
}

void GUI::left(BleFingerprint *f)
{
    Serial.printf("%d Left  | MAC: %s, ID: %-50s\n", xPortGetCoreID(), f->getMac().c_str(), f->getId().c_str());
    status("L: %s", f->getId());
}

void GUI::status(const char *format, ...)
{
#ifdef M5STICK
    sprite.fillSprite(TFT_BLACK);
    sprite.setTextDatum(MC_DATUM);

    char *message;
    va_list args;
    va_start(args, format);
    vasprintf(&message, format, args);
    va_end(args);
#ifdef PLUS
    sprite.drawString(message, sprite.width() / 2, sprite.height() / 2, 4);
#else
    sprite.drawString(message, sprite.width() / 2, sprite.height() / 2, 1);
#endif
    free(message);
    dirty = true;
#endif
}

void GUI::update()
{
#ifdef M5STICK
    if (!init)
    {
        M5.begin(true, true, false);
        M5.Lcd.setRotation(3);
        sprite.createSprite(M5.Lcd.width(), M5.Lcd.height());
        sprite.setSwapBytes(true);
        init = true;
    }
    if (dirty)
    {
        sprite.pushSprite(0, 0);
        M5.Axp.ScreenBreath(12);
        dirty = false;
    }
#endif
}
