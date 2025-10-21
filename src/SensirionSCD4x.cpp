#ifdef SENSORS
#include "SensirionSCD4x.h"

#include <HeadlessWiFiSettings.h>
#include <SensirionI2cScd4x.h>

#include "defaults.h"
#include "globals.h"
#include "mqtt.h"
#include "string_utils.h"

#ifdef NO_ERROR
#undef NO_ERROR
#endif
#define NO_ERROR 0

namespace SensirionSCD4x {
SensirionI2cScd4x* scd;
long SCD4x_status;
String SCD4x_I2c;
int SCD4x_I2c_Bus;

unsigned long SCD4xPreviousSensorMillis = 0;
unsigned long SCD4xPreviousReportMillis = 0;

int sensorInterval = 5000;  // SCD40/41 are designed to operate at 0.2Hz: so pull every five seconds
int reportInterval = 60000; // Report every minute to MQTT (to avoid flooding)
bool initialized = false;

void Setup() {
    if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;

    scd = new SensirionI2cScd4x();
    if (SCD4x_I2c == "0x62") {
        scd->begin(SCD4x_I2c_Bus == 1 ? Wire : Wire1, SCD41_I2C_ADDR_62);
    } else {
        return;
    }

    SCD4x_status = scd->wakeUp();
    if (SCD4x_status != NO_ERROR) {
        Log.println("[SCD4x] Error trying to execute wakeUp(): ");
    }
    SCD4x_status = scd->stopPeriodicMeasurement();
    if (SCD4x_status != NO_ERROR) {
        Log.println("[SCD4x] Error trying to execute stopPeriodicMeasurement(): ");
    }
    SCD4x_status = scd->reinit();
    if (SCD4x_status != NO_ERROR) {
        Log.println("[SCD4x] Error trying to execute reinit(): ");
    }

    uint64_t serialNumber = 0;

    // Read out information about the sensor
    SCD4x_status = scd->getSerialNumber(serialNumber);
    if (SCD4x_status != NO_ERROR) {
        Log.println("[SCD4x] Error trying to execute getSerialNumber(): ");
    } else {
        Log.print("SCD4x sn:     ");
        Log.print("0x");
        Log.print((uint32_t)(serialNumber >> 32), HEX);
        Log.print((uint32_t)(serialNumber & 0xFFFFFFFF), HEX);
        Log.println();
    }

    SCD4x_status = scd->startPeriodicMeasurement();
    if (SCD4x_status != NO_ERROR) {
        Log.println("[SCD4x] Error trying to execute startPeriodicMeasurement(): ");
        Log.println("[SCD4x] Couldn't find a sensor, check your wiring and I2C address!");
    } else {
        initialized = true;
    }
}

void ConnectToWifi() {
    SCD4x_I2c_Bus = HeadlessWiFiSettings.integer("SCD4x_I2c_Bus", 1, 2, DEFAULT_I2C_BUS, "I2C Bus");
    SCD4x_I2c = HeadlessWiFiSettings.string("SCD4x_I2c", "", "I2C address (0x62)");
}

void SerialReport() {
    if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
    if (SCD4x_I2c.isEmpty()) return;
    Log.print("SCD4x:        ");
    Log.println(SCD4x_I2c + " on bus " + SCD4x_I2c_Bus);
}

void Loop() {
    if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
    if (!initialized) return;

    if (SCD4xPreviousSensorMillis == 0 || millis() - SCD4xPreviousSensorMillis >= sensorInterval) {
        SCD4xPreviousSensorMillis = millis();

        bool dataReady = false;
        uint16_t co2Concentration = 0;
        float humidity;
        float temperature;
        SCD4x_status = scd->getDataReadyStatus(dataReady);
        if (SCD4x_status != NO_ERROR) {
             return;
        }

        if (!dataReady) {
            return; // Data not ready yet
        }

        SCD4x_status = scd->readMeasurement(co2Concentration, temperature, humidity);
        if (SCD4x_status != NO_ERROR) {
            return;
        }

        if (SCD4xPreviousSensorMillis > 30000) {  // First 30 seconds after boot, don't report
            if (SCD4xPreviousReportMillis == 0 || millis() - SCD4xPreviousReportMillis >= reportInterval) {
                SCD4xPreviousReportMillis = millis();

                pub((roomsTopic + "/co2").c_str(), 0, 1, String(co2Concentration).c_str());
                pub((roomsTopic + "/humidity").c_str(), 0, 1, String(humidity).c_str());
                pub((roomsTopic + "/temperature").c_str(), 0, 1, String(temperature).c_str());
            }
        }
    }
}

bool SendDiscovery() {
    if (SCD4x_I2c.isEmpty()) return true;

    return sendSensorDiscovery("Co2", EC_NONE, "carbon_dioxide", "ppm") && sendSensorDiscovery("Temperature", EC_NONE, "temperature", "°C") && sendSensorDiscovery("Humidity", EC_NONE, "humidity", "%");
}

}  // namespace SensirionSCD4x

#endif
