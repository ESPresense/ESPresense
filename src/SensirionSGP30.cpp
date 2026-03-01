#ifdef SENSORS
#include "SensirionSGP30.h"

#include <HeadlessWiFiSettings.h>
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

/**
 * @brief Initialize and configure the SGP30 air-quality sensor if an I2C bus is active.
 *
 * Attempts to instantiate the SGP30 object and begin communication on the configured
 * I2C bus/address. If the sensor is found, starts air-quality measurement and marks
 * the sensor as initialized; if not found, logs an error message.
 *
 * @details
 * - No action is taken if neither I2C bus is started or if the configured I2C address
 *   is not "0x58".
 * - On successful initialization, calls sensor startup routine and sets the
 *   module-level `initialized` flag to true.
 * - On failure to begin communication, leaves `initialized` false and logs a diagnostic message.
 */
void Setup() {
    if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;

    sgp = new SGP30();
    if (SGP30_I2c == "0x58") {
#if SOC_I2C_NUM > 1
        SGP30_status = sgp->begin(SGP30_I2c_Bus == 1 ? Wire : Wire1);
#else
        SGP30_status = sgp->begin(Wire);
#endif
    } else {
        return;
    }

    if (!SGP30_status) {
        Log.println("[SGP30] Couldn't find a sensor, check your wiring and I2C address!");
    } else {
        sgp->initAirQuality();
        initialized = true;
    }
}

void ConnectToWifi() {
    SGP30_I2c_Bus = HeadlessWiFiSettings.integer("SGP30_I2c_Bus", 1, 2, DEFAULT_I2C_BUS, "I2C Bus");
    SGP30_I2c = HeadlessWiFiSettings.string("SGP30_I2c", "", "I2C address (0x58)");
}

/**
 * @brief Logs the configured SGP30 I2C address and bus to the system log.
 *
 * If neither I2C bus has started or the configured I2C address string is empty, the function returns without logging.
 */
void SerialReport() {
    if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
    if (SGP30_I2c.isEmpty()) return;
    Log.print("SGP30:        ");
    Log.println(SGP30_I2c + " on bus " + SGP30_I2c_Bus);
}

/**
 * @brief Performs periodic SGP30 measurements and publishes CO2 and TVOC readings at configured intervals after startup.
 *
 * Reads the SGP30 sensor at the cadence defined by `sensorInterval` and, once more than 30 seconds have elapsed since boot,
 * publishes CO2 and TVOC values to MQTT topics when the `reportInterval` has elapsed or for the first report.
 *
 * The function returns immediately if neither I2C bus has started or if the sensor was not initialized. It updates
 * `SGP30PreviousSensorMillis` on each measurement and `SGP30PreviousReportMillis` when a report is published. Published
 * topics are `roomsTopic + "/co2"` and `roomsTopic + "/tvoc"`.
 */
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