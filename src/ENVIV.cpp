#ifdef SENSORS
#include "ENVIV.h"
#include "globals.h"
#include "mqtt.h"
#include "defaults.h"
#include <AsyncWiFiSettings.h>
#include "string_utils.h"

#include <M5UnitENV.h>
#include <SHT4x.h>

namespace ENVIV
{
    BMP280 bmp;
    //long BMP280_status;
    String BMP280_I2c;
    int BMP280_I2c_Bus;  
    unsigned long BMP280PreviousMillis = 0;
    int sensorInterval = 60000;
    bool initialized = false;
    
    void Setup() {
        //Serial.println("starting env IV setup");
        if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
        //Serial.println("bus check complete");

        if (!bmp.begin(&Wire, BMP280_I2C_ADDR, 2, 1, 400000U)) {
            Serial.println("[ENVIV BMP280] Couldn't find a sensor, check your wiring and I2C address!");
            while (1) delay(1);
        } else {
            initialized = true;
        }

        /* Default settings from datasheet. */
        bmp.setSampling(BMP280::MODE_NORMAL,     /* Operating Mode. */
                        BMP280::SAMPLING_X2,     /* Temp. oversampling */
                        BMP280::SAMPLING_X16,    /* Pressure oversampling */
                        BMP280::FILTER_X16,      /* Filtering. */
                        BMP280::STANDBY_MS_500); /* Standby time. */
    }

    void ConnectToWifi()
    {
        //Serial.println("env IV connect to wifi");
        BMP280_I2c_Bus = AsyncWiFiSettings.integer("BMP280_I2c_Bus", 1, 2, DEFAULT_I2C_BUS, "I2C Bus");
        BMP280_I2c = AsyncWiFiSettings.string("BMP280_I2c", "", "I2C address (0x76 or 0x77)");
    }

    void SerialReport()
    {
        if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
        //if (BMP280m5_I2c.isEmpty()) return;
        Serial.print("ENVIV BMP280:       ");
        Serial.println(BMP280_I2c + " on bus " + BMP280_I2c_Bus);
    }

    bool SendDiscovery()
    {
        //Serial.println("env IV send discovery");
        //if (BMP280_I2c.isEmpty()) return true;
        return sendSensorDiscovery("enviv_temperature", EC_NONE, "temperature", "°C")
            && sendSensorDiscovery("enviv_pressure", EC_NONE, "pressure", "hPa");
    }

    void Loop()
    {
        if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
        if (!initialized) return;

        if (BMP280PreviousMillis == 0 || millis() - BMP280PreviousMillis >= sensorInterval) {

            bmp.takeForcedMeasurement();
            bmp.readTemperature();
            bmp.readPressure(); 
        
            pub((roomsTopic + "/enviv_temperature").c_str(), 0, true, String(bmp.cTemp).c_str());
            pub((roomsTopic + "/enviv_pressure").c_str(), 0, true, String(bmp.pressure / 100).c_str());

            BMP280PreviousMillis = millis();
        }
    }
}

#endif
