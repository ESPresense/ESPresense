#include <Arduino.h>
#include <rom/rtc.h>


namespace NTP {
void Setup(void) {
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");  // UTC

    time_t now = time(nullptr);
    while (now < 8 * 3600 * 2) {
        yield();
        delay(500);
        now = time(nullptr);
    }

    struct tm timeinfo;
    gmtime_r(&now, &timeinfo);
    log_i("NTP synced, current time: %s", asctime(&timeinfo));
}
}  // namespace NTP
