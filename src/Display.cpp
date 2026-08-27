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
