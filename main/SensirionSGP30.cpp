#ifdef SENSORS
#include "SensirionSGP30.h"

#include "I2C.h"
#include "Settings.h"
#include "defaults.h"
#include "globals.h"
#include "mqtt.h"

// Raw I2C port of the SparkFun SGP30 usage: Init_air_quality once, Measure_air_quality at 1 Hz.
namespace SensirionSGP30 {
static const uint8_t SGP30_ADDR = 0x58;
long SGP30_status;
std::string SGP30_I2c;
int SGP30_I2c_Bus;

unsigned long SGP30PreviousSensorMillis = 0;
unsigned long SGP30PreviousReportMillis = 0;

int sensorInterval = 1000;   // SGP30/40/41 are designed to operate at 1Hz: so pull every second
int reportInterval = 60000;  // Report every minute to MQTT (to avoid flooding)
bool initialized = false;

static uint8_t crc8(const uint8_t* data, size_t len) {
    uint8_t crc = 0xFF;
    for (size_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (int b = 0; b < 8; b++) crc = (crc & 0x80) ? (uint8_t)((crc << 1) ^ 0x31) : (uint8_t)(crc << 1);
    }
    return crc;
}

static bool sendCommand(uint16_t cmd) {
    uint8_t buf[2] = {(uint8_t)(cmd >> 8), (uint8_t)cmd};
    return I2C::write(SGP30_I2c_Bus, SGP30_ADDR, buf, 2);
}

// Read `count` CRC-protected 16-bit words.
static bool readWords(uint16_t* words, size_t count) {
    uint8_t buf[3 * 4];
    if (count > 4) return false;
    if (!I2C::read(SGP30_I2c_Bus, SGP30_ADDR, buf, count * 3)) return false;
    for (size_t i = 0; i < count; i++) {
        if (crc8(&buf[i * 3], 2) != buf[i * 3 + 2]) return false;
        words[i] = (uint16_t)((buf[i * 3] << 8) | buf[i * 3 + 1]);
    }
    return true;
}

void Setup() {
    if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
    if (SGP30_I2c != "0x58") return;

    // Get_serial_id doubles as the presence check (what SparkFun's begin() does).
    uint16_t serial[3];
    SGP30_status = sendCommand(0x3682);
    delay(1);
    SGP30_status = SGP30_status && readWords(serial, 3);

    if (!SGP30_status) {
        Log.println("[SGP30] Couldn't find a sensor, check your wiring and I2C address!");
    } else {
        sendCommand(0x2003);  // Init_air_quality
        delay(10);
        initialized = true;
    }
}

void ConnectToWifi(bool updating) {
    SGP30_I2c_Bus = Settings::integer("SGP30_I2c_Bus", 1, 2, DEFAULT_I2C_BUS, "I2C Bus");
    SGP30_I2c = Settings::string("SGP30_I2c", "", "I2C address (0x58)");
}

void SerialReport() {
    if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
    if (SGP30_I2c.empty()) return;
    Log.print("SGP30:        ");
    Log.println(SGP30_I2c + " on bus " + toStr(SGP30_I2c_Bus));
}

void Loop() {
    if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
    if (!initialized) return;

    if (SGP30PreviousSensorMillis == 0 || millis() - SGP30PreviousSensorMillis >= (unsigned long)sensorInterval) {
        SGP30PreviousSensorMillis = millis();

        // Measure_air_quality: 12 ms max, then eCO2 (ppm) + TVOC (ppb)
        if (!sendCommand(0x2008)) return;
        delay(12);
        uint16_t words[2];
        if (!readWords(words, 2)) return;
        float co2 = words[0];
        float tvoc = words[1];

        if (SGP30PreviousSensorMillis > 30000) {  // First 30 seconds after boot, don't report
            if (SGP30PreviousReportMillis == 0 || millis() - SGP30PreviousReportMillis >= (unsigned long)reportInterval) {
                SGP30PreviousReportMillis = millis();

                pub((roomsTopic + "/co2").c_str(), 0, 1, toStr(co2).c_str());
                pub((roomsTopic + "/tvoc").c_str(), 0, 1, toStr(tvoc).c_str());
            }
        }
    }
}

bool SendDiscovery() {
    if (SGP30_I2c.empty()) return true;

    return sendSensorDiscovery("Co2", EC_NONE, "carbon_dioxide", "ppm") && sendSensorDiscovery("TVOC", EC_NONE, "volatile_organic_compounds_parts", "ppb");
}

}  // namespace SensirionSGP30

#endif
