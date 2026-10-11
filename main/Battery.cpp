#include "Battery.h"

#include <algorithm>
#include <cmath>

#include "defaults.h"
#include "globals.h"
#include "mqtt.h"
#include "Settings.h"
#include "AXP192.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_adc/adc_oneshot.h"

namespace Battery {
// A battery on an ADC1 pin behind a divider: a single Li-ion cell (e.g. LILYGO T-Energy-S3:
// GPIO3, x2) or a 12 V lead-acid (car) battery.
static int battPin = -1;
static bool leadAcid = false;
static float battMult = 2;
static adc_oneshot_unit_handle_t adc = nullptr;
static adc_cali_handle_t cali = nullptr;
static adc_channel_t channel;
static int smoothMilliVolts = 0;

// An AXP192 (M5StickC) measures the cell itself; otherwise an ADC pin does.
static bool enabled() { return adc != nullptr || AXP192::Ready(); }

static int readMilliVolts() {
    if (AXP192::Ready()) return AXP192::BatteryMilliVolts();
    int raw = 0, mv = 0;
    if (adc_oneshot_read(adc, channel, &raw) != ESP_OK) return smoothMilliVolts;
    if (!cali || adc_cali_raw_to_voltage(cali, raw, &mv) != ESP_OK) mv = raw * 3100 / 4095;
    mv = (int)lround(mv * battMult);
    smoothMilliVolts = smoothMilliVolts ? (int)lround(0.1 * (mv - smoothMilliVolts) + smoothMilliVolts) : mv;
    return smoothMilliVolts;
}

// Resting Li-ion voltage to state of charge, linear between points.
static unsigned int liIonPercent(int mv) {
    static const int curve[][2] = {{4200, 100}, {4100, 90}, {4000, 78}, {3900, 65}, {3800, 50}, {3700, 32}, {3600, 15}, {3500, 5}, {3300, 0}};
    if (mv >= curve[0][0]) return 100;
    for (size_t i = 1; i < sizeof(curve) / sizeof(curve[0]); i++)
        if (mv >= curve[i][0])
            return curve[i][1] + (mv - curve[i][0]) * (curve[i - 1][1] - curve[i][1]) / (curve[i - 1][0] - curve[i][0]);
    return 0;
}

// 12 V lead-acid: charging above 13.2 V, empty below 11.88 V, a fitted curve in between.
static unsigned int leadAcidPercent(int mv) {
    if (mv > 13200) return 100;
    if (mv < 11883) return 0;
    double soc = -13275.04 + 2.049731 * mv - 0.00007847975 * mv * mv;
    return (unsigned int)std::clamp(lround(soc), 0L, 100L);
}

void ConnectToWifi(bool updating) {
    battPin = Settings::integer("batt_pin", -1, 48, -1, "Battery voltage pin (-1 to disable)");
    battMult = Settings::floating("batt_mult", 1, 20, 2, "Battery voltage divider (multiplier)");
    leadAcid = Settings::dropdown("batt_type", {"Li-ion (1 cell)", "12V lead-acid"}, 0, "Battery type") == 1;
}

void Setup() {
    if (battPin < 0) return;
    adc_unit_t unit;
    if (adc_oneshot_io_to_channel(battPin, &unit, &channel) != ESP_OK || unit != ADC_UNIT_1) {
        // ADC2 is shared with WiFi, so only ADC1 pins can be read while connected.
        Log.printf("Battery: GPIO %d is not an ADC1 pin\n", battPin);
        return;
    }
    adc_oneshot_unit_init_cfg_t unitCfg = {};
    unitCfg.unit_id = ADC_UNIT_1;
    if (adc_oneshot_new_unit(&unitCfg, &adc) != ESP_OK) {
        Log.printf("Battery: ADC init failed\n");
        adc = nullptr;
        return;
    }
    adc_oneshot_chan_cfg_t chanCfg = {};
    chanCfg.atten = ADC_ATTEN_DB_12;
    chanCfg.bitwidth = ADC_BITWIDTH_12;
    adc_oneshot_config_channel(adc, channel, &chanCfg);
#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    adc_cali_curve_fitting_config_t caliCfg = {};
    caliCfg.unit_id = ADC_UNIT_1;
    caliCfg.chan = channel;
    caliCfg.atten = ADC_ATTEN_DB_12;
    caliCfg.bitwidth = ADC_BITWIDTH_12;
    if (adc_cali_create_scheme_curve_fitting(&caliCfg, &cali) != ESP_OK) cali = nullptr;
#elif ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
    adc_cali_line_fitting_config_t caliCfg = {};
    caliCfg.unit_id = ADC_UNIT_1;
    caliCfg.atten = ADC_ATTEN_DB_12;
    caliCfg.bitwidth = ADC_BITWIDTH_12;
    if (adc_cali_create_scheme_line_fitting(&caliCfg, &cali) != ESP_OK) cali = nullptr;
#endif
    readMilliVolts();
}

bool SendDiscovery() {
    // Disabled: remove what an earlier configuration may have announced.
    if (!enabled())
        return sendDeleteDiscovery("sensor", "Battery") && sendDeleteDiscovery("sensor", "Battery Voltage")
            && sendDeleteDiscovery("binary_sensor", "Charging");
    return sendTeleSensorDiscovery("Battery", EC_NONE, "{{ value_json.batt }}", "battery", "%")
        && sendTeleSensorDiscovery("Battery Voltage", EC_DIAGNOSTIC, "{{ value_json.mV }}", "voltage", "mV")
        && (leadAcid || AXP192::Ready() ? sendTeleBinarySensorDiscovery("Charging", EC_NONE, "{{ value_json.charging }}", "battery_charging")
                     : sendDeleteDiscovery("binary_sensor", "Charging"));
}

void SendTelemetry() {
    if (!enabled()) return;
    auto mv = readMilliVolts();
    if (mv < 0) return;  // AXP192 with no battery fitted
    doc["mV"] = mv;
    if (AXP192::Ready()) {
        doc["batt"] = liIonPercent(mv);
        doc["charging"] = AXP192::Charging() ? "ON" : "OFF";
    } else if (leadAcid) {
        doc["batt"] = leadAcidPercent(mv);
        doc["charging"] = mv > 13200 ? "ON" : "OFF";
    } else {
        doc["batt"] = liIonPercent(mv);
    }
}
}  // namespace Battery
