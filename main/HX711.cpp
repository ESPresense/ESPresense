#ifdef SENSORS
#include "HX711.h"

#include "Settings.h"
#include "defaults.h"
#include "esp_rom_sys.h"
#include "globals.h"
#include "mqtt.h"

namespace HX711 {
int sckPin = -1;
int doutPin = -1;
int gain = 1;
int sensorInterval = 5000;
unsigned long lastMillis = 0;

static portMUX_TYPE hxMux = portMUX_INITIALIZER_UNLOCKED;

void Setup() {
    if (sckPin < 0 || doutPin < 0 || (!sckPin && !doutPin)) return;  // 0/0 was "off" before -1
    pinMode(doutPin, INPUT);
    pinMode(sckPin, OUTPUT);
    digitalWrite(sckPin, LOW);
}

void ConnectToWifi(bool updating) {
    sckPin = Settings::pin("HX711_sckPin", PinUse::Output, -1, "HX711 SCK (Clock) pin");
    doutPin = Settings::pin("HX711_doutPin", PinUse::Input, -1, "HX711 DOUT (Data) pin");
}

void SerialReport() {
    if (sckPin < 0 || doutPin < 0 || (!sckPin && !doutPin)) return;  // 0/0 was "off" before -1
    Log.print("HX711 Weight Sensor: ");
    Log.println(toStr(sckPin) + "/" + toStr(doutPin));
}

void Loop() {
    if (sckPin < 0 || doutPin < 0 || (!sckPin && !doutPin)) return;  // 0/0 was "off" before -1
    if (millis() - lastMillis < (unsigned long)sensorInterval) return;
    if (digitalRead(doutPin) != LOW) {
        // HX711 is not ready for new measurements yet!
        return;
    }

    lastMillis = millis();

    uint32_t data = 0;

    // SCK held high >60 us powers the chip down, so keep the clocking uninterruptible.
    taskENTER_CRITICAL(&hxMux);
    for (uint8_t i = 0; i < 24; i++) {
        digitalWrite(sckPin, HIGH);
        esp_rom_delay_us(1);
        data |= uint32_t(digitalRead(doutPin)) << (23 - i);
        digitalWrite(sckPin, LOW);
        esp_rom_delay_us(1);
    }

    // Cycle clock pin for gain setting
    for (uint8_t i = 0; i < gain; i++) {
        digitalWrite(sckPin, HIGH);
        esp_rom_delay_us(1);
        digitalWrite(sckPin, LOW);
        esp_rom_delay_us(1);
    }
    taskEXIT_CRITICAL(&hxMux);

    if (data & 0x800000ULL) data |= 0xFF000000ULL;

    pub((roomsTopic + "/raw_weight").c_str(), 0, true, toStr((unsigned)data).c_str());
}

bool SendDiscovery() {
    if (!sckPin && !doutPin) return true;
    return sendSensorDiscovery("Raw Weight", EC_NONE);
}
}  // namespace HX711

#endif
