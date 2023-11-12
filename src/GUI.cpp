#include "GUI.h"

#include "BleFingerprintCollection.h"
#include "Display.h"
#include "LEDs.h"
#include "defaults.h"

namespace GUI {
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

void Added(BleFingerprint *f) {
    if (f->getIgnore()) return;
    Serial.printf("%u New %s  | %s | %-58s%ddBm\r\n", xPortGetCoreID(), f->getAllowQuery() ? "Q" : " ", f->getMac().c_str(), f->getId().c_str(), f->getRssi());
}

void Removed(BleFingerprint *f) {
    if (f->getIgnore() || !f->getAdded()) return;
    Serial.printf("\u001b[38;5;236m%u Del    | %s | %-58s%ddBm\u001b[0m\r\n", xPortGetCoreID(), f->getMac().c_str(), f->getId().c_str(), f->getRssi());
}

void Close(BleFingerprint *f) {
    Serial.printf("\u001b[32m%u Close  | %s | %-58s%ddBm\u001b[0m\r\n", xPortGetCoreID(), f->getMac().c_str(), f->getId().c_str(), f->getRawRssi());
    Display::Status("C:%s\r\n", f->getId().c_str());
}

void Left(BleFingerprint *f) {
    Serial.printf("\u001b[33m%u Left   | %s | %-58s%ddBm\u001b[0m\r\n", xPortGetCoreID(), f->getMac().c_str(), f->getId().c_str(), f->getRawRssi());
    Display::Status("L:%s\r\n", f->getId().c_str());
}
void Motion(bool pir, bool radar) {
    Serial.printf("%u Motion | Pir: %s Radar: %s\r\n", xPortGetCoreID(), pir ? "yes" : "no", radar ? "yes" : "no");
    LEDs::Motion(pir, radar);
}

void Switch(bool switch_1, bool switch_2) {
    Serial.printf("%u Switch | Switch One: %s Switch Two: %s\r\n", xPortGetCoreID(), switch_1 ? "yes" : "no", switch_2 ? "yes" : "no");
}

void Button(bool button_1, bool button_2) {
    Serial.printf("%u Button | Button One: %s Button Two: %s\r\n", xPortGetCoreID(), button_1 ? "yes" : "no", button_2 ? "yes" : "no");
}

void Seen(bool inprogress) {
    LEDs::Seen(inprogress);
}

void Update(unsigned int percent) {
    LEDs::Update(percent);
    if (percent == UPDATE_STARTED) {
        Serial.printf("%u Update | %s\r\n", xPortGetCoreID(), "started");
        Display::Status("Update:%s\r\n", "started");
    } else if (percent == UPDATE_COMPLETE) {
        Serial.printf("%u Update | %s\r\n", xPortGetCoreID(), "finished");
        Display::Status("Update:%s\r\n", "finished");
    } else {
        Serial.printf("%u Update | %d%%\r\n", xPortGetCoreID(), percent);
    }
}

void Connected(bool wifi, bool mqtt) {
    Display::Status("Wifi:%s Mqtt:%s\r\n", (wifi ? "yes" : "no"), (mqtt ? "yes" : "no"));
}

void Counting(BleFingerprint *f, bool add) {
    if (add)
        Serial.printf("\u001b[36m%u C# +1  | %s | %-58s%ddBm (%.2fm) %lums\u001b[0m\r\n", xPortGetCoreID(), f->getMac().c_str(), f->getId().c_str(), f->getRssi(), f->getDistance(), f->getMsSinceLastSeen());
    else
        Serial.printf("\u001b[35m%u C# -1  | %s | %-58s%ddBm (%.2fm) %lums\u001b[0m\r\n", xPortGetCoreID(), f->getMac().c_str(), f->getId().c_str(), f->getRssi(), f->getDistance(), f->getMsSinceLastSeen());
}

void Wifi(unsigned int percent) {
    LEDs::Wifi(percent);
}

void Portal(unsigned int percent) {
    LEDs::Portal(percent);
}

void Status(const char *format, ...) {
    char *message;
    va_list args;
    va_start(args, format);
    vasprintf(&message, format, args);
    va_end(args);
    Serial.printf("%u Status | %s", xPortGetCoreID(), message);
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
