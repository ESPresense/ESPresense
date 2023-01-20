#ifdef SENSORS
#include "SensirionSCD4X.h"

#include <AsyncWiFiSettings.h>
#include <SensirionI2CScd4x.h>

#include "defaults.h"
#include "globals.h"
#include "mqtt.h"
#include "string_utils.h"

namespace SensirionSCD4X {
SensirionI2CScd4x scd4x;
long SCD4X_status;
String SCD4X_I2c;
int SCD4X_I2c_Bus;
unsigned long SCD4XPreviousMillis = 0;
int sensorInterval = 1000;
bool initialized = false;

void printUint16Hex(uint16_t value) {
    Serial.print(value < 4096 ? "0" : "");
    Serial.print(value < 256 ? "0" : "");
    Serial.print(value < 16 ? "0" : "");
    Serial.print(value, HEX);
}

void printSerialNumber(uint16_t serial0, uint16_t serial1, uint16_t serial2) {
    Serial.print("Serial: 0x");
    printUint16Hex(serial0);
    printUint16Hex(serial1);
    printUint16Hex(serial2);
    Serial.println();
}

void Setup() {
    if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;

    uint16_t error;
    char errorMessage[256];
    scd4x.begin(Wire);

    // stop potentially previously started measurement
    error = scd4x.stopPeriodicMeasurement();
    if (error) {
        Serial.print("[SCD4X]  Error trying to execute stopPeriodicMeasurement(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    }
    uint16_t serial0;
    uint16_t serial1;
    uint16_t serial2;
    error = scd4x.getSerialNumber(serial0, serial1, serial2);
    if (error) {
        Serial.print("[SCD4X] Error trying to execute getSerialNumber(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    } else {
        printSerialNumber(serial0, serial1, serial2);
    }

    // Start Measurement
    error = scd4x.startPeriodicMeasurement();
    if (error) {
        Serial.print("[SCD4X] Error trying to execute startPeriodicMeasurement(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    } else {
        initialized = true;
    }

    Serial.println("[SCD4X] Waiting for first measurement... (5 sec)");

}

void ConnectToWifi() {
    AsyncWiFiSettings.html("h4", "SCD4X - Air Quality Sensor:");
    SCD4X_I2c_Bus = AsyncWiFiSettings.integer("SCD4X_I2c_Bus", 1, 2, DEFAULT_I2C_BUS, "I2C Bus");
    SCD4X_I2c = AsyncWiFiSettings.string("SCD4X_I2c", "", "I2C address (0x62)");
}

void SerialReport() {
    if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
    if (SCD4X_I2c.isEmpty()) return;
    Serial.print("SCD4X:        ");
    Serial.println(SCD4X_I2c + " on bus " + SCD4X_I2c_Bus);
}

void Loop() {
    uint16_t error;
    char errorMessage[256];
    if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
    if (!initialized) return;

    if (SCD4XPreviousMillis == 0 || millis() - SCD4XPreviousMillis >= sensorInterval) {
        SCD4XPreviousMillis = millis();
         // Read Measurement
    uint16_t co2 = 0;
    float temperature = 0.0f;
    float humidity = 0.0f;
    bool isDataReady = false;
    error = scd4x.getDataReadyFlag(isDataReady);
    if (error) {
        Serial.print("[SCD4X] Error trying to execute getDataReadyFlag(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
        return;
    }
    if (!isDataReady) {
        return;
    }
    error = scd4x.readMeasurement(co2, temperature, humidity);
    if (error) {
        Serial.print("[SCD4X] Error trying to execute readMeasurement(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    } else if (co2 == 0) {
        Serial.println("[SCD4X] Invalid sample detected, skipping.");
    } else {
        pub((roomsTopic + "/co2").c_str(), 0, 1, String(co2).c_str());
        pub((roomsTopic + "/humidity").c_str(), 0, 1, String(humidity, 1).c_str());
        pub((roomsTopic + "/temperature").c_str(), 0, 1, String(temperature, 1).c_str());
    }
    }
}

bool SendDiscovery() {
    if (SCD4X_I2c.isEmpty()) return true;
    return sendSensorDiscovery("Co2", EC_NONE, "carbon_dioxide", "ppm") && sendSensorDiscovery("Temperature", EC_NONE, "temperature", "°C") && sendSensorDiscovery("Humidity", EC_NONE, "humidity", "%");
}
}

#endif
