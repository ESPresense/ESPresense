#ifdef SENSORS
#include "MQ135.h"

#include "globals.h"
#include "mqtt.h"
#include "defaults.h"
#include <HeadlessWiFiSettings.h>
#include "string_utils.h"

#include <Adafruit_ADS1X15.h>
#include <cmath>

namespace MQ135
{
    Adafruit_ADS1115 *ads;
    String MQ135_I2c;
    int MQ135_I2c_Bus;
    int MQ135_Channel;
    float MQ135_RL;      // Load resistor in kOhms
    float MQ135_R0;      // Calibration resistance R0 in kOhms
    float MQ135_Vcc;     // Sensor supply voltage in volts

    unsigned long previousMillis = 0;
    const int sensorInterval = 10000; // 10 seconds between readings
    bool initialized = false;

    // Helper: convert ADC reading to voltage for ADS1115 with gain = GAIN_TWOTHIRDS (±6.144V)
    // LSB size = 6.144V / 32768 = 0.0001875 V
    const float ADS1115_LSB = 0.0001875;

    void Setup()
    {
        if (!I2C_Bus_1_Started && !I2C_Bus_2_Started)
            return;
        if (MQ135_I2c.isEmpty())
            return;

        // Parse I2C address from string (supports hex like "0x48")
        unsigned long addr = strtoul(MQ135_I2c.c_str(), nullptr, 0);
        if (addr < 0x48 || addr > 0x4B)
        {
            Log.println("MQ135: Invalid I2C address, must be 0x48-0x4B");
            return;
        }

        ads = new Adafruit_ADS1115();
#if SOC_I2C_NUM > 1
        if (MQ135_I2c_Bus == 1)
        {
            ads->begin(static_cast<uint8_t>(addr), &Wire);
        }
        else
        {
            ads->begin(static_cast<uint8_t>(addr), &Wire1);
        }
#else
        ads->begin(static_cast<uint8_t>(addr), &Wire);
#endif
        // Set gain to ±6.144V range (covers 0-5V single-ended)
        ads->setGain(GAIN_TWOTHIRDS);

        Log.println("MQ135: ADS1115 initialized");
        initialized = true;
    }

    void ConnectToWifi()
    {
        MQ135_I2c_Bus = HeadlessWiFiSettings.integer("MQ135_I2c_Bus", 1, 2, DEFAULT_I2C_BUS, "I2C Bus");
        MQ135_I2c = HeadlessWiFiSettings.string("MQ135_I2c", "", "I2C address (0x48-0x4B)");
        MQ135_Channel = HeadlessWiFiSettings.integer("MQ135_Channel", 0, 3, 0, "ADS1115 channel (0-3)");
        MQ135_RL = HeadlessWiFiSettings.floating("MQ135_RL", 1.0, 100.0, 10.0, "Load resistor (kOhm)");
        MQ135_R0 = HeadlessWiFiSettings.floating("MQ135_R0", 0.1, 1000.0, 10.0, "Calibration R0 (kOhm)");
        MQ135_Vcc = HeadlessWiFiSettings.floating("MQ135_Vcc", 3.0, 6.0, 5.0, "Sensor supply voltage (V)");
    }

    void SerialReport()
    {
        if (!I2C_Bus_1_Started && !I2C_Bus_2_Started)
            return;
        if (MQ135_I2c.isEmpty())
            return;
        Log.print("MQ135: addr ");
        Log.print(MQ135_I2c);
        Log.print(" bus ");
        Log.print(MQ135_I2c_Bus);
        Log.print(" ch ");
        Log.print(MQ135_Channel);
        Log.print(" RL=");
        Log.print(MQ135_RL);
        Log.print("kΩ R0=");
        Log.print(MQ135_R0);
        Log.print("kΩ Vcc=");
        Log.print(MQ135_Vcc);
        Log.println("V");
    }

    void Loop()
    {
        if (!I2C_Bus_1_Started && !I2C_Bus_2_Started)
            return;
        if (!initialized)
            return;

        if (millis() - previousMillis < sensorInterval)
            return;

        int16_t adc = ads->readADC_SingleEnded(MQ135_Channel);
        // Handle potential read errors: if adc is negative or zeroish
        if (adc <= 0)
        {
            previousMillis = millis();
            return;
        }

        float voltage = adc * ADS1115_LSB; // V
        if (voltage >= MQ135_Vcc)
            voltage = MQ135_Vcc - 0.001; // avoid div by zero

        float rl_ohms = MQ135_RL * 1000.0f; // convert kOhm to Ohm
        float rs = rl_ohms * voltage / (MQ135_Vcc - voltage); // Ohm
        float r0_ohms = MQ135_R0 * 1000.0f;
        float rs_r0 = rs / r0_ohms;

        // CO2 ppm approximation for MQ135
        // ppm = 110 * (Rs/R0)^(-1.144) for CO2 (approximate range 100-10000 ppm)
        float ppm = 110.0f * pow(rs_r0, -1.144f);
        if (isnan(ppm) || isinf(ppm))
            ppm = 0;
        if (ppm < 0)
            ppm = 0;

        // Publish CO2 ppm
        pub((roomsTopic + "/mq135_co2").c_str(), 0, 1, String(static_cast<int>(round(ppm))).c_str());

        // Publish resistance in kΩ for reference
        char buf[16];
        snprintf(buf, sizeof(buf), "%.2f", rs / 1000.0f);
        pub((roomsTopic + "/mq135_resistance").c_str(), 0, 1, buf);

        previousMillis = millis();
    }

    bool SendDiscovery()
    {
        if (MQ135_I2c.isEmpty())
            return true;

        // Send discovery for CO2 ppm sensor
        bool ok = sendSensorDiscovery("MQ135 CO2", EC_NONE, "carbon_dioxide", "ppm");
        // Sensor for resistance (no standard device class, just raw value)
        ok = ok && sendSensorDiscovery("MQ135 Resistance", EC_NONE, "", "kΩ");
        return ok;
    }
}

#endif