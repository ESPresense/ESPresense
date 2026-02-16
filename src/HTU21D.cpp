#ifdef SENSORS
#include "HTU21D.h"

#include <HeadlessWiFiSettings.h>
#include <Adafruit_HTU21DF.h>

#include "defaults.h"
#include "globals.h"
#include "mqtt.h"

namespace HTU21D {
Adafruit_HTU21DF* sensor;
int bus = -1;
unsigned long lastRead = 0;
int sensorInterval = 60000;
bool initialized = false;

void ConnectToWifi() {
    bus = HeadlessWiFiSettings.integer("HTU21D_I2c_Bus", 1, 2, -1, "I2C Bus (-1 to disable)");
}

void SerialReport() {
    if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
    if (bus < 1 || bus > 2) return;
    Log.println("HTU21D:       Auto-detect on bus " + String(bus));
}

void Setup() {
    if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
    if (bus < 1 || bus > 2) return;

    sensor = new Adafruit_HTU21DF();
    initialized = sensor->begin(bus == 1 ? &Wire : &Wire1);

    if (!initialized) {
        Log.println("[HTU21D] Couldn't find a sensor, check your wiring and I2C bus!");
    }
}

void Loop() {
    if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
    if (!initialized) return;

    if (lastRead == 0 || millis() - lastRead >= sensorInterval) {
        lastRead = millis();

        float temperature = sensor->readTemperature();
        float humidity = sensor->readHumidity();

        if (!isnan(temperature)) {
            pub((roomsTopic + "/htu21d_temperature").c_str(), 0, 1, String(temperature).c_str());
        }
        if (!isnan(humidity)) {
            pub((roomsTopic + "/htu21d_humidity").c_str(), 0, 1, String(humidity).c_str());
        }
    }
}

bool SendDiscovery() {
    if (bus < 1 || bus > 2) return true;
    return sendSensorDiscovery("HTU21D Temperature", EC_NONE, "htu21d_temperature", "°C")
        && sendSensorDiscovery("HTU21D Humidity", EC_NONE, "htu21d_humidity", "%");
}
}  // namespace HTU21D

#endif
