#include <Arduino.h>
#include <rom/rtc.h>

namespace NTP {
bool ntpSynced = false;

void Setup(void) {
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");  // UTC
}

void Loop(void) {
    if (ntpSynced)
        return;

    time_t now = time(nullptr);
    if (now < 8 * 3600 * 2) {
        // Time not yet synced
        return;
    }

    // Time synced - log it and set flag
    struct tm timeinfo;
    gmtime_r(&now, &timeinfo);
    Serial.printf("NTP synced, current time: %s\r\n", asctime(&timeinfo));
    ntpSynced = true;
}
}  // namespace NTP
