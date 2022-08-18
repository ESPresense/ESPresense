#pragma once

#include <Arduino.h>

enum ControlType {
    Control_Type_MQTT,
    Control_Type_Status,
    Control_Type_Motion,
    Control_Type_Count,
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
    virtual void Setup();
    virtual void Loop();

    const virtual uint8_t getBrightness(void);
    virtual bool setBrightness(uint8_t brightness);

    const virtual Color getColor(void);
    virtual bool setColor(uint8_t red, uint8_t green, uint8_t blue);

    virtual bool setWhite(uint8_t white);

    const virtual uint16_t getColorTemperature(void);
    virtual bool setColorTemperature(uint16_t colorTemperature);

    virtual bool setEffect(const char* effect);

    const virtual bool getState(void);
    virtual bool setState(bool state);

    const uint8_t getIndex() { return index; }
    const ControlType getControlType() { return controlType; }
    const String getId();
    const String getName();

private:
    ControlType controlType;
    uint8_t index;
    Color color = {255, 255, 128, 128};
    bool state = true;
    uint8_t brightness = 64;
};
