#include "Display.h"

#ifdef M5STICK
#ifdef PLUS
#include <M5StickCPlus.h>
#else
#include <M5StickC.h>
#endif
#include <TFT_eSPI.h>
#include "tb_display.h"
#endif

void Display::Setup() {
#ifdef M5STICK
    tb_display_init();
#endif
}

void Display::ConnectToWifi() {
}

void Display::Loop() {
}

void Display::Erasing() {
    Status("Erasing...");
}

void Display::Erased() {
}

void Display::Connecting() {
}

void Display::Connected(bool wifi = false, bool mqtt = false) {
    Status("Wifi:%s Mqtt:%s\n", (wifi ? "yes" : "no"), (mqtt ? "yes" : "no"));
}

void Display::Added(BleFingerprint *f) {
}

void Display::Removed(BleFingerprint *f) {
}

void Display::Close(BleFingerprint *f) {
    Status("C:%s", f->getId().c_str());
}

void Display::Left(BleFingerprint *f) {
    Status("L:%s", f->getId().c_str());
}

void Display::Status(const char *format, ...) {
    char *message;
    va_list args;
    va_start(args, format);
    vasprintf(&message, format, args);
    va_end(args);
#ifdef M5STICK
    tb_display_print_String(message);
#endif
    free(message);
}

void Display::UpdateStart() {
    Status("Update:%s", "started");
}

void Display::UpdateProgress(unsigned int percent) {
}

void Display::UpdateEnd() {
    Status("Update:%s", "finished");
}
