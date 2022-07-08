#ifdef SENSORS

#include "I2CScanner.h"

#include "globals.h"
#include "mqtt.h"
#include "defaults.h"
#include <WiFiSettings.h>
#include "string_utils.h"

#include <Adafruit_BME280.h>

namespace I2CScanner
{
    bool enabled = false;

    void Setup()
    {
    }

    void ConnectToWifi()
    {
        enabled = WiFiSettings.checkbox("enabled", false, "Debug I2C addreses. Look at the serial log to get the correct address");
    }

    void SerialReport()
    {
    }

    void Loop()
    {
        if (!I2C_Bus_1_Enabled && !I2C_Bus_2_Enabled) return;
        if (!enabled) return;
        enabled = false;

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
                    Serial.print("Unknow error on bus 1 at address 0x");
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
                    Serial.print("Unknow error on bus 2 at address 0x");
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
    }
}

#endif
