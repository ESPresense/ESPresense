
#include <Arduino.h>
#include <Wire.h>

namespace AXP192 {

uint8_t Read8bit(uint8_t Addr) {
    Wire1.beginTransmission(0x34);
    Wire1.write(Addr);
    Wire1.endTransmission();
    Wire1.requestFrom(0x34, 1);
    return Wire1.read();
}

void Write1Byte(uint8_t Addr, uint8_t Data) {
    Wire1.beginTransmission(0x34);
    Wire1.write(Addr);
    Wire1.write(Data);
    Wire1.endTransmission();
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
    Wire1.begin(21, 22);
    Wire1.setClock(400000);
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
