#include "Battery.h"

#include <Arduino.h>

#include "defaults.h"
#include "globals.h"
#include "mqtt.h"

namespace Battery {
#ifdef MACCHINA_A0
const int voltageDividerResistor1 = 2200;
const int voltageDividerResistor2 = 10000;
const int chargingVoltage = 13200;
const int deadVoltage = 11883;
#define BATTERY_SENSOR
#elif defined(TTGO_TCELL)
const int voltageDividerResistor1 = 100000;
const int voltageDividerResistor2 = 100000;
const int chargingVoltage = 4150;
const int deadVoltage = 3200;
#define BATTERY_SENSOR
#endif
#ifdef BATTERY_SENSOR
int smoothMilliVolts;
int a0_read_batt_mv() {
    int mv = analogReadMilliVolts(GPIO_NUM_35) * (voltageDividerResistor1 + voltageDividerResistor2) / voltageDividerResistor1;
    if (smoothMilliVolts)
        smoothMilliVolts = round(0.1 * (mv - smoothMilliVolts) + smoothMilliVolts);
    else
        smoothMilliVolts = mv;
    return smoothMilliVolts;
}
#endif

void Setup() {
#ifdef BATTERY_SENSOR
    pinMode(GPIO_NUM_35, INPUT);
#endif
}

bool SendDiscovery() {
#ifdef BATTERY_SENSOR
    return sendTeleSensorDiscovery("Battery", EC_NONE, "{{ value_json.batt }}", "battery", "%") &&
        sendTeleBinarySensorDiscovery("Charging", EC_NONE, "{{ value_json.charging }}", "battery_charging") &&
        sendTeleSensorDiscovery("Battery Voltage", EC_NONE, "{{ value_json.mV }}", "voltage", "mV");
#else
    return true;
#endif
}

void SendTelemetry() {
#ifdef BATTERY_SENSOR
    auto mv = a0_read_batt_mv();
    doc["mV"] = mv;
    bool charging = (mv > chargingVoltage);
    bool dead = (mv < deadVoltage);
#ifdef MACCHINA_A0
    unsigned int soc = round(-13275.04 + 2.049731 * mv - (0.00007847975 * mv) * mv);
#else
    unsigned int soc = round(mv < 3700 ? (-64 + 0.02 * mv) : -4266.02 + 2.021295 * mv - (0.00023382077 * mv) * mv);
#endif
    doc["batt"] = dead ? 0 : (charging ? (unsigned int)100 : max((unsigned int)0, min((unsigned int)100, soc)));
    doc["charging"] = charging ? "ON" : "OFF";
#endif
}
}  // namespace Battery
