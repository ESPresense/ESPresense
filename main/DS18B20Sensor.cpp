#ifdef SENSORS
#include "DS18B20Sensor.h"

#include <vector>

#include "Settings.h"
#include "defaults.h"
#include "ds18b20.h"
#include "globals.h"
#include "mqtt.h"
#include "onewire_bus.h"

// DS18B20 over the onewire_bus (RMT) + ds18b20 managed components, driven from Loop()
// as a small state machine instead of the old FreeRTOS task.
namespace DS18B20 {
int ds18b20Pin = -1;
float dsTempOffset;
int numSensors = 0;

onewire_bus_handle_t bus = nullptr;
std::vector<ds18b20_device_handle_t> devices;

/** wait before first read, then set to true */
bool initialReadDone = false;

/** update temp every 20 seconds */
int dsUpdateTimeS = 20;

/** setup delay before first read to give the sensor time to settle after initialization, seconds */
int dsFirstReadDelayS = 2;

/** wait for temps delay, milliseconds; datasheet says 750ms max */
int dsWaitTimeMs = 750;

bool conversionPending = false;
unsigned long conversionMillis = 0;
unsigned long lastRequestMillis = 0;

void Setup() {
    if (ds18b20Pin < 0) return;

    onewire_bus_config_t busCfg = {};
    busCfg.bus_gpio_num = ds18b20Pin;
    busCfg.flags.en_pull_up = true;
    onewire_bus_rmt_config_t rmtCfg = {};
    rmtCfg.max_rx_bytes = 10;  // 1 byte ROM command + 8 bytes ROM + 1 byte scratchpad command
    esp_err_t err = onewire_new_bus_rmt(&busCfg, &rmtCfg, &bus);
    if (err != ESP_OK) {
        Log.println("[ERROR] DS18B20 1-Wire bus init failed");
        bus = nullptr;
        return;
    }

    onewire_device_iter_handle_t iter = nullptr;
    if (onewire_new_device_iter(bus, &iter) == ESP_OK) {
        onewire_device_t dev;
        while (onewire_device_iter_get_next(iter, &dev) == ESP_OK) {
            ds18b20_config_t cfg = {};
            ds18b20_device_handle_t h;
            if (ds18b20_new_device_from_enumeration(&dev, &cfg, &h) == ESP_OK) devices.push_back(h);
        }
        onewire_del_device_iter(iter);
    }
    numSensors = (int)devices.size();

    if (numSensors == 0) {
        Log.println("[ERROR] No DS sensors found");
        return;
    }
}

void ConnectToWifi(bool updating) {
    ds18b20Pin = Settings::integer("ds18b20_pin", -1, "DS18B20 sensor pin (-1 for disable)");
    dsTempOffset = Settings::floating("dsTemp_offset", -40, 125, 0.0, "DS18B20 temperature offset");
}

void SerialReport() {
    if (ds18b20Pin < 0) return;
    Log.print("DS18B20 Sensor: ");
    Log.println(ds18b20Pin >= 0 ? "pin " + toStr(ds18b20Pin) : std::string("disabled"));
    Log.print("DS18B20 Offset:   ");
    Log.println(dsTempOffset);
}

// Raw reading for sensor i; -127 when the read fails (DallasTemperature's DEVICE_DISCONNECTED_C).
static float readTemp(int i) {
    float t;
    if (ds18b20_get_temperature(devices[i], &t) != ESP_OK) return -127;
    return t;
}

void Loop() {
    if (ds18b20Pin < 0 || numSensors == 0) return;

    if (!conversionPending) {
        unsigned long interval = initialReadDone ? (unsigned long)dsUpdateTimeS * 1000 : (unsigned long)dsFirstReadDelayS * 1000;
        if (lastRequestMillis != 0 && millis() - lastRequestMillis < interval) return;
        ds18b20_trigger_temperature_conversion_for_all(bus);
        lastRequestMillis = millis();
        conversionMillis = millis();
        conversionPending = true;
        return;
    }

    // since the sensor is slow (up to 750ms to read the temperature), no point in checking too often
    // ponytail: wait by timestamp instead of the old blocking delay() in the main loop.
    if (millis() - conversionMillis < (unsigned long)dsWaitTimeMs) return;
    conversionPending = false;

    // avoid 85 C readings on initialization
    if (!initialReadDone) {
        bool allReadsOk = true;
        for (int i = 0; i < numSensors; i++) {
            if (readTemp(i) == 85) allReadsOk = false;
        }
        if (!allReadsOk) return;  // retry after dsFirstReadDelayS
        initialReadDone = true;
    }

    for (int i = 0; i < numSensors; i++) {
        float rawTemp = readTemp(i);
        float temperature = rawTemp + dsTempOffset;
        Log.println("DS18B20 Temp_" + toStr(i + 1) + ": " + toStr(temperature, 1) + "'C");
        if (rawTemp > -127)  // Skip null values
        {
            pub((roomsTopic + "/ds18b20_temperature_" + toStr(i + 1)).c_str(), 0, 1, toStr(temperature, 1).c_str());
        }
    }
}

bool SendDiscovery() {
    if (ds18b20Pin < 0) return true;
    bool returnVal = true;

    for (int i = 0; i < numSensors; i++) {
        returnVal = sendSensorDiscovery("DS18B20 Temperature " + toStr(i + 1), EC_NONE, "temperature", "°C") && returnVal;
    }
    return returnVal;
}

bool Command(std::string& command, std::string& pay) {
    return false;
}

bool SendOnline() {
    return true;
}
}  // namespace DS18B20
#endif
