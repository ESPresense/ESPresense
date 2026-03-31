#ifdef SENSORS
#include "M5_ENV_III.h"

#include "globals.h"
#include "mqtt.h"
#include "defaults.h"
#include <HeadlessWiFiSettings.h>
#include <AsyncMqttClient.h>
#include "string_utils.h"

#include <SHTSensor.h>
#include <QMP6988.h>

namespace M5_ENV_III
{
    // SHT30 sensor configuration
    int shtBus = -1;
    SHTSensor* shtSensor = nullptr;
    
    // QMP6988 sensor configuration
    int qmpBus = -1;
    QMP6988* qmpSensor = nullptr;
    
    // Sensor timing
    unsigned long lastRead = 0;
    int sensorInterval = 60000; // 60 seconds default
    
    // Flags for initialization
    bool shtInitialized = false;
    bool qmpInitialized = false;
    
    /**
     * @brief Initialize SHT30 temperature/humidity sensor
     *
     * Attempts to initialize the SHT30 sensor on the selected I2C bus.
     */
    void initSHT30() {
        if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
        if (shtBus < 1 || shtBus > 2) return;
        
        shtSensor = new SHTSensor();
        shtInitialized = shtSensor->init(shtBus == 1 ? Wire : Wire1);
        
        if (!shtInitialized) {
            Log.println("[M5_ENV_III] Couldn't find SHT30 sensor, check your wiring and I2C address!");
        } else {
            shtSensor->setAccuracy(SHTSensor::SHT_ACCURACY_MEDIUM);
        }
    }
    
    /**
     * @brief Initialize QMP6988 pressure sensor
     *
     * Attempts to initialize the QMP6988 sensor on the selected I2C bus.
     */
    void initQMP6988() {
        if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
        if (qmpBus < 1 || qmpBus > 2) return;
        
        qmpSensor = new QMP6988();
        qmpInitialized = qmpSensor->begin(qmpBus == 1 ? &Wire : &Wire1, QMP6988_SLAVE_ADDRESS_L);
        
        if (!qmpInitialized) {
            Log.println("[M5_ENV_III] Couldn't find QMP6988 sensor, check your wiring and I2C address!");
        } else {
            // Set default configuration for QMP6988
            qmpSensor->setFilter(QMP6988_FILTERCOEFF_4);
            qmpSensor->setOversamplingP(QMP6988_OVERSAMPLING_8X);
            qmpSensor->setOversamplingT(QMP6988_OVERSAMPLING_1X);
        }
    }

    /**
     * @brief Initialize M5stack ENV III sensors
     *
     * Sets up both SHT30 (temperature/humidity) and QMP6988 (pressure) sensors.
     */
    void Setup()
    {
        initSHT30();
        initQMP6988();
    }

    /**
     * @brief Configure M5stack ENV III sensor settings from WiFi settings
     *
     * Reads configuration parameters from HeadlessWiFiSettings for both sensors.
     */
    void ConnectToWifi()
    {
        shtBus = HeadlessWiFiSettings.integer("M5_ENV_III_SHT_I2c_Bus", 1, 2, -1, "SHT30 I2C Bus (-1 to disable)");
        qmpBus = HeadlessWiFiSettings.integer("M5_ENV_III_QMP_I2c_Bus", 1, 2, -1, "QMP6988 I2C Bus (-1 to disable)");
        sensorInterval = HeadlessWiFiSettings.integer("M5_ENV_III_Interval", 60000, "Sensor update interval in ms");
    }

    /**
     * @brief Report M5stack ENV III sensor configuration to log
     *
     * Logs the configured I2C buses for both sensors.
     */
    void SerialReport()
    {
        if (shtBus >= 1 && shtBus <= 2) {
            Log.println("M5_ENV_III SHT30: Auto-detect on bus " + String(shtBus));
        } else {
            Log.println("M5_ENV_III SHT30: Disabled");
        }
        
        if (qmpBus >= 1 && qmpBus <= 2) {
            Log.println("M5_ENV_III QMP6988: Auto-detect on bus " + String(qmpBus));
        } else {
            Log.println("M5_ENV_III QMP6988: Disabled");
        }
        
        Log.println("M5_ENV_III Interval: " + String(sensorInterval) + "ms");
    }

    /**
     * @brief Process sensor measurements and publish data
     *
     * Reads from both sensors and publishes measurements to MQTT if available.
     */
    void Loop()
    {
        if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
        
        if (lastRead == 0 || millis() - lastRead >= sensorInterval) {
            lastRead = millis();
            
            // Handle SHT30 sensor data
            if (shtInitialized && shtSensor) {
                if (shtSensor->readSample()) {
                    pub((roomsTopic + "/temperature").c_str(), 0, 1, String(shtSensor->getTemperature()).c_str());
                    pub((roomsTopic + "/humidity").c_str(), 0, 1, String(shtSensor->getHumidity()).c_str());
                }
            }
            
            // Handle QMP6988 sensor data
            if (qmpInitialized && qmpSensor) {
                qmpSensor->update();
                pub((roomsTopic + "/pressure").c_str(), 0, 1, String(qmpSensor->pressure).c_str());
                pub((roomsTopic + "/altitude").c_str(), 0, 1, String(qmpSensor->altitude).c_str());
            }
        }
    }

    /**
     * @brief Send Home Assistant discovery messages for all sensors
     *
     * @return true if discovery messages were sent successfully, false otherwise
     */
    bool SendDiscovery()
    {
        bool result = true;
        
        // Send discovery for SHT30 sensors if enabled
        if (shtBus >= 1 && shtBus <= 2) {
            result = result && sendSensorDiscovery("Temperature", EC_NONE, "temperature", "°C");
            result = result && sendSensorDiscovery("Humidity", EC_NONE, "humidity", "%");
        }
        
        // Send discovery for QMP6988 sensors if enabled
        if (qmpBus >= 1 && qmpBus <= 2) {
            result = result && sendSensorDiscovery("Pressure", EC_NONE, "pressure", "hPa");
            result = result && sendSensorDiscovery("Altitude", EC_NONE, "altitude", "m");
        }
        
        return result;
    }

    /**
     * @brief Handle MQTT commands for M5stack ENV III sensors
     *
     * @param command Command string
     * @param pay Payload string
     * @return true if command was handled, false otherwise
     */
    bool Command(String& command, String& pay)
    {
        return false;
    }

    /**
     * @brief Send online status for M5stack ENV III sensors
     *
     * @return true if status was sent successfully, false otherwise
     */
    bool SendOnline()
    {
        return true;
    }
}
#endif