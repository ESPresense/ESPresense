#include "GUI.h"

#include "BleFingerprintCollection.h"
#include "Display.h"
#include "LEDs.h"
#include "defaults.h"
#include "Logger.h"

namespace GUI {
/**
 * @brief Initialize GUI event routing and hardware/visual subsystems.
 *
 * Sets up BLE fingerprint event handlers to forward events to the GUI handlers
 * and initializes the display-related subsystem appropriate for the initialization stage.
 *
 * @param beforeWifi If true, register BLE event callbacks and initialize the Display;
 *                   if false, initialize the LEDs subsystem instead.
 */
void Setup(bool beforeWifi) {
    if (beforeWifi) {
        BleFingerprintCollection::onSeen = [](bool inprogress) {
            Seen(inprogress);
        };
        BleFingerprintCollection::onAdd = [](BleFingerprint *fingerprint) {
            Added(fingerprint);
        };
        BleFingerprintCollection::onDel = [](BleFingerprint *fingerprint) {
            Removed(fingerprint);
        };
        BleFingerprintCollection::onClose = [](BleFingerprint *fingerprint) {
            Close(fingerprint);
        };
        BleFingerprintCollection::onLeft = [](BleFingerprint *fingerprint) {
            Left(fingerprint);
        };
        BleFingerprintCollection::onCountAdd = [](BleFingerprint *fingerprint) {
            Counting(fingerprint, true);
        };
        BleFingerprintCollection::onCountDel = [](BleFingerprint *fingerprint) {
            Counting(fingerprint, false);
        };
        Display::Setup();
    } else {
        LEDs::Setup();
    }
}

bool SendOnline() {
    return LEDs::SendOnline();
}

void SerialReport() {
    LEDs::SerialReport();
}

void ConnectToWifi() {
    LEDs::ConnectToWifi();
}

void Loop() {
    LEDs::Loop();
}

/**
 * Handle a newly observed BLE fingerprint and emit a "New" log entry.
 *
 * If the fingerprint is marked to be ignored, no action is taken. Otherwise
 * logs a "New" event that records the core ID, allow/query flag, MAC address,
 * fingerprint identifier, and RSSI.
 *
 * @param f Pointer to the detected BleFingerprint; ignored if `f->getIgnore()` is true.
 */
void Added(BleFingerprint *f) {
    if (f->getIgnore()) return;
    Log.printf("%u New %s  | %s | %-58s%.1fdBm\r\n", xPortGetCoreID(), f->getAllowQuery() ? "Q" : " ", f->getMac().c_str(), f->getId().c_str(), f->getRssi());
}

/**
 * @brief Log a deletion event for a BLE fingerprint and ignore cases not applicable.
 *
 * Logs a decorated "Del" entry containing the core ID, device MAC, device ID, and RSSI
 * for the given fingerprint. If the fingerprint is marked to be ignored or was not
 * previously added, the function returns without logging.
 *
 * @param f Pointer to the BLE fingerprint to be removed and logged.
 */
void Removed(BleFingerprint *f) {
    if (f->getIgnore() || !f->getAdded()) return;
    Log.printf("\u001b[38;5;236m%u Del    | %s | %-58s%.1fdBm\u001b[0m\r\n", xPortGetCoreID(), f->getMac().c_str(), f->getId().c_str(), f->getRssi());
}

/**
 * @brief Handle a fingerprint "close" event by logging it and updating the display.
 *
 * Logs a green "Close" entry including core ID, MAC, identifier, and raw RSSI, and posts a short
 * status message to the display containing the fingerprint's ID.
 *
 * @param f Pointer to the BleFingerprint that closed (provides MAC, ID, and raw RSSI).
 */
void Close(BleFingerprint *f) {
    Log.printf("\u001b[32m%u Close  | %s | %-58s%.1fdBm\u001b[0m\r\n", xPortGetCoreID(), f->getMac().c_str(), f->getId().c_str(), f->getRawRssi());
    Display::Status("C:%s\r\n", f->getId().c_str());
}

/**
 * @brief Handle a fingerprint that has left the area by logging the event and updating the display.
 *
 * Logs a formatted "Left" event with core ID, MAC, identifier, and raw RSSI, and updates the on-device display status.
 *
 * @param f Pointer to the BleFingerprint representing the device that left.
 */
void Left(BleFingerprint *f) {
    Log.printf("\u001b[33m%u Left   | %s | %-58s%.1fdBm\u001b[0m\r\n", xPortGetCoreID(), f->getMac().c_str(), f->getId().c_str(), f->getRawRssi());
    Display::Status("L:%s\r\n", f->getId().c_str());
}
/**
 * @brief Logs motion sensor states and forwards them to the LEDs subsystem.
 *
 * @param pir True if the PIR sensor currently detects motion.
 * @param radar True if the radar sensor currently detects motion.
 */
void Motion(bool pir, bool radar) {
    Log.printf("%u Motion | Pir: %s Radar: %s\r\n", xPortGetCoreID(), pir ? "yes" : "no", radar ? "yes" : "no");
    LEDs::Motion(pir, radar);
}

/**
 * @brief Log the current states of two physical switches.
 *
 * Logs which core is running and whether each switch is active.
 *
 * @param switch_1 `true` if the first switch is active/on, `false` otherwise.
 * @param switch_2 `true` if the second switch is active/on, `false` otherwise.
 */
void Switch(bool switch_1, bool switch_2) {
    Log.printf("%u Switch | Switch One: %s Switch Two: %s\r\n", xPortGetCoreID(), switch_1 ? "yes" : "no", switch_2 ? "yes" : "no");
}

/**
 * @brief Logs the current states of two physical buttons.
 *
 * @param button_1 State of the first button (`true` if pressed, `false` otherwise).
 * @param button_2 State of the second button (`true` if pressed, `false` otherwise).
 */
void Button(bool button_1, bool button_2) {
    Log.printf("%u Button | Button One: %s Button Two: %s\r\n", xPortGetCoreID(), button_1 ? "yes" : "no", button_2 ? "yes" : "no");
}

/**
 * @brief Update LEDs to reflect a "seen" event state.
 *
 * @param inprogress `true` to show the 'seen' indicator as in progress on the LEDs, `false` to clear/stop it.
 */
void Seen(bool inprogress) {
    LEDs::Seen(inprogress);
}

/**
 * @brief Report firmware update progress and propagate it to LEDs, the display, and logs.
 *
 * Updates UI indicators and emits a log entry for the provided progress value.
 *
 * @param percent Progress value in percent (0–100). Special sentinel values:
 * - `UPDATE_STARTED`: marks the start of the update and sets the display to "started".
 * - `UPDATE_COMPLETE`: marks completion of the update and sets the display to "finished".
 */
void Update(unsigned int percent) {
    LEDs::Update(percent);
    if (percent == UPDATE_STARTED) {
        Log.printf("%u Update | %s\r\n", xPortGetCoreID(), "started");
        Display::Status("Update:%s\r\n", "started");
    } else if (percent == UPDATE_COMPLETE) {
        Log.printf("%u Update | %s\r\n", xPortGetCoreID(), "finished");
        Display::Status("Update:%s\r\n", "finished");
    } else {
        Log.printf("%u Update | %d%%\r\n", xPortGetCoreID(), percent);
    }
}

void Connected(bool wifi, bool mqtt) {
    Display::Status("Wifi:%s Mqtt:%s\r\n", (wifi ? "yes" : "no"), (mqtt ? "yes" : "no"));
}

/**
 * @brief Log a counting event for a BLE fingerprint indicating an increment or decrement.
 *
 * Logs a colored, formatted message containing the fingerprint's MAC, ID, RSSI, estimated distance,
 * and milliseconds since last seen, marking whether the count was incremented or decremented.
 *
 * @param f Pointer to the BleFingerprint whose data will be logged.
 * @param add If `true`, the event represents a count increment; if `false`, it represents a count decrement.
 */
void Counting(BleFingerprint *f, bool add) {
    if (add)
        Log.printf("\u001b[36m%u C# +1  | %s | %-58s%.1fdBm (%.2fm) %lums\u001b[0m\r\n", xPortGetCoreID(), f->getMac().c_str(), f->getId().c_str(), f->getRssi(), f->getDistance(), f->getMsSinceLastSeen());
    else
        Log.printf("\u001b[35m%u C# -1  | %s | %-58s%.1fdBm (%.2fm) %lums\u001b[0m\r\n", xPortGetCoreID(), f->getMac().c_str(), f->getId().c_str(), f->getRssi(), f->getDistance(), f->getMsSinceLastSeen());
}

/**
 * @brief Update the Wi‑Fi indicator with the current progress.
 *
 * @param percent Progress percentage for Wi‑Fi connection (0–100).
 */
void Wifi(unsigned int percent) {
    LEDs::Wifi(percent);
}

void Portal(unsigned int percent) {
    LEDs::Portal(percent);
}

/**
 * @brief Format and broadcast a status message to the log and the display.
 *
 * Formats a message using a printf-style format string and its arguments, prefixes the
 * log entry with the current core ID and "Status |", sends the formatted text to the
 * logging subsystem, and updates the on-device display with the same message.
 *
 * @param format printf-style format string followed by the values to be formatted.
 */
void Status(const char *format, ...) {
    char *message;
    va_list args;
    va_start(args, format);
    vasprintf(&message, format, args);
    va_end(args);
    Log.printf("%u Status | %s", xPortGetCoreID(), message);
    Display::Status(message);
    free(message);
}

bool Command(String &command, String &pay) {
    return LEDs::Command(command, pay);
}

bool SendDiscovery() {
    return LEDs::SendDiscovery();
}

void Count(unsigned int count) {
    LEDs::Count(count);
}
}  // namespace GUI