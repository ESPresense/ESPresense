#ifdef SENSORS
#include "DS18B20.h"

#include "globals.h"
#include "mqtt.h"
#include "defaults.h"
#include <HeadlessWiFiSettings.h>
#include <AsyncMqttClient.h>
#include "string_utils.h"

#include <DallasTemperature.h>
#include <OneWire.h>


namespace DS18B20
{
    int ds18b20Pin = -1;
    float dsTempOffset;
    int numSensors = 0;

    /** Initialize onewire and Dallas Temperature*/
    OneWire oneWire;
    DallasTemperature sensors;

    /** Task handle for the light value read task */
    TaskHandle_t DSTempTaskHandle = NULL;

    /** Flags for temperature readings finished */
    bool gotNewTemperature = false;

    /** Flag if main loop is running */
    bool dsTasksEnabled = false;

    /** wait before first read, then set to true */
    bool initialReadDone = false;

    /** update temp every 20 seconds */
    int dsUpdateTimeS = 20;

    /** setup delay before first read to give the sensor time to settle after initialization, seconds */
    int dsFirstReadDelayS = 2;

    /** wait for temps delay, milliseconds; datasheet says 750ms max */
    int dsWaitTimeMs = 750;

    /**
     * Task to reads temperature from DS18B20 sensor
     * @param pvParameters
     *		pointer to task parameters
     */
    void tempTask(void *pvParameters)
    {
        while (1) // tempTask loop
        {
            // avoid 85 C readings on initialization
            if (!initialReadDone) {
                sensors.requestTemperatures();
                delay(dsFirstReadDelayS * 1000);
                bool allReadsOk = true;
                for (int i = 0; i < numSensors; i++) {
                    float rawTemp = sensors.getTempCByIndex(i);
                    if (rawTemp == 85) {
                        allReadsOk = false;
                    }
                }
                if (!allReadsOk) {
                    continue;
                }
                initialReadDone = true;
            }

            if (dsTasksEnabled && !gotNewTemperature)
            {
                // Read temperature only if old data was processed already
                sensors.requestTemperatures();
                gotNewTemperature = true;
            }
            delay(dsUpdateTimeS * 1000);
        }
    }

    /**
     * @brief Initialize DS18B20 hardware and start the background temperature task.
     *
     * When the configured DS18B20 pin is enabled (ds18b20Pin >= 0), configures the OneWire/DallasTemperature
     * interfaces, detects the number of temperature devices, and starts a FreeRTOS task that periodically
     * reads temperatures. On failure to find any devices or to create the task, logs an error and exits
     * without enabling the task loop.
     */
    void Setup()
    {
        if (ds18b20Pin>=0) {
            oneWire.begin(ds18b20Pin);
            sensors.setOneWire(&oneWire);
            sensors.begin();

            numSensors = sensors.getDeviceCount();

            if (numSensors == 0) {
                Log.println("[ERROR] No DS sensors found");
                return;
            }
            // Start task to get temperature
            xTaskCreatePinnedToCore(
                tempTask,           /* Function to implement the task */
                "DS",               /* Name of the task */
                1024,               /* Stack size in words */
                NULL,               /* Task input parameter */
                5,                  /* Priority of the task */
                &DSTempTaskHandle,  /* Task handle. */
                1);                 /* Core where the task should run */

            if (DSTempTaskHandle == NULL)
            {
                Log.println("[ERROR] Failed to start task for temperature update");
                return;
            }

            // Signal end of setup() to tasks
            dsTasksEnabled = true;
        }
    }

    void ConnectToWifi()
    {
        ds18b20Pin = HeadlessWiFiSettings.integer("ds18b20_pin", -1, "DS18B20 sensor pin (-1 for disable)");
        dsTempOffset = HeadlessWiFiSettings.floating("dsTemp_offset", -40, 125, 0.0, "DS18B20 temperature offset");
    }

    /**
     * @brief Log the DS18B20 configuration (pin and temperature offset) when enabled.
     *
     * Prints the configured DS18B20 pin and the global temperature offset to the log.
     * If the DS18B20 pin is negative (sensor disabled), the function returns without logging.
     */
    void SerialReport()
    {
        if (ds18b20Pin<0) return;
        Log.print("DS18B20 Sensor: ");
        Log.println((ds18b20Pin>=0 ? "pin " + String(ds18b20Pin) : "disabled").c_str());
        Log.print("DS18B20 Offset:   ");
        Log.println(dsTempOffset);
    }

    /**
     * @brief Publish newly available DS18B20 temperatures and pace sensor polling.
     *
     * When the DS18B20 interface is enabled and new temperature data has been flagged,
     * reads each sensor, applies the configured temperature offset, logs the value,
     * and publishes valid readings to MQTT topics of the form "<roomsTopic>/ds18b20_temperature_<n>".
     * Readings with a raw value less than or equal to -127 are treated as invalid and are not published.
     * After publishing, clears the new-data flag and delays briefly to avoid polling the slow sensor too frequently.
     */
    void Loop()
    {
        if (ds18b20Pin<0) return;

        if (gotNewTemperature)
        {
            for (int i = 0; i < numSensors; i++){
                float rawTemp = sensors.getTempCByIndex(i);
                float temperature = rawTemp + dsTempOffset;
                Log.println("DS18B20 Temp_"+ String(i+1) + ": " + String(temperature, 1) + "'C");
                if( rawTemp > -127) // Skip null values
                {
                    pub((roomsTopic + "/ds18b20_temperature_" + String(i+1)).c_str(), 0, 1, String(temperature, 1).c_str());
                }
            }

            gotNewTemperature = false;
        }

        // since the sensor is slow (up to 750ms to read the temperature), no point in checking too often
        delay(dsWaitTimeMs / 4);
    }

    bool SendDiscovery()
    {
        if (ds18b20Pin<0) return true;
        bool returnVal = true;

        for (int i = 0; i < numSensors; i++){
            returnVal = sendSensorDiscovery("DS18B20 Temperature " + String(i+1), EC_NONE, "temperature", "°C") && returnVal;
        }
        return returnVal;
    }

    bool Command(String& command, String& pay)
    {
        return false;
    }

    bool SendOnline()
    {
        return true;
    }
}
#endif