#ifdef SENSORS
#include "PWS.h"

#include <AsyncWiFiSettings.h>
#include <Ticker.h>

#include "defaults.h"
#include "globals.h"
#include "mqtt.h"
#include "string_utils.h"

namespace PWS {
    int pwPin1 = -1;
    uint32_t last_rise = 0;
    uint32_t last_width = 0;

    /** Task handle for the pw report task */
    TaskHandle_t pwTaskHandle = NULL;

    /** Ticker for report pw */
    Ticker pwTicker;

    /** Flags for temperature readings finished */
    bool gotReport = false;

    /* Flag if main loop is running */
    bool pwTasksEnabled = false;

    /** Flags for pulse readings finished */
    bool gotNewPulse = false;

    /* update time */
    int pwUpdateTime = 10; //ToDo: maybe make this a user choise via settings menu

    void IRAM_ATTR pw_isr_handler(void)
    {
        const bool new_level = digitalRead(pwPin1);
        const uint32_t now = millis();

        if (new_level) {
            last_rise = now;
            gotNewPulse = false;
        } else {
            last_width = (now - last_rise);
            gotNewPulse = true;
        }
    }

    /**
     * Task to report Pulse Width
     * @param pvParameters
     *		pointer to task parameters
     */
    void pwTask(void *pvParameters)
    {
        while (1) // tempTask loop
        {
            if (pwTasksEnabled && !gotReport)
            {
                gotReport = true;
            }
            vTaskSuspend(NULL);
        }
    }

    /**
     * triggerReportPW
     * Sets flag dhtUpdated to true for handling in loop()
     * called by Ticker tempTicker
     */
    void triggerReportPW()
    {
        if (pwTaskHandle != NULL)
        {
            xTaskResumeFromISR(pwTaskHandle);
        }
    }

    float get_pulse_width_ms(void) {
        return last_width;
    }

    /* Initialize PW functions:
    *  - configure and initialize task
    */
    static void pw_init(void)
    {
        attachInterrupt(pwPin1, pw_isr_handler, CHANGE);
    }

    void Setup() {
        if (pwPin1 > 0) {
            pinMode(pwPin1, INPUT);
            pw_init();

            // Start task to get temperature
            xTaskCreatePinnedToCore(
                    pwTask,           /* Function to implement the task */
                    "PWS",              /* Name of the task */
                    1024,               /* Stack size in words */
                    NULL,               /* Task input parameter */
                    5,                  /* Priority of the task */
                    &pwTaskHandle, /* Task handle. */
                    1);                 /* Core where the task should run */

            if (pwTaskHandle == NULL)
            {
                Serial.println("[ERROR] Failed to start task for temperature update");
            }
            else
            {
                // Start update of environment data every 10 seconds
                pwTicker.attach(pwUpdateTime, triggerReportPW);
            }

            // Signal end of setup() to tasks
            pwTasksEnabled = true;
        }
    }

    void ConnectToWifi() {
        AsyncWiFiSettings.html("h4", "Pulse Width Sensor:");
        pwPin1 = AsyncWiFiSettings.integer("pw_pin", -1, "Pulse Width Sensor pin (-1 for disable)");
    }

    void SerialReport() {
        if (pwPin1 < 0) return;
        Serial.print("Pulse Width Sensor: ");
        Serial.println((pwPin1>=0 ? "pin " + String(pwPin1) : "disabled").c_str());
    }

    void Loop() {
        if (pwPin1 < 0) return;

        if (gotReport) {
            float width = get_pulse_width_ms();
            Serial.println(("\n\n**********************Got pulse width "  + String(width) + " s").c_str());
            pub((roomsTopic + "/pulse_width").c_str(), 0, 1, String(width, 1).c_str());
            gotReport = false;
        }
    }

    bool SendDiscovery() {
        if (pwPin1 < 0) return true;
        return sendSensorDiscovery("Pulse Width", EC_NONE, "", "ms");
    }
}  // namespace SHT

#endif
