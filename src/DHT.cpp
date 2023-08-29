#ifdef SENSORS
#include "DHT.h"

#include "globals.h"
#include "mqtt.h"
#include "defaults.h"
#include <AsyncWiFiSettings.h>
#include <AsyncMqttClient.h>
#include "string_utils.h"

#include <DHTesp.h>
#include <Ticker.h>

namespace DHT
{
    int dht11Pin = -1;
    int dht22Pin = -1;
    float dhtTempOffset;

    /** Initialize DHT sensor 1 */
    DHTesp dhtSensor;

    /** Task handle for the light value read task */
    TaskHandle_t dhtTempTaskHandle = NULL;

    /** Ticker for temperature reading */
    Ticker tempTicker;

    /** Flags for temperature readings finished */
    bool gotNewTemperature = false;

    /** Data from dht sensor 1 */
    TempAndHumidity dhtSensorData;

    /* Flag if main loop is running */
    bool dhtTasksEnabled = false;

    /* update time */
    int dhtUpdateTime = 10; //ToDo: maybe make this a user choise via settings menu

    /**
     * Task to reads temperature from DHT11 sensor
     * @param pvParameters
     *		pointer to task parameters
     */
    void tempTask(void *pvParameters)
    {
        while (1) // tempTask loop
        {
            if (dhtTasksEnabled && !gotNewTemperature)
            {
                // Read temperature only if old data was processed already
                // Reading temperature for humidity takes about 250 milliseconds!
                // Sensor readings may also be up to 2 seconds 'old' (it's a very slow sensor)
                dhtSensorData = dhtSensor.getTempAndHumidity(); // Read values from sensor 1
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
        if (dhtTempTaskHandle != NULL)
        {
            xTaskResumeFromISR(dhtTempTaskHandle);
        }
    }

    void Setup()
    {
        if (dht11Pin>=0) dhtSensor.setup(dht11Pin, DHTesp::DHT11);
        if (dht22Pin>=0) dhtSensor.setup(dht22Pin, DHTesp::DHT22); //(AM2302)

        if (dht11Pin>=0 || dht22Pin>=0)
        {
            // Start task to get temperature
            xTaskCreatePinnedToCore(
                    tempTask,           /* Function to implement the task */
                    "DHT",              /* Name of the task */
                    1024,               /* Stack size in words */
                    NULL,               /* Task input parameter */
                    5,                  /* Priority of the task */
                    &dhtTempTaskHandle, /* Task handle. */
                    1);                 /* Core where the task should run */

            if (dhtTempTaskHandle == NULL)
            {
                Serial.println("[ERROR] Failed to start task for temperature update");
            }
            else
            {
                // Start update of environment data every 10 seconds
                tempTicker.attach(dhtUpdateTime, triggerGetTemp);
            }

            // Signal end of setup() to tasks
            dhtTasksEnabled = true;
        }
    }

    void ConnectToWifi()
    {
        dht11Pin = AsyncWiFiSettings.integer("dht11_pin", -1, "DHT11 sensor pin (-1 for disable)");
        dht22Pin = AsyncWiFiSettings.integer("dht22_pin", -1, "DHT22 sensor pin (-1 for disable)");
        dhtTempOffset = AsyncWiFiSettings.floating("dhtTemp_offset", -40, 125, 0.0, "DHT temperature offset");
    }

    void SerialReport()
    {
        if (dht11Pin<0 && dht22Pin<0) return;
        Serial.print("DHT11 Sensor: ");
        Serial.println((dht11Pin>=0 ? "pin " + String(dht11Pin) : "disabled").c_str());
        Serial.print("DHT22 Sensor: ");
        Serial.println((dht22Pin>=0 ? "pin " + String(dht22Pin) : "disabled").c_str());
        Serial.print("DHT Offset:   ");
        Serial.println(dhtTempOffset);
    }

    void Loop()
    {
        if (dht11Pin<0 && dht22Pin<0) return;

        if (gotNewTemperature)
        {
            float humidity = dhtSensorData.humidity;
            float temperature = dhtSensorData.temperature + dhtTempOffset;
            Serial.println("Temp: " + String(temperature, 1) + "'C Humidity: " + String(humidity, 1) + "%");

            pub((roomsTopic + "/humidity").c_str(), 0, 1, String(humidity, 1).c_str());
            pub((roomsTopic + "/temperature").c_str(), 0, 1, String(temperature, 1).c_str());

            gotNewTemperature = false;
        }
    }

    bool SendDiscovery()
    {
        if (dht11Pin<0 && dht22Pin<0) return true;

        return sendSensorDiscovery("Temperature", EC_NONE, "temperature", "°C") && sendSensorDiscovery("Humidity", EC_NONE, "humidity", "%");
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
