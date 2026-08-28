#ifdef SENSORS
#include "SHT.h"

#include "I2C.h"
#include "Settings.h"
#include "defaults.h"
#include "globals.h"
#include "mqtt.h"

namespace SHT {
int bus = -1;
unsigned long lastRead = 0;
unsigned long sensorInterval = 60000;
bool initialized = false;

// arduino-sht auto-detect tried SHT3x then SHT4x on 0x44/0x45 and used SHT_ACCURACY_MEDIUM.
// ponytail: SHTC1/SHTC3/SHTW (0x70) auto-detection dropped; nobody wired those to ESPresense.
enum Kind { NONE, SHT3X, SHT4X };
static Kind kind = NONE;
static uint8_t addr = 0;

static uint8_t crc8(const uint8_t* d, int n) {
    uint8_t crc = 0xFF;
    for (int i = 0; i < n; i++) {
        crc ^= d[i];
        for (int b = 0; b < 8; b++) crc = (crc & 0x80) ? (uint8_t)((crc << 1) ^ 0x31) : (uint8_t)(crc << 1);
    }
    return crc;
}

static bool cmd16(uint16_t c) {
    uint8_t b[2] = {(uint8_t)(c >> 8), (uint8_t)c};
    return I2C::write(bus, addr, b, 2);
}

// Read 6 bytes (T msb, lsb, crc, RH msb, lsb, crc) and CRC-check both words.
static bool readWords(uint16_t& t, uint16_t& rh) {
    uint8_t d[6];
    if (!I2C::read(bus, addr, d, 6)) return false;
    if (crc8(d, 2) != d[2] || crc8(d + 3, 2) != d[5]) return false;
    t = (d[0] << 8) | d[1];
    rh = (d[3] << 8) | d[4];
    return true;
}

static bool detectSHT3x(uint8_t a) {
    addr = a;
    if (!cmd16(0xF32D)) return false;  // read status register
    uint8_t d[3];
    return I2C::read(bus, addr, d, 3) && crc8(d, 2) == d[2];
}

static bool detectSHT4x(uint8_t a) {
    addr = a;
    uint8_t c = 0x89;  // read serial number
    if (!I2C::write(bus, addr, &c, 1)) return false;
    delay(1);
    uint8_t d[6];
    return I2C::read(bus, addr, d, 6) && crc8(d, 2) == d[2] && crc8(d + 3, 2) == d[5];
}

void Setup() {
    if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
    if (bus < 1 || bus > 2) return;

    if (detectSHT3x(0x44) || detectSHT3x(0x45)) kind = SHT3X;
    else if (detectSHT4x(0x44) || detectSHT4x(0x45)) kind = SHT4X;
    initialized = kind != NONE;

    if (!initialized) {
        Log.println("[SHT] Couldn't find a sensor, check your wiring and I2C address!");
    }
}

void ConnectToWifi(bool updating) {
    bus = Settings::integer("SHT_I2c_Bus", 1, 2, -1, "I2C Bus (-1 to disable)");
}

void SerialReport() {
    if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
    if (bus < 1 || bus > 2) return;
    Log.println("SHT:          Auto-detect on bus " + toStr(bus));
}

static bool readSample(float& temperature, float& humidity) {
    uint16_t t, rh;
    if (kind == SHT3X) {
        if (!cmd16(0x240B)) return false;  // single shot, medium repeatability, no clock stretch
        delay(15);
        if (!readWords(t, rh)) return false;
        temperature = -45.0f + 175.0f * t / 65535.0f;
        humidity = 100.0f * rh / 65535.0f;
        return true;
    }
    if (kind == SHT4X) {
        uint8_t c = 0xF6;  // medium precision measurement
        if (!I2C::write(bus, addr, &c, 1)) return false;
        delay(10);
        if (!readWords(t, rh)) return false;
        temperature = -45.0f + 175.0f * t / 65535.0f;
        humidity = -6.0f + 125.0f * rh / 65535.0f;
        return true;
    }
    return false;
}

void Loop() {
    if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
    if (!initialized) return;

    if (lastRead == 0 || millis() - lastRead >= sensorInterval) {
        lastRead = millis();

        float temperature, humidity;
        if (readSample(temperature, humidity)) {
            pub((roomsTopic + "/temperature").c_str(), 0, 1, toStr(temperature).c_str());
            pub((roomsTopic + "/humidity").c_str(), 0, 1, toStr(humidity).c_str());
        }
    }
}

bool SendDiscovery() {
    if (bus < 1 || bus > 2) return true;
    return sendSensorDiscovery("Temperature", EC_NONE, "temperature", "°C") && sendSensorDiscovery("Humidity", EC_NONE, "humidity", "%");
}
}  // namespace SHT

#endif
