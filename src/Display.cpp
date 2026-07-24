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

#if defined(M5ATOM) && defined(MATRIX)
#include <Adafruit_NeoPixel.h>

namespace {
constexpr uint8_t kAtomMatrixPin = 27;
constexpr uint8_t kAtomMatrixWidth = 5;
constexpr uint8_t kAtomMatrixHeight = 5;
constexpr uint8_t kAtomMatrixCount = kAtomMatrixWidth * kAtomMatrixHeight;

Adafruit_NeoPixel matrix(kAtomMatrixCount, kAtomMatrixPin, NEO_GRB + NEO_KHZ800);
bool matrixInitialized = false;

uint8_t xyToIndex(uint8_t x, uint8_t y) {
    // M5 Atom Matrix is wired in a serpentine pattern.
    if (y % 2 == 0) {
        return y * kAtomMatrixWidth + x;
    }
    return y * kAtomMatrixWidth + (kAtomMatrixWidth - 1 - x);
}

void clearMatrix() {
    for (uint8_t i = 0; i < kAtomMatrixCount; i++) {
        matrix.setPixelColor(i, 0);
    }
}

// 5x5 glyphs, each row encoded in lower 5 bits.
const uint8_t* glyphFor(char c) {
    static const uint8_t GLYPH_SPACE[5] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000};
    static const uint8_t GLYPH_DASH[5] = {0b00000, 0b00000, 0b11111, 0b00000, 0b00000};
    static const uint8_t GLYPH_DOT[5] = {0b00000, 0b00000, 0b00000, 0b00110, 0b00110};

    static const uint8_t GLYPH_0[5] = {0b01110, 0b10011, 0b10101, 0b11001, 0b01110};
    static const uint8_t GLYPH_1[5] = {0b00100, 0b01100, 0b00100, 0b00100, 0b01110};
    static const uint8_t GLYPH_2[5] = {0b01110, 0b00001, 0b01110, 0b10000, 0b11111};
    static const uint8_t GLYPH_3[5] = {0b11110, 0b00001, 0b00110, 0b00001, 0b11110};
    static const uint8_t GLYPH_4[5] = {0b10010, 0b10010, 0b11111, 0b00010, 0b00010};
    static const uint8_t GLYPH_5[5] = {0b11111, 0b10000, 0b11110, 0b00001, 0b11110};
    static const uint8_t GLYPH_6[5] = {0b01110, 0b10000, 0b11110, 0b10001, 0b01110};
    static const uint8_t GLYPH_7[5] = {0b11111, 0b00001, 0b00010, 0b00100, 0b00100};
    static const uint8_t GLYPH_8[5] = {0b01110, 0b10001, 0b01110, 0b10001, 0b01110};
    static const uint8_t GLYPH_9[5] = {0b01110, 0b10001, 0b01111, 0b00001, 0b01110};

    static const uint8_t GLYPH_A[5] = {0b01110, 0b10001, 0b11111, 0b10001, 0b10001};
    static const uint8_t GLYPH_C[5] = {0b01110, 0b10001, 0b10000, 0b10001, 0b01110};
    static const uint8_t GLYPH_D[5] = {0b11110, 0b10001, 0b10001, 0b10001, 0b11110};
    static const uint8_t GLYPH_E[5] = {0b11111, 0b10000, 0b11110, 0b10000, 0b11111};
    static const uint8_t GLYPH_F[5] = {0b11111, 0b10000, 0b11110, 0b10000, 0b10000};
    static const uint8_t GLYPH_H[5] = {0b10001, 0b10001, 0b11111, 0b10001, 0b10001};
    static const uint8_t GLYPH_I[5] = {0b01110, 0b00100, 0b00100, 0b00100, 0b01110};
    static const uint8_t GLYPH_L[5] = {0b10000, 0b10000, 0b10000, 0b10000, 0b11111};
    static const uint8_t GLYPH_M[5] = {0b10001, 0b11011, 0b10101, 0b10001, 0b10001};
    static const uint8_t GLYPH_N[5] = {0b10001, 0b11001, 0b10101, 0b10011, 0b10001};
    static const uint8_t GLYPH_O[5] = {0b01110, 0b10001, 0b10001, 0b10001, 0b01110};
    static const uint8_t GLYPH_P[5] = {0b11110, 0b10001, 0b11110, 0b10000, 0b10000};
    static const uint8_t GLYPH_R[5] = {0b11110, 0b10001, 0b11110, 0b10100, 0b10010};
    static const uint8_t GLYPH_S[5] = {0b01111, 0b10000, 0b01110, 0b00001, 0b11110};
    static const uint8_t GLYPH_T[5] = {0b11111, 0b00100, 0b00100, 0b00100, 0b00100};
    static const uint8_t GLYPH_U[5] = {0b10001, 0b10001, 0b10001, 0b10001, 0b01110};
    static const uint8_t GLYPH_W[5] = {0b10001, 0b10001, 0b10101, 0b11011, 0b10001};

    switch (c) {
        case ' ': return GLYPH_SPACE;
        case '-': return GLYPH_DASH;
        case '.': return GLYPH_DOT;
        case '0': return GLYPH_0;
        case '1': return GLYPH_1;
        case '2': return GLYPH_2;
        case '3': return GLYPH_3;
        case '4': return GLYPH_4;
        case '5': return GLYPH_5;
        case '6': return GLYPH_6;
        case '7': return GLYPH_7;
        case '8': return GLYPH_8;
        case '9': return GLYPH_9;
        case 'A': return GLYPH_A;
        case 'C': return GLYPH_C;
        case 'D': return GLYPH_D;
        case 'E': return GLYPH_E;
        case 'F': return GLYPH_F;
        case 'H': return GLYPH_H;
        case 'I': return GLYPH_I;
        case 'L': return GLYPH_L;
        case 'M': return GLYPH_M;
        case 'N': return GLYPH_N;
        case 'O': return GLYPH_O;
        case 'P': return GLYPH_P;
        case 'R': return GLYPH_R;
        case 'S': return GLYPH_S;
        case 'T': return GLYPH_T;
        case 'U': return GLYPH_U;
        case 'W': return GLYPH_W;
        default: return GLYPH_SPACE;
    }
}

void renderGlyph(const uint8_t* glyph, uint32_t color = 0x001000) {
    clearMatrix();
    for (uint8_t y = 0; y < kAtomMatrixHeight; y++) {
        const uint8_t row = glyph[y];
        for (uint8_t x = 0; x < kAtomMatrixWidth; x++) {
            if (row & (1 << (kAtomMatrixWidth - 1 - x))) {
                matrix.setPixelColor(xyToIndex(x, y), color);
            }
        }
    }
    matrix.show();
}
}
#endif

void Display::Setup() {
#ifdef M5STICK
    tb_display_init();
#endif
#if defined(M5ATOM) && defined(MATRIX)
    matrix.begin();
    matrix.setBrightness(20);
    clearMatrix();
    matrix.show();
    matrixInitialized = true;
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
#if defined(M5ATOM) && defined(MATRIX)
    if (matrixInitialized && message != nullptr && message[0] != '\0') {
        char c = message[0];
        if (c >= 'a' && c <= 'z') {
            c -= ('a' - 'A');
        }
        renderGlyph(glyphFor(c));
    }
#endif
    free(message);
}
