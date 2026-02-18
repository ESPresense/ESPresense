#ifdef SENSORS
#include "BME280.h"

#include "globals.h"
#include "mqtt.h"
#include "defaults.h"
#include <HeadlessWiFiSettings.h>
#include "string_utils.h"

#include <Adafruit_BME280.h>
#include <bme68xLibrary.h>

namespace BME280
{
    enum class SensorType
    {
        NONE,
        BME280,
        BME68X,
    };

    Adafruit_BME280 bme280;
    Bme68x bme68x;
    SensorType sensorType = SensorType::NONE;

    String BME280_I2c;
    int BME280_I2c_Bus;
    unsigned long bme280PreviousMillis = 0;
    int sensorInterval = 60000;
    bool initialized = false;

    static bool beginBME68x(uint8_t address)
    {
        TwoWire &wire = (BME280_I2c_Bus == 1) ? Wire : Wire1;
        bme68x.begin(address, wire);

        int8_t status = bme68x.checkStatus();
        if (status == BME68X_ERROR) {
            return false;
        }

        if (status == BME68X_WARNING) {
            Log.println("[BME280] BME68x warning: " + bme68x.statusString());
        }

        bme68x.setTPH();
        bme68x.setFilter(BME68X_FILTER_OFF);
        bme68x.setHeaterProf(300, 100);
        sensorType = SensorType::BME68X;
        return true;
    }

    static bool beginBME280(uint8_t address)
    {
        bool ok = false;
        if (BME280_I2c_Bus == 1) {
            ok = bme280.begin(address, &Wire);
        } else if (BME280_I2c_Bus == 2) {
            ok = bme280.begin(address, &Wire1);
        }

        if (!ok) {
            return false;
        }

        bme280.setSampling(
            Adafruit_BME280::MODE_FORCED,
            Adafruit_BME280::SAMPLING_X1,
            Adafruit_BME280::SAMPLING_X1,
            Adafruit_BME280::SAMPLING_X1,
            Adafruit_BME280::FILTER_OFF
        );

        sensorType = SensorType::BME280;
        return true;
    }

    void Setup()
    {
        if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;

        uint8_t address = 0;
        if (BME280_I2c == "0x76") {
            address = 0x76;
        } else if (BME280_I2c == "0x77") {
            address = 0x77;
        } else {
            return;
        }

        // Prefer Bosch BME68x implementation when available. If chip is not BME68x,
        // fall back to BME280 library to preserve existing behavior.
        if (!beginBME68x(address) && !beginBME280(address)) {
            Log.println("[BME280] Couldn't find a compatible BME sensor, check wiring and I2C address!");
            return;
        }

        initialized = true;
    }

    void ConnectToWifi()
    {
        BME280_I2c_Bus = HeadlessWiFiSettings.integer("BME280_I2c_Bus", 1, 2, DEFAULT_I2C_BUS, "I2C Bus");
        BME280_I2c = HeadlessWiFiSettings.string("BME280_I2c", "", "I2C address (0x76 or 0x77)");
    }

    void SerialReport()
    {
        if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
        if (BME280_I2c.isEmpty()) return;

        Log.print("BME280:       ");
        String model = "unknown";
        if (sensorType == SensorType::BME68X) model = "BME68x";
        if (sensorType == SensorType::BME280) model = "BME280";
        Log.println(BME280_I2c + " on bus " + BME280_I2c_Bus + " (" + model + ")");
    }

    void Loop()
    {
        if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
        if (!initialized) return;

        if (millis() - bme280PreviousMillis < sensorInterval) return;

        float temperature = NAN;
        float humidity = NAN;
        float pressure = NAN;
        float gasResistance = NAN;

        if (sensorType == SensorType::BME68X) {
            bme68xData data;
            bme68x.setOpMode(BME68X_FORCED_MODE);
            delayMicroseconds(bme68x.getMeasDur());

            if (!bme68x.fetchData()) {
                return;
            }

            bme68x.getData(data);
            temperature = data.temperature;
            humidity = data.humidity;
            pressure = data.pressure / 100.0F;
            gasResistance = data.gas_resistance;
        } else if (sensorType == SensorType::BME280) {
            bme280.takeForcedMeasurement();
            temperature = bme280.readTemperature();
            humidity = bme280.readHumidity();
            pressure = bme280.readPressure() / 100.0F;
        } else {
            return;
        }

        pub((roomsTopic + "/bme280_temperature").c_str(), 0, 1, String(temperature).c_str());
        pub((roomsTopic + "/bme280_humidity").c_str(), 0, 1, String(humidity).c_str());
        pub((roomsTopic + "/bme280_pressure").c_str(), 0, 1, String(pressure).c_str());

        if (sensorType == SensorType::BME68X) {
            pub((roomsTopic + "/bme68x_gas").c_str(), 0, 1, String(gasResistance).c_str());
        }

        bme280PreviousMillis = millis();
    }

    bool SendDiscovery()
    {
        if (BME280_I2c.isEmpty()) return true;

        bool ok = sendSensorDiscovery("BME280 Temperature", EC_NONE, "temperature", "°C")
            && sendSensorDiscovery("BME280 Humidity", EC_NONE, "humidity", "%")
            && sendSensorDiscovery("BME280 Pressure", EC_NONE, "pressure", "hPa");

        if (sensorType == SensorType::BME68X) {
            ok = ok && sendSensorDiscovery("BME68x Gas", EC_NONE, "", "Ω");
        }

        return ok;
    }
}

#endif
