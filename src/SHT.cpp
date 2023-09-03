#ifdef SENSORS
#include "SHT.h"

#include <AsyncWiFiSettings.h>
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

void Setup() {
    if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
    if (bus < 1 || bus > 2) return;
    sensor = new SHTSensor();
    initialized = sensor->init(bus == 1 ? Wire : Wire1);

    if (!initialized) {
        Serial.println("[SHT] Couldn't find a sensor, check your wiring and I2C address!");
    } else {
        sensor->setAccuracy(SHTSensor::SHT_ACCURACY_MEDIUM);
    }
}

void ConnectToWifi() {
    bus = AsyncWiFiSettings.integer("SHT_I2c_Bus", 1, 2, -1, "I2C Bus (-1 to disable)");
}

void SerialReport() {
    if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
    if (bus < 1 || bus > 2) return;
    Serial.println("SHT:          Auto-detect on bus " + String(bus));
}

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
