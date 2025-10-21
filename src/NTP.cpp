#include <Arduino.h>
#include <rom/rtc.h>
#include "Logger.h"

namespace NTP {
bool ntpSynced = false;

void Setup(void) {
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");  // UTC
}

/**
 * @brief Detects when system time is synchronized with NTP and records the event.
 *
 * Checks the current system epoch time; when the time indicates synchronization, logs
 * the current UTC time and sets the global `ntpSynced` flag to true.
 *
 * @note The function treats the system time as unsynchronized while the epoch time is
 *       less than 57600 seconds (8 * 3600 * 2). Once that threshold is exceeded the
 *       time is considered synchronized.
 */
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
    Log.printf("NTP synced, current time: %s\r\n", asctime(&timeinfo));
    ntpSynced = true;
}
}  // namespace NTP