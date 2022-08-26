namespace Battery {
#ifdef MACCHINA_A0
int smoothMilliVolts;
int a0_read_batt_mv() {
    int mv = round(((float)analogRead(GPIO_NUM_35) + 35) / 0.215);
    if (smoothMilliVolts)
        smoothMilliVolts = round(0.1 * (mv - smoothMilliVolts) + smoothMilliVolts);
    else
        smoothMilliVolts = mv;
    return smoothMilliVolts;
}
#endif

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
    unsigned int soc = round(-13275.04 + 2.049731 * mv - (0.00007847975 * mv) * mv);
    doc["batt"] = charging ? (unsigned int)100 : max((unsigned int)0, min((unsigned int)100, soc));
    doc["charging"] = charging ? "ON" : "OFF";
#endif
}
}  // namespace Battery
