#ifdef SENSORS
#include "BME280.h"

#include "globals.h"
#include "mqtt.h"
#include "defaults.h"
#include <HeadlessWiFiSettings.h>
#include "string_utils.h"

#include <Adafruit_BME280.h>
#include <Adafruit_BME680.h>

namespace BME280
{
    enum class SensorType
    {
        NONE,
        BME280,
        BME68X,
    };

    static constexpr uint8_t CHIP_ID_REG = 0xD0;
    static constexpr uint8_t BME280_CHIP_ID = 0x60;
    static constexpr uint8_t BME68X_SENSOR_CHIP_ID = 0x61;  // BME680/BME688

    Adafruit_BME280 bme280;
    Adafruit_BME680 bme680;
    SensorType sensorType = SensorType::NONE;

    String BME280_I2c;
    int BME280_I2c_Bus;
    unsigned long bme280PreviousMillis = 0;
    int sensorInterval = 60000;
    bool initialized = false;

    static TwoWire &getWire()
    {
        return (BME280_I2c_Bus == 1) ? Wire : Wire1;
    }

    static bool readChipId(uint8_t address, uint8_t &chipId)
    {
        auto &wire = getWire();
        wire.beginTransmission(address);
        wire.write(CHIP_ID_REG);
        if (wire.endTransmission(false) != 0) {
            return false;
        }

        if (wire.requestFrom((int)address, 1) != 1) {
            return false;
        }

        chipId = wire.read();
        return true;
    }

    static bool beginBME68x(uint8_t address)
    {
        if (!bme680.begin(address, &getWire())) {
            return false;
        }

        // Match recommended baseline config.
        bme680.setTemperatureOversampling(BME680_OS_8X);
        bme680.setHumidityOversampling(BME680_OS_2X);
        bme680.setPressureOversampling(BME680_OS_4X);
        bme680.setIIRFilterSize(BME680_FILTER_SIZE_3);
        bme680.setGasHeater(320, 150);  // 320°C for 150ms

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

        uint8_t chipId = 0;
        if (!readChipId(address, chipId)) {
            Log.println("[BME280] Failed to read chip ID at configured address");
            return;
        }

        bool ok = false;
        if (chipId == BME280_CHIP_ID) {
            ok = beginBME280(address);
        } else if (chipId == BME68X_SENSOR_CHIP_ID) {
            ok = beginBME68x(address);
        } else {
            Log.printf("[BME280] Unknown Bosch chip ID: 0x%02X\r\n", chipId);
            return;
        }

        if (!ok) {
            Log.println("[BME280] Couldn't initialize compatible BME sensor");
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
            if (!bme680.performReading()) {
                return;
            }

            temperature = bme680.temperature;
            humidity = bme680.humidity;
            pressure = bme680.pressure / 100.0F;
            gasResistance = bme680.gas_resistance;
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
