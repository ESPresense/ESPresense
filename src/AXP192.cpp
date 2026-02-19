
#include <Arduino.h>
#include <Wire.h>

#if defined(WIRE_INTERFACES_COUNT) && (WIRE_INTERFACES_COUNT > 1)
  #define AXP192_WIRE_PORT Wire1
#else
  #define AXP192_WIRE_PORT Wire
#endif

namespace AXP192 {

uint8_t Read8bit(uint8_t Addr) {
    AXP192_WIRE_PORT.beginTransmission(0x34);
    AXP192_WIRE_PORT.write(Addr);
    AXP192_WIRE_PORT.endTransmission();
    AXP192_WIRE_PORT.requestFrom(0x34, 1);
    return AXP192_WIRE_PORT.read();
}

void Write1Byte(uint8_t Addr, uint8_t Data) {
    AXP192_WIRE_PORT.beginTransmission(0x34);
    AXP192_WIRE_PORT.write(Addr);
    AXP192_WIRE_PORT.write(Data);
    AXP192_WIRE_PORT.endTransmission();
}

void SetLDO2(bool State) {
    uint8_t buf = Read8bit(0x12);
    if (State == true)
        buf = (1 << 2) | buf;
    else
        buf = ~(1 << 2) & buf;
    Write1Byte(0x12, buf);
}

void Setup() {
    AXP192_WIRE_PORT.begin(21, 22);
    AXP192_WIRE_PORT.setClock(400000);
    Write1Byte(0x28, 0xcc);
    Write1Byte(0x82, 0xff);
    Write1Byte(0x33, 0xc0);
    Write1Byte(0x82, 0xff);
    Write1Byte(0x12, Read8bit(0x12) | 0x4D);
    Write1Byte(0x36, 0x0C);
    Write1Byte(0x91, 0xF0);
    Write1Byte(0x90, 0x02);
    Write1Byte(0x30, 0x80);
    Write1Byte(0x39, 0xfc);
    Write1Byte(0x35, 0xa2);
    Write1Byte(0x32, 0x46);
    Write1Byte(0x28, 0xec);
}

void Loop() {
}
}  // namespace AXP192
