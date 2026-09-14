#ifdef SENSORS
#include "SensirionSCD4x.h"

#include "I2C.h"
#include "Settings.h"
#include "defaults.h"
#include "globals.h"
#include "mqtt.h"

// Raw I2C port of the Sensirion SCD4x usage (start periodic measurement, poll data-ready, read).
namespace SensirionSCD4x {
static const uint8_t SCD4x_ADDR = 0x62;
long SCD4x_status;
std::string SCD4x_I2c;
int SCD4x_I2c_Bus;

unsigned long SCD4xPreviousSensorMillis = 0;
unsigned long SCD4xPreviousReportMillis = 0;

int sensorInterval = 5000;   // SCD40/SCD41 sensors are designed to operate at 0.2Hz: so pull every five seconds
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

static bool sendCommand(uint16_t cmd, uint32_t execMs) {
    uint8_t buf[2] = {(uint8_t)(cmd >> 8), (uint8_t)cmd};
    bool ok = I2C::write(SCD4x_I2c_Bus, SCD4x_ADDR, buf, 2);
    if (execMs) delay(execMs);
    return ok;
}

// Read `count` CRC-protected 16-bit words.
static bool readWords(uint16_t* words, size_t count) {
    uint8_t buf[3 * 3];
    if (count > 3) return false;
    if (!I2C::read(SCD4x_I2c_Bus, SCD4x_ADDR, buf, count * 3)) return false;
    for (size_t i = 0; i < count; i++) {
        if (crc8(&buf[i * 3], 2) != buf[i * 3 + 2]) return false;
        words[i] = (uint16_t)((buf[i * 3] << 8) | buf[i * 3 + 1]);
    }
    return true;
}

void Setup() {
    if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
    if (SCD4x_I2c != "0x62") return;

    // wake_up: the sensor does not ACK this command, so the write failing is expected.
    // ponytail: only log it like the old driver did if the write itself errored.
    SCD4x_status = sendCommand(0x36F6, 30) ? 0 : 1;
    if (SCD4x_status) {
        Log.println("[SCD4x] Error trying to execute wakeUp(): ");
    }
    SCD4x_status = sendCommand(0x3F86, 500) ? 0 : 1;  // stop_periodic_measurement
    if (SCD4x_status) {
        Log.println("[SCD4x] Error trying to execute stopPeriodicMeasurement(): ");
    }
    SCD4x_status = sendCommand(0x3646, 30) ? 0 : 1;  // reinit
    if (SCD4x_status) {
        Log.println("[SCD4x] Error trying to execute reinit(): ");
    }

    // Read out information about the sensor
    uint16_t sn[3] = {0, 0, 0};
    SCD4x_status = (sendCommand(0x3682, 1) && readWords(sn, 3)) ? 0 : 1;  // get_serial_number
    if (SCD4x_status) {
        Log.println("[SCD4x] Error trying to execute getSerialNumber(): ");
    } else {
        uint64_t serialNumber = ((uint64_t)sn[0] << 32) | ((uint32_t)sn[1] << 16) | sn[2];
        Log.print("SCD4x sn:     ");
        Log.println(Sprintf("0x%x%x", (unsigned)(serialNumber >> 32), (unsigned)(serialNumber & 0xFFFFFFFF)));
    }

    SCD4x_status = sendCommand(0x21B1, 0) ? 0 : 1;  // start_periodic_measurement
    if (SCD4x_status) {
        Log.println("[SCD4x] Error trying to execute startPeriodicMeasurement(): ");
        Log.println("[SCD4x] Couldn't find a sensor, check your wiring and I2C address!");
    } else {
        initialized = true;
    }
}

void ConnectToWifi(bool updating) {
    SCD4x_I2c_Bus = Settings::integer("SCD4x_I2c_Bus", 1, 2, DEFAULT_I2C_BUS, "I2C Bus");
    SCD4x_I2c = Settings::string("SCD4x_I2c", "", "I2C address (0x62 for SCD40/SCD41/SCD43)");
}

void SerialReport() {
    if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
    if (SCD4x_I2c.empty()) return;
    Log.print("SCD4x:        ");
    Log.println(SCD4x_I2c + " on bus " + toStr(SCD4x_I2c_Bus));
}

void Loop() {
    if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
    if (!initialized) return;

    if (SCD4xPreviousSensorMillis == 0 || millis() - SCD4xPreviousSensorMillis >= (unsigned long)sensorInterval) {
        SCD4xPreviousSensorMillis = millis();

        // get_data_ready_status: ready when the low 11 bits are non-zero
        uint16_t ready;
        if (!sendCommand(0xE4B8, 1) || !readWords(&ready, 1)) {
            SCD4x_status = 1;
            return;
        }
        if ((ready & 0x07FF) == 0) {
            return;  // Data not ready yet
        }

        // read_measurement: CO2 ppm, T = -45 + 175 * raw / 65535, RH = 100 * raw / 65535
        uint16_t words[3];
        if (!sendCommand(0xEC05, 1) || !readWords(words, 3)) {
            SCD4x_status = 1;
            return;
        }
        SCD4x_status = 0;
        uint16_t co2Concentration = words[0];
        float temperature = -45.0f + 175.0f * (float)words[1] / 65535.0f;
        float humidity = 100.0f * (float)words[2] / 65535.0f;

        if (SCD4xPreviousSensorMillis > 30000) {  // First 30 seconds after boot, don't report
            if (SCD4xPreviousReportMillis == 0 || millis() - SCD4xPreviousReportMillis >= (unsigned long)reportInterval) {
                SCD4xPreviousReportMillis = millis();

                pub((roomsTopic + "/co2").c_str(), 0, 1, toStr((unsigned)co2Concentration).c_str());
                pub((roomsTopic + "/humidity").c_str(), 0, 1, toStr(humidity).c_str());
                pub((roomsTopic + "/temperature").c_str(), 0, 1, toStr(temperature).c_str());
            }
        }
    }
}

bool SendDiscovery() {
    if (SCD4x_I2c.empty()) return true;

    return sendSensorDiscovery("Co2", EC_NONE, "carbon_dioxide", "ppm") && sendSensorDiscovery("Temperature", EC_NONE, "temperature", "°C") && sendSensorDiscovery("Humidity", EC_NONE, "humidity", "%");
}

}  // namespace SensirionSCD4x

#endif
