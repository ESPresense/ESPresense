#ifdef SENSORS

#include "defaults.h"
#include "TSL2561Sensor.h"
#include <WiFiSettings.h>
#include <AsyncMqttClient.h>
#include <Adafruit_TSL2561_U.h>

// for #define ESPMAC
#include "string_utils.h"

// TODO: Not a fan of externs, but this helps refactoring for now
extern bool I2C_Bus_1_Enabled;
extern bool I2C_Bus_2_Enabled;
extern unsigned long sensorInterval;

extern char buffer[2048];
extern String room;
extern String roomsTopic;
extern void commonDiscovery();
extern bool pub(const char *topic, uint8_t qos, bool retain, const char *payload, size_t length = 0, bool dup = false, uint16_t message_id = 0);

namespace TSL2561
{
    String TSL2561_I2c;
    int TSL2561_I2c_Bus;
    String TSL2561_I2c_Gain;
    unsigned long tsl2561PreviousMillis = 0;

    void Setup()
    {
    }

    void ConnectToWifi()
    {
        WiFiSettings.html("h4", "TSL2561 - Ambient Light Sensor:");
        TSL2561_I2c_Bus = WiFiSettings.integer("TSL2561_I2c_Bus", 1, 2, DEFAULT_I2C_BUS, "I2C Bus");
        TSL2561_I2c = WiFiSettings.string("TSL2561_I2c", "", "I2C address (0x39, 0x49 or 0x29)");
        TSL2561_I2c_Gain = WiFiSettings.string("TSL2561_I2c_Gain", DEFAULT_TSL2561_I2C_GAIN, "Gain (auto, 1x or 16x)");
    }

    void SerialReport()
    {
        Serial.print("TSL2561_I2c Sensor: ");
        Serial.println(TSL2561_I2c + " on bus " + TSL2561_I2c_Bus);
    }

    void Loop(AsyncMqttClient& mqttClient)
    {
        if (!I2C_Bus_1_Enabled && !I2C_Bus_2_Enabled) return;

        // TODO: This should move to setup
        int tsl2561_address;

        if (TSL2561_I2c == "0x39") {
            tsl2561_address = 0x39;
        } else if (TSL2561_I2c == "0x29") {
            tsl2561_address = 0x29;
        } else if (TSL2561_I2c == "0x49") {
            tsl2561_address = 0x49;
        } else {
            return;
        }

        Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(tsl2561_address);

        if (TSL2561_I2c_Bus == 1) {
            tsl.begin(&Wire);
        } else if (TSL2561_I2c_Bus == 2) {
            tsl.begin(&Wire1);
        }

        if (TSL2561_I2c_Gain == "auto") {
            tsl.enableAutoRange(true);
        } else if (TSL2561_I2c_Gain == "1x") {
            tsl.setGain(TSL2561_GAIN_1X);
        } else if (TSL2561_I2c_Gain == "16x") {
            tsl.setGain(TSL2561_GAIN_16X);
        } else {
            Serial.println("[TSL2561] Invalid gain");
            return;
        }

        tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_402MS);

        sensors_event_t event;
        tsl.getEvent(&event);

        if (event.light) {
            if (millis() - tsl2561PreviousMillis >= sensorInterval) {
                mqttClient.publish((roomsTopic + "/tsl2561_lux").c_str(), 0, 1, String(event.light).c_str());

                tsl2561PreviousMillis = millis();
            }
        } else {
            Serial.println("[TSL2561] Sensor overloaded");
        }
    }

    bool SendDiscovery(DynamicJsonDocument& doc)
    {
        if (TSL2561_I2c.isEmpty()) return true;

        commonDiscovery();
        doc["~"] = roomsTopic;
        doc["name"] = "ESPresense " + room + " TSL2561 Lux";
        doc["uniq_id"] = Sprintf("espresense_%06" PRIx64 "_tsl2561_lux", ESP.getEfuseMac() >> 24);
        doc["avty_t"] = "~/status";
        doc["stat_t"] = "~/tsl2561_lux";
        doc["dev_cla"] = "illuminance";
        doc["unit_of_meas"] = "lx";
        doc["frc_upd"] = true;

        serializeJson(doc, buffer);
        String discoveryTopic = "homeassistant/sensor/espresense_" + ESPMAC + "/tsl2561_lux/config";

        return pub(discoveryTopic.c_str(), 0, true, buffer);
    }
}

#endif
