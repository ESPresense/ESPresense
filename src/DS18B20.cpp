#ifdef SENSORS
#include "DS18B20.h"

#include "globals.h"
#include "mqtt.h"
#include "defaults.h"
#include <AsyncWiFiSettings.h>
#include <AsyncMqttClient.h>
#include "string_utils.h"

#include <DallasTemperature.h>
#include <OneWire.h>
#include <Ticker.h>


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

    /** Ticker for temperature reading */
    Ticker tempTicker;

    /** Flags for temperature readings finished */
    bool gotNewTemperature = false;

    /* Flag if main loop is running */
    bool dsTasksEnabled = false;

    /* update every 10 seconds */
    int dsUpdateTime = 10;

    /**
     * Task to reads temperature from DS18B20 sensor
     * @param pvParameters
     *		pointer to task parameters
     */
    void tempTask(void *pvParameters)
    {
        while (1) // tempTask loop
        {
            if (dsTasksEnabled && !gotNewTemperature)
            {
                // Read temperature only if old data was processed already
                sensors.requestTemperatures();
                gotNewTemperature = true;
            }
            vTaskSuspend(NULL);
        }
    }

    /**
     * triggerGetTemp
     * Sets flag dhtUpdated to true for handling in loop()
     * called by Ticker tempTicker
     */
    void triggerGetTemp()
    {
        if (DSTempTaskHandle != NULL)
        {
            xTaskResumeFromISR(DSTempTaskHandle);
        }
    }

    void Setup()
    {
        if (ds18b20Pin>=0) {
            oneWire.begin(ds18b20Pin);
            sensors.setOneWire(&oneWire);
            sensors.begin();
            //Need to begin twice for some reason to get the sensors to start
            sensors.begin();

            numSensors = sensors.getDeviceCount();

            // Start task to get temperature
            xTaskCreatePinnedToCore(
                    tempTask,           /* Function to implement the task */
                    "DS",              /* Name of the task */
                    1024,               /* Stack size in words */
                    NULL,               /* Task input parameter */
                    5,                  /* Priority of the task */
                    &DSTempTaskHandle, /* Task handle. */
                    1);                 /* Core where the task should run */

            if (DSTempTaskHandle == NULL)
            {
                Serial.println("[ERROR] Failed to start task for temperature update");
            }
            else
            {
                // Start update of environment data every 10 seconds
                tempTicker.attach(dsUpdateTime, triggerGetTemp);
            }

            // Signal end of setup() to tasks
            dsTasksEnabled = true;
        }
    }

    void ConnectToWifi()
    {
        ds18b20Pin = AsyncWiFiSettings.integer("ds18b20_pin", -1, "DS18B20 sensor pin (-1 for disable)");
        dsTempOffset = AsyncWiFiSettings.floating("dsTemp_offset", -40, 125, 0.0, "DS18B20 temperature offset");
    }

    void SerialReport()
    {
        if (ds18b20Pin<0) return;
        Serial.print("DS18B20 Sensor: ");
        Serial.println((ds18b20Pin>=0 ? "pin " + String(ds18b20Pin) : "disabled").c_str());
        Serial.print("DS18B20 Offset:   ");
        Serial.println(dsTempOffset);
    }

    void Loop()
    {
        if (ds18b20Pin<0) return;

        if (gotNewTemperature)
        {
            for (int i = 0; i < numSensors; i++){
                float temperature = sensors.getTempCByIndex(i) + dsTempOffset;
                Serial.println("DS18B20 Temp_"+ String(i+1) + ": " + String(temperature, 1) + "'C");
                if( sensors.getTempCByIndex(i) > -127) // Skip null values
                {
                    pub((roomsTopic + "/ds18b20_temperature_" + String(i+1)).c_str(), 0, 1, String(temperature, 1).c_str());
                }
            }

            gotNewTemperature = false;
        }
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
