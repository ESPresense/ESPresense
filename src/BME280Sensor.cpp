#include "defaults.h"
#include "TSL2561Sensor.h"
#include <WiFiSettings.h>
#include <AsyncMqttClient.h>
#include <Adafruit_BME280.h>

// for #define ESPMAC
#include "strings.h"

// TODO: Not a fan of externs, but this helps refactoring for now
extern bool I2C_Bus_1_Enabled;
extern bool I2C_Bus_2_Enabled;
extern unsigned long sensorInterval;

extern char buffer[2048];
extern String room;
extern String roomsTopic;
extern void commonDiscovery();
extern bool pub(const char *topic, uint8_t qos, bool retain, const char *payload, size_t length = 0, bool dup = false, uint16_t message_id = 0);

namespace BME280
{
    Adafruit_BME280 BME280;
    long BME280_status;
    String BME280_I2c;
    int BME280_I2c_Bus;
    unsigned long bme280PreviousMillis = 0;

    void Setup()
    {
        
    }

    void ConnectToWifi()
    {
        WiFiSettings.html("h4", "BME280 - Weather Sensor:");
        BME280_I2c_Bus = WiFiSettings.integer("BME280_I2c_Bus", 1, 2, DEFAULT_I2C_BUS, "I2C Bus");
        BME280_I2c = WiFiSettings.string("BME280_I2c", "", "I2C address (0x76 or 0x77)");
    }

    void SerialReport()
    {
        Serial.print("BME280_I2c Sensor: ");
        Serial.println(BME280_I2c + " on bus " + BME280_I2c_Bus);
    }

    void Loop(AsyncMqttClient& mqttClient)
    {
        if (!I2C_Bus_1_Enabled && !I2C_Bus_2_Enabled) return;

        // TODO: This should move to setup
        if (BME280_I2c == "0x76" && BME280_I2c_Bus == 1) {
            BME280_status = BME280.begin(0x76, &Wire);
        } else if (BME280_I2c == "0x77" && BME280_I2c_Bus == 1) {
            BME280_status = BME280.begin(0x77, &Wire);
        } else if (BME280_I2c == "0x76" && BME280_I2c_Bus == 2) {
            BME280_status = BME280.begin(0x76, &Wire1);
        } else if (BME280_I2c == "0x77" && BME280_I2c_Bus == 2) {
            BME280_status = BME280.begin(0x77, &Wire1);
        } else {
            return;
        }

        if (!BME280_status) {
            Serial.println("[BME280] Couldn't find a sensor, check your wiring and I2C address!");
        }

        BME280.setSampling(
            Adafruit_BME280::MODE_NORMAL,
            Adafruit_BME280::SAMPLING_X16,  // Temperature
            Adafruit_BME280::SAMPLING_X16,  // Pressure
            Adafruit_BME280::SAMPLING_X16,  // Humidity
            Adafruit_BME280::FILTER_X16,
            //Adafruit_BME280::FILTER_OFF,
            Adafruit_BME280::STANDBY_MS_1000
        );

        float temperature = BME280.readTemperature();
        float humidity = BME280.readHumidity();
        float pressure = BME280.readPressure() / 100.0F;

        if (millis() - bme280PreviousMillis >= sensorInterval) {

            mqttClient.publish((roomsTopic + "/bme280_temperature").c_str(), 0, 1, String(temperature).c_str());
            mqttClient.publish((roomsTopic + "/bme280_humidity").c_str(), 0, 1, String(humidity).c_str());
            mqttClient.publish((roomsTopic + "/bme280_pressure").c_str(), 0, 1, String(pressure).c_str());

            bme280PreviousMillis = millis();
        }
    }

    static bool SendTemperature(DynamicJsonDocument& doc)
    {
        commonDiscovery();
        doc["~"] = roomsTopic;
        doc["name"] = "ESPresense " + room + " BME280 Temperature";
        doc["uniq_id"] = Sprintf("espresense_%06" PRIx64 "_bme280_temperature", ESP.getEfuseMac() >> 24);
        doc["avty_t"] = "~/status";
        doc["stat_t"] = "~/bme280_temperature";
        doc["dev_cla"] = "temperature";
        doc["unit_of_meas"] = "°C";
        doc["frc_upd"] = true;

        serializeJson(doc, buffer);
        String discoveryTopic = "homeassistant/sensor/espresense_" + ESPMAC + "/bme280_temperature/config";

        return pub(discoveryTopic.c_str(), 0, true, buffer);
    }

    static bool SendHumidity(DynamicJsonDocument& doc)
    {
        commonDiscovery();
        doc["~"] = roomsTopic;
        doc["name"] = "ESPresense " + room + " BME280 Humidity";
        doc["uniq_id"] = Sprintf("espresense_%06" PRIx64 "_bme280_humidity", ESP.getEfuseMac() >> 24);
        doc["avty_t"] = "~/status";
        doc["stat_t"] = "~/bme280_humidity";
        doc["dev_cla"] = "humidity";
        doc["unit_of_meas"] = "%";
        doc["frc_upd"] = true;

        serializeJson(doc, buffer);
        String discoveryTopic = "homeassistant/sensor/espresense_" + ESPMAC + "/bme280_humidity/config";

        return pub(discoveryTopic.c_str(), 0, true, buffer);
    }

    static bool SendPressure(DynamicJsonDocument& doc)
    {
        commonDiscovery();
        doc["~"] = roomsTopic;
        doc["name"] = "ESPresense " + room + " BME280 Pressure";
        doc["uniq_id"] = Sprintf("espresense_%06" PRIx64 "_bme280_pressure", ESP.getEfuseMac() >> 24);
        doc["avty_t"] = "~/status";
        doc["stat_t"] = "~/bme280_pressure";
        doc["dev_cla"] = "pressure";
        doc["unit_of_meas"] = "hPa";
        doc["frc_upd"] = true;

        serializeJson(doc, buffer);
        String discoveryTopic = "homeassistant/sensor/espresense_" + ESPMAC + "/bme280_pressure/config";

        return pub(discoveryTopic.c_str(), 0, true, buffer);
    }

    bool SendDiscovery(DynamicJsonDocument& doc)
    {
        if (BME280_I2c.isEmpty()) return true;

        return SendTemperature(doc) && SendHumidity(doc) && SendPressure(doc);
    }
}