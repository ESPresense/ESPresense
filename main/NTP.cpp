#include "NTP.h"

#include <ctime>

#include "Logger.h"
#include "esp_netif_sntp.h"

namespace NTP {
bool ntpSynced = false;

void Setup(void) {
    // UTC; non-blocking start, Loop() notices when the clock has been set.
    esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG_MULTIPLE(2, ESP_SNTP_SERVER_LIST("pool.ntp.org", "time.nist.gov"));
    config.start = true;
    config.wait_for_sync = false;
    esp_netif_sntp_init(&config);
}

/**
 * @brief Detects when system time is synchronized with NTP and records the event.
 *
 * The system time is treated as unsynchronized while the epoch time is less than
 * 57600 seconds (8 * 3600 * 2). Once that threshold is exceeded the time is
 * considered synchronized, logged once, and `ntpSynced` is set.
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
