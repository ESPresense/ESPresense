#pragma once

#include <cstdint>
#include <string>

enum ControlType {
    Control_Type_None = -1,
    Control_Type_MQTT = 0,
    Control_Type_Status = 1,
    Control_Type_Motion = 2,
    Control_Type_Count = 3,
    Control_Type_Output = 4,  // mirrors an output (a plug's relay LED)
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
    virtual ~LED() = default;
    virtual void update();
    virtual void service();

    virtual uint8_t getBrightness(void);
    virtual bool setBrightness(uint8_t brightness);

    const virtual Color getColor(void);
    virtual bool setColor(uint32_t color);
    virtual bool setColor(uint8_t red, uint8_t green, uint8_t blue, uint8_t white = 0);

    virtual bool setWhite(uint8_t white);

    virtual uint16_t getColorTemperature(void);
    virtual bool setColorTemperature(uint16_t colorTemperature);

    virtual bool setEffect(const char* effect);

    virtual bool getState(void);
    virtual bool setState(bool state);

    uint8_t getIndex() { return index; }
    ControlType getControlType() { return controlType; }
    const std::string getId();
    const std::string getName();

    bool getDirty() { return this->dirty; }
    void setDirty(bool dirty) { this->dirty = dirty; }

    const std::string getStateFilename();
    const std::string getStateString();
    void setStateString(const std::string& encoded);

    // Ceiling for the output, 1-255 (led max_brightness): the brightness range is scaled into it.
    void setMaxBrightness(uint8_t max) { maxBrightness = max ? max : 1; }
    uint8_t getMaxBrightness() { return maxBrightness; }

    virtual bool hasRgb() { return false; }
    virtual bool hasRgbw() { return false; }

   private:
    ControlType controlType;
    uint8_t index;
    Color color = {255, 255, 128, 128};
    bool state = true;
    uint8_t brightness = 128;
    bool dirty = false;
    uint8_t maxBrightness = 255;
};
