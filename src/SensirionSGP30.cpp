#ifdef SENSORS
#include "SensirionSGP30.h"

#include <AsyncWiFiSettings.h>
#include <SparkFun_SGP30_Arduino_Library.h>

#include "defaults.h"
#include "globals.h"
#include "mqtt.h"
#include "string_utils.h"

namespace SensirionSGP30 {
SGP30* sgp;
long SGP30_status;
String SGP30_I2c;
int SGP30_I2c_Bus;

unsigned long SGP30PreviousSensorMillis = 0;
unsigned long SGP30PreviousReportMillis = 0;

int sensorInterval = 1000;  // SGP30/40/41 are designed to operate at 1Hz: so pull every second
int reportInterval = 60000; // Report every minute to MQTT (to avoid flooding)
bool initialized = false;

void Setup() {
    if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;

    sgp = new SGP30();
    if (SGP30_I2c == "0x58") {
        SGP30_status = sgp->begin(SGP30_I2c_Bus == 1 ? Wire : Wire1);
    } else {
        return;
    }

    if (!SGP30_status) {
        Serial.println("[SGP30] Couldn't find a sensor, check your wiring and I2C address!");
    } else {
        sgp->initAirQuality();
        initialized = true;
    }
}

void ConnectToWifi() {
    SGP30_I2c_Bus = AsyncWiFiSettings.integer("SGP30_I2c_Bus", 1, 2, DEFAULT_I2C_BUS, "I2C Bus");
    SGP30_I2c = AsyncWiFiSettings.string("SGP30_I2c", "", "I2C address (0x58)");
}

void SerialReport() {
    if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
    if (SGP30_I2c.isEmpty()) return;
    Serial.print("SGP30:        ");
    Serial.println(SGP30_I2c + " on bus " + SGP30_I2c_Bus);
}

void Loop() {
    if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
    if (!initialized) return;

    if (SGP30PreviousSensorMillis == 0 || millis() - SGP30PreviousSensorMillis >= sensorInterval) {
        SGP30PreviousSensorMillis = millis();

        sgp->measureAirQuality();
        float co2 = sgp->CO2;
        float tvoc = sgp->TVOC;
 
        if (SGP30PreviousSensorMillis > 30000) {  // First 30 seconds after boot, don't report
            if (SGP30PreviousReportMillis == 0 || millis() - SGP30PreviousReportMillis >= reportInterval) {  
                SGP30PreviousReportMillis = millis();
                
                pub((roomsTopic + "/co2").c_str(), 0, 1, String(co2).c_str());
                pub((roomsTopic + "/tvoc").c_str(), 0, 1, String(tvoc).c_str());
            }
        }
    }
} 

bool SendDiscovery() {
    if (SGP30_I2c.isEmpty()) return true;

    return sendSensorDiscovery("Co2", EC_NONE, "carbon_dioxide", "ppm") && sendSensorDiscovery("TVOC", EC_NONE, "volatile_organic_compounds_parts", "ppb");
}

}  // namespace SensirionSGP30

#endif
