#ifdef SENSORS
#include "HC_SR04.h"

#include "globals.h"
#include "mqtt.h"
#include "defaults.h"
#include <HeadlessWiFiSettings.h>
#include <AsyncMqttClient.h>
#include "string_utils.h"

#include <Ticker.h>

namespace HC_SR04
{
    int trigPin = -1;
    int echoPin = -1;
    int maxDistanceCm = 400;
    int measurementIntervalMs = 1000;
    float distanceOffset = 0.0;

    // Variables to store distance measurements
    float currentDistance = 0.0;
    bool distanceAvailable = false;
    unsigned long lastMeasurementTime = 0;

    // Ticker for periodic measurements
    Ticker distanceTicker;

    /**
     * @brief Measure distance using HC-SR04 sensor
     *
     * @return Distance in centimeters, or -1 if measurement failed
     */
    float measureDistance()
    {
        if (trigPin < 0 || echoPin < 0) return -1;

        // Send trigger pulse
        digitalWrite(trigPin, LOW);
        delayMicroseconds(2);
        digitalWrite(trigPin, HIGH);
        delayMicroseconds(10);
        digitalWrite(trigPin, LOW);

        // Read echo pulse
        long duration = pulseIn(echoPin, HIGH, maxDistanceCm * 58 + 1000); // Timeout based on max distance

        if (duration == 0) return -1; // Timeout

        // Convert to centimeters
        float distanceCm = duration * 0.0343 / 2.0;
        
        // Apply offset
        distanceCm += distanceOffset;
        
        // Ensure distance is within bounds
        if (distanceCm < 0) distanceCm = 0;
        if (distanceCm > maxDistanceCm) distanceCm = maxDistanceCm;
        
        return distanceCm;
    }

    /**
     * @brief Trigger distance measurement
     *
     * Called by Ticker to take measurements periodically
     */
    void triggerDistanceMeasurement()
    {
        if (trigPin >= 0 && echoPin >= 0)
        {
            currentDistance = measureDistance();
            distanceAvailable = (currentDistance >= 0);
        }
    }

    /**
     * @brief Initialize HC-SR04 sensor
     *
     * Sets up the trigger and echo pins and starts the measurement ticker
     */
    void Setup()
    {
        if (trigPin >= 0 && echoPin >= 0)
        {
            pinMode(trigPin, OUTPUT);
            pinMode(echoPin, INPUT);
            
            // Start periodic measurements
            distanceTicker.attach_ms(measurementIntervalMs, triggerDistanceMeasurement);
            
            Log.println("HC-SR04 sensor initialized");
        }
    }

    /**
     * @brief Configure HC-SR04 sensor settings from WiFi settings
     *
     * Reads configuration parameters from HeadlessWiFiSettings
     */
    void ConnectToWifi()
    {
        trigPin = HeadlessWiFiSettings.integer("hc_sr04_trig_pin", -1, "HC-SR04 trigger pin (-1 for disable)");
        echoPin = HeadlessWiFiSettings.integer("hc_sr04_echo_pin", -1, "HC-SR04 echo pin (-1 for disable)");
        maxDistanceCm = HeadlessWiFiSettings.integer("hc_sr04_max_distance", 400, "Maximum distance in cm");
        measurementIntervalMs = HeadlessWiFiSettings.integer("hc_sr04_interval", 1000, "Measurement interval in ms");
        distanceOffset = HeadlessWiFiSettings.floating("hc_sr04_offset", -1000, 1000, 0.0, "Distance offset in cm");
    }

    /**
     * @brief Report HC-SR04 sensor configuration to log
     *
     * Logs the configured pins and settings
     */
    void SerialReport()
    {
        if (trigPin < 0 && echoPin < 0) return;
        
        Log.print("HC-SR04 Trigger: ");
        Log.println((trigPin >= 0 ? "pin " + String(trigPin) : "disabled").c_str());
        Log.print("HC-SR04 Echo: ");
        Log.println((echoPin >= 0 ? "pin " + String(echoPin) : "disabled").c_str());
        Log.print("HC-SR04 Max Distance: ");
        Log.println(maxDistanceCm);
        Log.print("HC-SR04 Interval: ");
        Log.println(measurementIntervalMs);
        Log.print("HC-SR04 Offset: ");
        Log.println(distanceOffset);
    }

    /**
     * @brief Process distance measurements and publish data
     *
     * Publishes the latest distance measurement to MQTT if available
     */
    void Loop()
    {
        if (trigPin < 0 && echoPin < 0) return;

        if (distanceAvailable)
        {
            Log.println("Distance: " + String(currentDistance, 1) + " cm");
            
            // Publish distance measurement
            pub((roomsTopic + "/distance").c_str(), 0, 1, String(currentDistance, 1).c_str());
            
            distanceAvailable = false;
        }
    }

    /**
     * @brief Send Home Assistant discovery message for HC-SR04 sensor
     *
     * @return true if discovery message was sent successfully, false otherwise
     */
    bool SendDiscovery()
    {
        if (trigPin < 0 && echoPin < 0) return true;
        
        return sendSensorDiscovery("Distance", EC_NONE, "distance", "cm");
    }

    /**
     * @brief Handle MQTT commands for HC-SR04 sensor
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
     * @brief Send online status for HC-SR04 sensor
     *
     * @return true if status was sent successfully, false otherwise
     */
    bool SendOnline()
    {
        return true;
    }
}
#endif