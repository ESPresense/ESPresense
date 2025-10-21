#ifdef SENSORS

#include "I2C.h"

#include <Adafruit_BME280.h>
#include <HeadlessWiFiSettings.h>

#include "defaults.h"
#include "globals.h"
#include "mqtt.h"
#include "string_utils.h"

namespace I2C {
bool I2CDebug = false;
int I2C_Bus_1_SDA = 0;
int I2C_Bus_1_SCL = 0;
int I2C_Bus_2_SDA = 0;
int I2C_Bus_2_SCL = 0;

void ConnectToWifi() {

    I2C_Bus_1_SDA = HeadlessWiFiSettings.integer("I2C_Bus_1_SDA", -1, 39, DEFAULT_I2C_BUS_1_SDA, "SDA pin (-1 to disable)");
    I2C_Bus_1_SCL = HeadlessWiFiSettings.integer("I2C_Bus_1_SCL", -1, 39, DEFAULT_I2C_BUS_1_SCL, "SCL pin (-1 to disable)");

    I2C_Bus_2_SDA = HeadlessWiFiSettings.integer("I2C_Bus_2_SDA", -1, 39, DEFAULT_I2C_BUS_2_SDA, "SDA pin (-1 to disable)");
    I2C_Bus_2_SCL = HeadlessWiFiSettings.integer("I2C_Bus_2_SCL", -1, 39, DEFAULT_I2C_BUS_2_SCL, "SCL pin (-1 to disable)");

    I2CDebug = HeadlessWiFiSettings.checkbox("I2CDebug", false, "Debug I2C addreses. Look at the serial log to get the correct address");

    if (I2C_Bus_1_SDA != -1 && I2C_Bus_1_SDA != -1) {
        I2C_Bus_1_Started = Wire.begin(I2C_Bus_1_SDA, I2C_Bus_1_SCL);
    }

    if (I2C_Bus_2_SDA != -1 && I2C_Bus_2_SDA != -1) {
        I2C_Bus_2_Started = Wire1.begin(I2C_Bus_2_SDA, I2C_Bus_2_SCL);
    }
}

void Setup() {
}

/**
 * @brief Reports configured I2C buses and, when debugging is enabled, scans started buses for devices.
 *
 * @details Logs the SDA/SCL pin assignments for any started I2C buses. If at least one bus is started and the
 * debug flag is enabled, performs an address scan on each started bus and logs discovered device addresses
 * and any unknown errors encountered during probing. If no devices are found, logs a corresponding message.
 */
void SerialReport() {
    if (I2C_Bus_1_Started)
        Log.println(String("I2C Bus 1:    sda=") + I2C_Bus_1_SDA + " scl=" + I2C_Bus_1_SCL);
    if (I2C_Bus_2_Started)
        Log.println(String("I2C Bus 2:    sda=") + I2C_Bus_2_SDA + " scl=" + I2C_Bus_2_SCL);

    if (!I2C_Bus_1_Started && !I2C_Bus_2_Started) return;
    if (!I2CDebug) return;
    byte error, address;
    int nDevices;
    nDevices = 0;

    if (I2C_Bus_1_Started) {
        Log.println("Scanning I2C for devices on Bus 1...");
        for (address = 1; address < 127; address++) {
            Wire.beginTransmission(address);
            error = Wire.endTransmission();
            if (error == 0) {
                Log.print("I2C device found on bus 1 at address 0x");

                if (address < 16) {
                    Log.print("0");
                }

                Log.println(address, HEX);
                nDevices++;
            } else if (error == 4) {
                Log.print("Unknown error on bus 1 at address 0x");
                if (address < 16) {
                    Log.print("0");
                }
                Log.println(address, HEX);
            }
        }
    }

    if (I2C_Bus_2_Started) {
        Log.println("Scanning I2C for devices on Bus 2...");

        for (address = 1; address < 127; address++) {
            Wire1.beginTransmission(address);
            error = Wire1.endTransmission();
            if (error == 0) {
                Log.print("I2C device found on bus 2 at address 0x");

                if (address < 16) {
                    Log.print("0");
                }

                Log.println(address, HEX);
                nDevices++;
            } else if (error == 4) {
                Log.print("Unknown error on bus 2 at address 0x");
                if (address < 16) {
                    Log.print("0");
                }
                Log.println(address, HEX);
            }
        }
    }

    if (nDevices == 0) {
        Log.println("No I2C devices found\r\n");
    }
}
}  // namespace I2C

#endif