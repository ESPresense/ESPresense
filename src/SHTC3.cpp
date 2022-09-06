#ifdef SENSORS
#include "SHTC3.h"

#include <Adafruit_SHTC3.h>
#include <AsyncWiFiSettings.h>

#include "defaults.h"
#include "globals.h"
#include "mqtt.h"
#include "string_utils.h"

namespace SHTC3 {
Adafruit_SHTC3* sensor;
int bus = -1;
unsigned long lastRead = 0;
int sensorInterval = 60000;
bool initialized = false;

void Setup() {
    if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
    if (bus < 1 || bus > 2) return;
    sensor = new Adafruit_SHTC3();
    initialized = sensor->begin(bus == 1 ? &Wire : &Wire1);

    if (!initialized) {
        Serial.println("[SHTC3] Couldn't find a sensor, check your wiring and I2C address!");
    }
}

void ConnectToWifi() {
    AsyncWiFiSettings.html("h4", "SHTC3 - Temperature and Humidity Sensor:");
    bus = AsyncWiFiSettings.integer("SHTC3_I2c_Bus", 1, 2, -1, "I2C Bus (-1 to disable)");
}

void SerialReport() {
    if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
    if (bus < 1 || bus > 2) return;
    Serial.println("SHTC3:        0x70 on bus " + String(bus));
}

void Loop() {
    if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
    if (!initialized) return;

    if (lastRead == 0 || millis() - lastRead >= sensorInterval) {
        lastRead = millis();

        sensors_event_t humidity_t, temp_t;
        sensor->getEvent(&humidity_t, &temp_t);
        float temperature = temp_t.temperature;
        float humidity = humidity_t.relative_humidity;

        mqttClient.publish((roomsTopic + "/shtc3_temperature").c_str(), 0, 1, String(temperature).c_str());
        mqttClient.publish((roomsTopic + "/shtc3_humidity").c_str(), 0, 1, String(humidity).c_str());
    }
}

bool SendDiscovery() {
    if (bus < 1 || bus > 2) return true;
    return sendSensorDiscovery("SHTC3 Temperature", EC_NONE, "temperature", "°C") && sendSensorDiscovery("SHTC3 Humidity", EC_NONE, "humidity", "%");
}
}  // namespace SHTC3

#endif
