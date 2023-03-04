#pragma once

#include <Arduino.h>

enum ControlType {
    Control_Type_None = -1,
    Control_Type_MQTT = 0,
    Control_Type_Status = 1,
    Control_Type_Motion = 2,
    Control_Type_Count = 3,
};

struct Color {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t white;
};

class LED {
   public:
    LED(uint8_t index, ControlType controlType);
    virtual void begin();
    virtual void service();

    virtual uint8_t getBrightness(void);
    virtual bool setBrightness(uint8_t brightness);

    const virtual Color getColor(void);
    virtual bool setColor(uint32_t color);
    virtual bool setColor(uint8_t red, uint8_t green, uint8_t blue);

    virtual bool setWhite(uint8_t white);

    virtual uint16_t getColorTemperature(void);
    virtual bool setColorTemperature(uint16_t colorTemperature);

    virtual bool setEffect(const char* effect);

    virtual bool getState(void);
    virtual bool setState(bool state);

    uint8_t getIndex() { return index; }
    ControlType getControlType() { return controlType; }
    const String getId();
    const String getName();

    virtual bool hasRgb() { return false; }
    virtual bool hasRgbw() { return false; }

   private:
    ControlType controlType;
    uint8_t index;
    Color color = {255, 255, 128, 128};
    bool state = true;
    uint8_t brightness = 64;
};
