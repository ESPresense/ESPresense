#include "Battery.h"

#include <algorithm>
#include <cmath>

#include "defaults.h"
#include "globals.h"
#include "mqtt.h"
#ifdef MACCHINA_A0
#include "esp_adc/adc_oneshot.h"
#endif

namespace Battery {
#ifdef MACCHINA_A0
// GPIO35 = ADC1 channel 7; 12-bit, 12 dB attenuation (what Arduino analogRead defaulted to).
static adc_oneshot_unit_handle_t adc = nullptr;
int smoothMilliVolts;

static int analogRead35() {
    int raw = 0;
    if (adc) adc_oneshot_read(adc, ADC_CHANNEL_7, &raw);
    return raw;
}

int a0_read_batt_mv() {
    int mv = round(((float)analogRead35() + 35) / 0.215);
    if (smoothMilliVolts)
        smoothMilliVolts = round(0.1 * (mv - smoothMilliVolts) + smoothMilliVolts);
    else
        smoothMilliVolts = mv;
    return smoothMilliVolts;
}
#endif

void Setup() {
#ifdef MACCHINA_A0
    adc_oneshot_unit_init_cfg_t unitCfg = {};
    unitCfg.unit_id = ADC_UNIT_1;
    unitCfg.ulp_mode = ADC_ULP_MODE_DISABLE;
    if (adc_oneshot_new_unit(&unitCfg, &adc) != ESP_OK) {
        log_e("Battery: ADC init failed");
        adc = nullptr;
        return;
    }
    adc_oneshot_chan_cfg_t chanCfg = {};
    chanCfg.atten = ADC_ATTEN_DB_12;
    chanCfg.bitwidth = ADC_BITWIDTH_12;
    adc_oneshot_config_channel(adc, ADC_CHANNEL_7, &chanCfg);
#endif
}

bool SendDiscovery() {
#ifdef MACCHINA_A0
    return sendTeleSensorDiscovery("Battery", EC_NONE, "{{ value_json.batt }}", "battery", "%") && sendTeleBinarySensorDiscovery("Charging", EC_NONE, "{{ value_json.charging }}", "battery_charging");
#else
    return true;
#endif
}

void SendTelemetry() {
#ifdef MACCHINA_A0
    auto mv = a0_read_batt_mv();
    doc["mV"] = mv;
    bool charging = (mv > 13200);
    bool dead = (mv < 11883);
    unsigned int soc = round(-13275.04 + 2.049731 * mv - (0.00007847975 * mv) * mv);
    doc["batt"] = dead ? 0 : (charging ? (unsigned int)100 : std::max((unsigned int)0, std::min((unsigned int)100, soc)));
    doc["charging"] = charging ? "ON" : "OFF";
#endif
}
}  // namespace Battery
