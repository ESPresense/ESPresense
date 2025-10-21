#ifdef SENSORS
#include "SHT.h"

#include <HeadlessWiFiSettings.h>
#include <SHTSensor.h>

#include "defaults.h"
#include "globals.h"
#include "mqtt.h"
#include "string_utils.h"

namespace SHT {
SHTSensor* sensor;
int bus = -1;
unsigned long lastRead = 0;
int sensorInterval = 60000;
bool initialized = false;

/**
 * @brief Initializes the SHT temperature/humidity sensor if an I2C bus is available and configured.
 *
 * Attempts to create and initialize the sensor on the selected I2C bus (1 or 2). On successful
 * initialization stores the sensor instance, marks the module initialized, and sets the sensor's
 * accuracy to a medium preset. If initialization fails, logs an error indicating the sensor could
 * not be found.
 */
void Setup() {
    if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
    if (bus < 1 || bus > 2) return;
    sensor = new SHTSensor();
    initialized = sensor->init(bus == 1 ? Wire : Wire1);

    if (!initialized) {
        Log.println("[SHT] Couldn't find a sensor, check your wiring and I2C address!");
    } else {
        sensor->setAccuracy(SHTSensor::SHT_ACCURACY_MEDIUM);
    }
}

void ConnectToWifi() {
    bus = HeadlessWiFiSettings.integer("SHT_I2c_Bus", 1, 2, -1, "I2C Bus (-1 to disable)");
}

/**
 * @brief Logs a diagnostic message about SHT auto-detection on the configured I2C bus.
 *
 * If neither I2C bus has started or the configured bus is not 1 or 2, the function does nothing.
 */
void SerialReport() {
    if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
    if (bus < 1 || bus > 2) return;
    Log.println("SHT:          Auto-detect on bus " + String(bus));
}

/**
 * @brief Performs periodic sensor polling and publishes temperature and humidity readings when available.
 *
 * This is the main per-loop handler for the SHT sensor: it does nothing unless an I2C bus has started and the
 * sensor was initialized. When the configured interval has elapsed since the last read, it updates the read
 * timestamp and, if a new sample is available, publishes the temperature to `roomsTopic + "/temperature"` and
 * the humidity to `roomsTopic + "/humidity"`.
 */
void Loop() {
    if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
    if (!initialized) return;

    if (lastRead == 0 || millis() - lastRead >= sensorInterval) {
        lastRead = millis();

        if (sensor->readSample()) {
            pub((roomsTopic + "/temperature").c_str(), 0, 1, String(sensor->getTemperature()).c_str());
            pub((roomsTopic + "/humidity").c_str(), 0, 1, String(sensor->getHumidity()).c_str());
        }
    }
}

bool SendDiscovery() {
    if (bus < 1 || bus > 2) return true;
    return sendSensorDiscovery("Temperature", EC_NONE, "temperature", "°C") && sendSensorDiscovery("Humidity", EC_NONE, "humidity", "%");
}
}  // namespace SHT

#endif