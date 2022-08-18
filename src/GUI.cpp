#include "GUI.h"

#include "BleFingerprintCollection.h"
#include "Display.h"
#include "LEDs.h"

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
            Count(fingerprint, true);
        };
        BleFingerprintCollection::onCountDel = [](BleFingerprint *fingerprint) {
            Count(fingerprint, false);
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
    Display::ConnectToWifi();
}
void Loop() {
    LEDs::Loop();
    Display::Loop();
}
void Status(const char *message, ...);
void Added(BleFingerprint *f) {
    if (f->getIgnore()) return;
    Serial.printf("%u New %s | MAC: %s, ID: %-58s%ddBm %s\n", xPortGetCoreID(), f->getRmAsst() ? "R" : (f->getAllowQuery() ? "Q" : " "), f->getMac().c_str(), f->getId().c_str(), f->getRssi(), f->getDiscriminator().c_str());
}
void Removed(BleFingerprint *f) {
    if (f->getIgnore() || !f->getAdded()) return;
    Serial.printf("\u001b[38;5;236m%u Del   | MAC: %s, ID: %-58s%ddBm %s\u001b[0m\n", xPortGetCoreID(), f->getMac().c_str(), f->getId().c_str(), f->getRssi(), f->getDiscriminator().c_str());
}
void Close(BleFingerprint *f) {
    Serial.printf("\u001b[32m%u Close | MAC: %s, ID: %-58s%ddBm\u001b[0m\n", xPortGetCoreID(), f->getMac().c_str(), f->getId().c_str(), f->getNewestRssi());
    Display::Status("C:%s\n", f->getId().c_str());
}

void Left(BleFingerprint *f) {
    Serial.printf("\u001b[33m%u Left  | MAC: %s, ID: %-58s%ddBm\u001b[0m\n", xPortGetCoreID(), f->getMac().c_str(), f->getId().c_str(), f->getNewestRssi());
    Display::Status("L:%s\n", f->getId().c_str());
}
void Motion(bool pir, bool radar) {
    Serial.printf("%u Motion| Pir: %s Radar: %s\n", xPortGetCoreID(), pir ? "yes" : "no", radar ? "yes" : "no");
    Display::Status("Pir:%s Radar:%s\n", pir ? "yes" : "no", radar ? "yes" : "no");
}
void Erase(bool inprogress) {
    Serial.println(F("Resetting back to defaults..."));

    if (inprogress)
        Status("Erasing...");
    else
        Status("Erased");
}
void Seen(bool inprogress) {
    LEDs::Seen(inprogress);
}

void Update(bool inprogress) {
    LEDs::Update(inprogress);
    if (inprogress) {
        Serial.printf("%u Update| %s\n", xPortGetCoreID(), "started");
        Status("Update:%s", "started");
    } else {
        Serial.printf("%u Update| %s\n", xPortGetCoreID(), "finished");
        Status("Update:%s", "finished");
    }
}
void UpdateProgress(unsigned int percent) {
    LEDs::UpdateProgress(percent);
    Serial.printf("%u Update| %d%%\n", xPortGetCoreID(), percent);
}

void Connected(bool wifi, bool mqtt) {
    Status("Wifi:%s Mqtt:%s\n", (wifi ? "yes" : "no"), (mqtt ? "yes" : "no"));
}

void Count(BleFingerprint *f, bool add) {
    if (add)
        Serial.printf("\u001b[36m%u C# +1 | MAC: %s, ID: %-58s%ddBm (%.2fm) %lums\u001b[0m\n", xPortGetCoreID(), f->getMac().c_str(), f->getId().c_str(), f->getRssi(), f->getDistance(), f->getMsSinceLastSeen());
    else
        Serial.printf("\u001b[35m%u C# -1 | MAC: %s, ID: %-58s%ddBm (%.2fm) %lums\u001b[0m\n", xPortGetCoreID(), f->getMac().c_str(), f->getId().c_str(), f->getRssi(), f->getDistance(), f->getMsSinceLastSeen());
}

void ConnectProgress() {
    LEDs::ConnectProgress();
}

void Status(const char *format, ...) {
    char *message;
    va_list args;
    va_start(args, format);
    vasprintf(&message, format, args);
    va_end(args);
    Serial.printf("%u Status| %s", xPortGetCoreID(), message);
    Display::Status(message);
    free(message);
}

bool Command(String &command, String &pay) {
    return LEDs::Command(command, pay);
}

bool SendDiscovery() {
    return LEDs::SendDiscovery();
}
}  // namespace GUI
