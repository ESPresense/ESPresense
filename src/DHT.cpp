#ifdef SENSORS
#include "DHT.h"

#include "globals.h"
#include "mqtt.h"
#include "defaults.h"
#include <HeadlessWiFiSettings.h>
#include <AsyncMqttClient.h>
#include "string_utils.h"

#include <DHTesp.h>
#include <Ticker.h>

namespace DHT
{
    int dht11Pin = -1;
    int dht22Pin = -1;
    float dhtTempOffset;
    float dhtHumidityOffset;

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

    /**
     * @brief Initialize configured DHT sensors and start periodic temperature/humidity collection.
     *
     * Initializes DHT11 and/or DHT22 drivers for any configured pins, creates the background
     * temperature task, attaches the periodic trigger using the module's update interval,
     * and enables DHT task execution. If no DHT pins are configured, the function does nothing.
     *
     * If task creation fails an error is logged and periodic updates are not started.
     */
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
                Log.println("[ERROR] Failed to start task for temperature update");
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
        dht11Pin = HeadlessWiFiSettings.integer("dht11_pin", -1, "DHT11 sensor pin (-1 for disable)");
        dht22Pin = HeadlessWiFiSettings.integer("dht22_pin", -1, "DHT22 sensor pin (-1 for disable)");
        dhtTempOffset = HeadlessWiFiSettings.floating("dhtTemp_offset", -40, 125, 0.0, "DHT temperature offset");
        dhtHumidityOffset = HeadlessWiFiSettings.floating("dhtHumidity_offset", -100, 100, 0.0, "DHT humidity offset");
    }

    /**
     * @brief Reports configured DHT sensor pins and temperature offset to the log.
     *
     * Prints the configured pin number for DHT11 and DHT22 (or "disabled" if not configured)
     * and the configured temperature offset. Does nothing if both sensors are disabled.
     */
    void SerialReport()
    {
        if (dht11Pin<0 && dht22Pin<0) return;
        Log.print("DHT11 Sensor: ");
        Log.println((dht11Pin>=0 ? "pin " + String(dht11Pin) : "disabled").c_str());
        Log.print("DHT22 Sensor: ");
        Log.println((dht22Pin>=0 ? "pin " + String(dht22Pin) : "disabled").c_str());
        Log.print("DHT Temp Offset:   ");
        Log.println(dhtTempOffset);
        Log.print("DHT Humidity Offset: ");
        Log.println(dhtHumidityOffset);
    }

    /**
     * @brief Processes newly read DHT sensor data: logs measurements and publishes them.
     *
     * If neither DHT11 nor DHT22 is configured, the function does nothing.
     * When a new reading is available, the stored temperature is adjusted by the configured offset,
     * the temperature and humidity are logged, the humidity is published to `roomsTopic + "/humidity"`
     * and the temperature to `roomsTopic + "/temperature"` (QoS 0, retained), and the new-reading flag is cleared.
     */
    void Loop()
    {
        if (dht11Pin<0 && dht22Pin<0) return;

        if (gotNewTemperature)
        {
            float humidity = dhtSensorData.humidity + dhtHumidityOffset;
            float temperature = dhtSensorData.temperature + dhtTempOffset;
            Log.println("Temp: " + String(temperature, 1) + "'C Humidity: " + String(humidity, 1) + "%");

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