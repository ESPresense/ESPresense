#include "Battery.h"

#include <Arduino.h>
#include <HeadlessWiFiSettings.h>

#include "defaults.h"
#include "globals.h"
#include "mqtt.h"

namespace Battery {
int adcPin = DEFAULT_BATTERY_ADC_PIN;
float adcMultiplier = DEFAULT_BATTERY_ADC_MULTIPLIER;
int adcOffset = DEFAULT_BATTERY_ADC_OFFSET;
int batteryMinMv = DEFAULT_BATTERY_MIN_MV;
int batteryMaxMv = DEFAULT_BATTERY_MAX_MV;
int chargingMv = DEFAULT_BATTERY_CHARGING_MV;

int smoothMilliVolts = 0;
bool validConfig = false;

static bool isEsp32Adc2Pin(int pin) {
#if defined(CONFIG_IDF_TARGET_ESP32)
    switch (pin) {
        case 0:
        case 2:
        case 4:
        case 12:
        case 13:
        case 14:
        case 15:
        case 25:
        case 26:
        case 27:
            return true;
        default:
            return false;
    }
#else
    (void)pin;
    return false;
#endif
}

static int readBatteryMilliVolts() {
    if (!validConfig) return 0;

    int raw = analogRead(adcPin);
    int mv = round((raw + adcOffset) * adcMultiplier);
    if (smoothMilliVolts)
        smoothMilliVolts = round(0.1 * (mv - smoothMilliVolts) + smoothMilliVolts);
    else
        smoothMilliVolts = mv;

    return smoothMilliVolts;
}

void ConnectToWifi() {
    adcPin = HeadlessWiFiSettings.integer("battery_adc_pin", DEFAULT_BATTERY_ADC_PIN, "Battery ADC pin (-1 disables)");
    adcMultiplier = HeadlessWiFiSettings.floating("battery_adc_multiplier", 0, 100, DEFAULT_BATTERY_ADC_MULTIPLIER, "Battery ADC multiplier (mV per ADC count)");
    adcOffset = HeadlessWiFiSettings.integer("battery_adc_offset", DEFAULT_BATTERY_ADC_OFFSET, "Battery ADC offset (counts)");
    batteryMinMv = HeadlessWiFiSettings.integer("battery_min_mv", DEFAULT_BATTERY_MIN_MV, "Battery empty voltage (mV), 0 disables percent");
    batteryMaxMv = HeadlessWiFiSettings.integer("battery_max_mv", DEFAULT_BATTERY_MAX_MV, "Battery full voltage (mV), 0 disables percent");
    chargingMv = HeadlessWiFiSettings.integer("battery_charging_mv", DEFAULT_BATTERY_CHARGING_MV, "Charging threshold (mV), 0 disables charging status");
}

void Setup() {
    validConfig = false;
    smoothMilliVolts = 0;

    if (adcPin < 0) {
        Log.println("Battery telemetry disabled (battery_adc_pin < 0)");
        return;
    }

    if (adcMultiplier <= 0) {
        Log.println("Battery telemetry disabled: battery_adc_multiplier must be > 0");
        return;
    }

    if (digitalPinToAnalogChannel(adcPin) < 0) {
        Log.printf("Battery telemetry disabled: GPIO %d is not ADC-capable\r\n", adcPin);
        return;
    }

    if (isEsp32Adc2Pin(adcPin)) {
        Log.printf("Battery telemetry disabled: GPIO %d is ADC2 on ESP32 and conflicts with WiFi. Use ADC1 pin (32-39).\r\n", adcPin);
        return;
    }

    pinMode(adcPin, INPUT);
    validConfig = true;

    Log.printf("Battery telemetry enabled: pin=%d multiplier=%.6f offset=%d min=%d max=%d charging=%d\r\n", adcPin, adcMultiplier, adcOffset, batteryMinMv, batteryMaxMv, chargingMv);
}

bool SendDiscovery() {
    if (!validConfig) return true;

    bool ok = sendTeleSensorDiscovery("Battery Voltage", EC_DIAGNOSTIC, "{{ value_json.mV }}", "voltage", "mV");

    if (batteryMinMv > 0 && batteryMaxMv > batteryMinMv)
        ok = ok && sendTeleSensorDiscovery("Battery", EC_NONE, "{{ value_json.batt }}", "battery", "%");

    if (chargingMv > 0)
        ok = ok && sendTeleBinarySensorDiscovery("Charging", EC_NONE, "{{ value_json.charging }}", "battery_charging");

    return ok;
}

void SendTelemetry() {
    int mv = readBatteryMilliVolts();
    if (mv <= 0) return;

    doc["mV"] = mv;

    if (batteryMinMv > 0 && batteryMaxMv > batteryMinMv) {
        int soc = round((100.0f * (mv - batteryMinMv)) / (batteryMaxMv - batteryMinMv));
        doc["batt"] = max(0, min(100, soc));
    }

    if (chargingMv > 0)
        doc["charging"] = (mv >= chargingMv) ? "ON" : "OFF";
}
}  // namespace Battery
