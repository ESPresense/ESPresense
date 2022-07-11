#ifdef SENSORS

#include "I2C.h"

#include "globals.h"
#include "mqtt.h"
#include "defaults.h"
#include <WiFiSettings.h>
#include "string_utils.h"

#include <Adafruit_BME280.h>

namespace I2C
{
    bool I2CDebug = false;

    void ConnectToWifi()
    {
        WiFiSettings.heading("I2C Settings <a href='https://espresense.com/configuration/settings#i2c-settings' target='_blank'>ℹ️</a>", false);

        WiFiSettings.html("h4", "Bus 1:");

        I2C_Bus_1_SDA = WiFiSettings.integer("I2C_Bus_1_SDA", 0, 39, DEFAULT_I2C_BUS_1_SDA, "SDA pin (-1 to disable)");
        I2C_Bus_1_SCL = WiFiSettings.integer("I2C_Bus_1_SCL", 0, 39, DEFAULT_I2C_BUS_1_SCL, "SCL pin (-1 to disable)");

        WiFiSettings.html("h4", "Bus 2:");

        I2C_Bus_2_SDA = WiFiSettings.integer("I2C_Bus_2_SDA", -1, "SDA pin (-1 to disable)");
        I2C_Bus_2_SCL = WiFiSettings.integer("I2C_Bus_2_SCL", -1, "SCL pin (-1 to disable)");

        I2CDebug = WiFiSettings.checkbox("I2CDebug", false, "Debug I2C addreses. Look at the serial log to get the correct address");
    }

    void SerialReport()
    {
    }

    void Setup()
    {
        if (I2C_Bus_1_SDA != -1 && I2C_Bus_1_SDA != -1) {
            Wire.begin(I2C_Bus_1_SDA, I2C_Bus_1_SCL);
            I2C_Bus_1_Enabled = true;
            Serial.println("Initialized I2C Bus 1 (SDA: " + String(I2C_Bus_1_SDA) + ", SCL: " + String(I2C_Bus_1_SCL) + ")");
        }

        if (I2C_Bus_2_SDA != -1 && I2C_Bus_2_SDA != -1) {
            Wire1.begin(I2C_Bus_2_SDA, I2C_Bus_2_SCL);
            I2C_Bus_2_Enabled = true;
            Serial.println("Initialized I2C Bus 2 (SDA: " + String(I2C_Bus_2_SDA) + ", SCL: " + String(I2C_Bus_2_SCL) + ")");
        }
    }

    void Loop()
    {
        if (!I2C_Bus_1_Enabled && !I2C_Bus_2_Enabled) return;
        if (!I2CDebug) return;
        I2CDebug = false;

        byte error, address;
        int nDevices;
        nDevices = 0;

        if (I2C_Bus_1_Enabled)
        {
            Serial.println("Scanning I2C for devices on Bus 1...");
            for (address = 1; address < 127; address++)
            {
                Wire.beginTransmission(address);
                error = Wire.endTransmission();
                if (error == 0)
                {
                    Serial.print("I2C device found on bus 1 at address 0x");

                    if (address < 16)
                    {
                        Serial.print("0");
                    }

                    Serial.println(address, HEX);
                    nDevices++;
                }
                else if (error == 4)
                {
                    Serial.print("Unknown error on bus 1 at address 0x");
                    if (address < 16)
                    {
                        Serial.print("0");
                    }
                    Serial.println(address, HEX);
                }
            }
        }

        if (I2C_Bus_2_Enabled)
        {
            Serial.println("Scanning I2C for devices on Bus 2...");

            for (address = 1; address < 127; address++)
            {
                Wire1.beginTransmission(address);
                error = Wire1.endTransmission();
                if (error == 0)
                {
                    Serial.print("I2C device found on bus 2 at address 0x");

                    if (address < 16)
                    {
                        Serial.print("0");
                    }

                    Serial.println(address, HEX);
                    nDevices++;
                }
                else if (error == 4)
                {
                    Serial.print("Unknown error on bus 2 at address 0x");
                    if (address < 16)
                    {
                        Serial.print("0");
                    }
                    Serial.println(address, HEX);
                }
            }
        }

        if (nDevices == 0)
        {
            Serial.println("No I2C devices found\n");
        }
    }

    bool SendDiscovery()
    {
        return true;
    }
}

#endif
