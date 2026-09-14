#ifdef SENSORS
#include "DHT.h"

#include "Settings.h"
#include "defaults.h"
#include "esp_rom_sys.h"
#include "globals.h"
#include "mqtt.h"

// Self-contained DHT11/DHT22 single-wire reader, sampled from Loop() every dhtUpdateTime seconds.
namespace DHT {
int dht11Pin = -1;
int dht22Pin = -1;
float dhtTempOffset;
float dhtHumidityOffset;

/* update time */
int dhtUpdateTime = 10;  // ToDo: maybe make this a user choise via settings menu
unsigned long lastReadMillis = 0;

static portMUX_TYPE dhtMux = portMUX_INITIALIZER_UNLOCKED;

// Busy-wait until the pin reads `level`; returns the microseconds it took or -1 on timeout.
static int waitLevel(int pin, int level, int timeoutUs) {
    int t = 0;
    while (gpio_get_level((gpio_num_t)pin) != level) {
        if (++t > timeoutUs) return -1;
        esp_rom_delay_us(1);
    }
    return t;
}

// Reads the 40-bit frame. Returns false on timeout or checksum mismatch.
static bool readFrame(int pin, bool isDht11, uint8_t data[5]) {
    // Host start signal: pull low (>=18 ms for DHT11, >=1 ms for DHT22), then release.
    gpio_set_direction((gpio_num_t)pin, GPIO_MODE_OUTPUT_OD);
    gpio_set_level((gpio_num_t)pin, 0);
    delay(isDht11 ? 20 : 2);
    gpio_set_level((gpio_num_t)pin, 1);
    gpio_set_direction((gpio_num_t)pin, GPIO_MODE_INPUT);

    memset(data, 0, 5);
    bool ok = true;
    taskENTER_CRITICAL(&dhtMux);
    // Sensor response: low ~80 us, high ~80 us.
    if (waitLevel(pin, 0, 100) < 0 || waitLevel(pin, 1, 120) < 0 || waitLevel(pin, 0, 120) < 0) ok = false;
    for (int i = 0; ok && i < 40; i++) {
        // Each bit: ~50 us low, then 26-28 us high (0) or ~70 us high (1).
        if (waitLevel(pin, 1, 100) < 0) { ok = false; break; }
        int high = waitLevel(pin, 0, 120);
        if (high < 0) { ok = false; break; }
        if (high > 40) data[i / 8] |= 0x80 >> (i % 8);
    }
    taskEXIT_CRITICAL(&dhtMux);

    if (!ok) return false;
    return (uint8_t)(data[0] + data[1] + data[2] + data[3]) == data[4];
}

void Setup() {
    if (dht11Pin < 0 && dht22Pin < 0) return;
    // ponytail: the old code held one DHTesp instance, so with both pins set the DHT22 won.
    int pin = dht22Pin >= 0 ? dht22Pin : dht11Pin;
    pinMode(pin, INPUT_PULLUP);
}

void ConnectToWifi(bool updating) {
    dht11Pin = Settings::integer("dht11_pin", -1, "DHT11 sensor pin (-1 for disable)");
    dht22Pin = Settings::integer("dht22_pin", -1, "DHT22 sensor pin (-1 for disable)");
    dhtTempOffset = Settings::floating("dhtTemp_offset", -40, 125, 0.0, "DHT temperature offset");
    dhtHumidityOffset = Settings::floating("dhtHumidity_offset", -100, 100, 0.0, "DHT humidity offset");
}

void SerialReport() {
    if (dht11Pin < 0 && dht22Pin < 0) return;
    Log.print("DHT11 Sensor: ");
    Log.println(dht11Pin >= 0 ? "pin " + toStr(dht11Pin) : std::string("disabled"));
    Log.print("DHT22 Sensor: ");
    Log.println(dht22Pin >= 0 ? "pin " + toStr(dht22Pin) : std::string("disabled"));
    Log.print("DHT Temp Offset:   ");
    Log.println(dhtTempOffset);
    Log.print("DHT Humidity Offset: ");
    Log.println(dhtHumidityOffset);
}

void Loop() {
    if (dht11Pin < 0 && dht22Pin < 0) return;
    if (millis() - lastReadMillis < (unsigned long)dhtUpdateTime * 1000) return;
    lastReadMillis = millis();

    bool isDht11 = dht22Pin < 0;
    int pin = isDht11 ? dht11Pin : dht22Pin;
    uint8_t d[5];
    if (!readFrame(pin, isDht11, d)) {
        Log.println("[DHT] read failed (timeout or bad checksum)");
        return;
    }

    float humidity, temperature;
    if (isDht11) {
        humidity = d[0] + d[1] * 0.1f;
        temperature = d[2] + (d[3] & 0x7F) * 0.1f;
        if (d[3] & 0x80) temperature = -temperature;
    } else {
        humidity = ((d[0] << 8) | d[1]) * 0.1f;
        temperature = (((d[2] & 0x7F) << 8) | d[3]) * 0.1f;
        if (d[2] & 0x80) temperature = -temperature;
    }
    humidity += dhtHumidityOffset;
    temperature += dhtTempOffset;
    Log.println("Temp: " + toStr(temperature, 1) + "'C Humidity: " + toStr(humidity, 1) + "%");

    pub((roomsTopic + "/humidity").c_str(), 0, 1, toStr(humidity, 1).c_str());
    pub((roomsTopic + "/temperature").c_str(), 0, 1, toStr(temperature, 1).c_str());
}

bool SendDiscovery() {
    if (dht11Pin < 0 && dht22Pin < 0) return true;

    return sendSensorDiscovery("Temperature", EC_NONE, "temperature", "°C") && sendSensorDiscovery("Humidity", EC_NONE, "humidity", "%");
}

bool Command(std::string& command, std::string& pay) {
    return false;
}

bool SendOnline() {
    return true;
}
}  // namespace DHT
#endif
