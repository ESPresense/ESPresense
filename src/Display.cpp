#include "Display.h"

#ifdef M5STICK
#include <TFT_eSPI.h>
#include <freertos/portmacro.h>
#include <lvgl.h>
#include <cstdio>
#include <cstring>
#endif

namespace {
#ifdef M5STICK
constexpr uint32_t LVGL_LOOP_MS = 33;
constexpr size_t MAX_EVENT_LEN = 48;
constexpr size_t MAX_STATUS_LEN = 72;
constexpr size_t MAX_PAYLOAD_LEN = 96;
constexpr uint32_t DRAW_BUFFER_LINES = 20;
constexpr uint32_t DRAW_BUFFER_WIDTH = 240;

struct DashboardState {
    bool wifiKnown = false;
    bool wifiConnected = false;
    bool mqttKnown = false;
    bool mqttConnected = false;
    uint32_t closeCount = 0;
    uint32_t leftCount = 0;
    char lastEvent[MAX_EVENT_LEN] = "Waiting for events";
    char statusLine[MAX_STATUS_LEN] = "Starting...";
    bool dirty = true;
};

DashboardState gState;
portMUX_TYPE gStateMux = portMUX_INITIALIZER_UNLOCKED;

bool lvglReady = false;
uint32_t lastTickMs = 0;
uint32_t lastHandlerMs = 0;
lv_disp_t *display = nullptr;

TFT_eSPI tft;
lv_disp_draw_buf_t drawBufferHandle;
lv_disp_drv_t displayDriver;
lv_color_t drawBuffer[DRAW_BUFFER_WIDTH * DRAW_BUFFER_LINES];

lv_obj_t *wifiLabel = nullptr;
lv_obj_t *mqttLabel = nullptr;
lv_obj_t *eventLabel = nullptr;
lv_obj_t *countLabel = nullptr;
lv_obj_t *statusLabel = nullptr;

void copyTrimmed(const char *src, char *dst, size_t dstSize) {
    if (dstSize == 0) return;

    size_t index = 0;
    while (src[index] != '\0' && index < dstSize - 1) {
        if (src[index] == '\r' || src[index] == '\n') break;
        dst[index] = src[index];
        index++;
    }
    dst[index] = '\0';
}

void lvglFlush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *colorP) {
    const uint32_t width = static_cast<uint32_t>(area->x2 - area->x1 + 1);
    const uint32_t height = static_cast<uint32_t>(area->y2 - area->y1 + 1);

    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, width, height);
    tft.pushColors(reinterpret_cast<uint16_t *>(colorP), width * height, true);
    tft.endWrite();

    lv_disp_flush_ready(disp);
}

void parseStatusMessage(const char *message) {
    char payload[MAX_PAYLOAD_LEN];
    copyTrimmed(message, payload, sizeof(payload));
    if (payload[0] == '\0') return;

    portENTER_CRITICAL(&gStateMux);
    if (strncmp(payload, "Wifi:", 5) == 0) {
        gState.wifiKnown = true;
        gState.wifiConnected = strstr(payload, "Wifi:yes") != nullptr;
        gState.mqttKnown = true;
        gState.mqttConnected = strstr(payload, "Mqtt:yes") != nullptr;
        strncpy(gState.statusLine, payload, sizeof(gState.statusLine) - 1);
        gState.statusLine[sizeof(gState.statusLine) - 1] = '\0';
    } else if (strncmp(payload, "C:", 2) == 0) {
        gState.closeCount++;
        snprintf(gState.lastEvent, sizeof(gState.lastEvent), "Close %s", payload + 2);
    } else if (strncmp(payload, "L:", 2) == 0) {
        gState.leftCount++;
        snprintf(gState.lastEvent, sizeof(gState.lastEvent), "Left %s", payload + 2);
    } else if (strncmp(payload, "Update:", 7) == 0) {
        snprintf(gState.statusLine, sizeof(gState.statusLine), "FW %s", payload + 7);
    } else {
        strncpy(gState.statusLine, payload, sizeof(gState.statusLine) - 1);
        gState.statusLine[sizeof(gState.statusLine) - 1] = '\0';
    }
    gState.dirty = true;
    portEXIT_CRITICAL(&gStateMux);
}

void createDashboard() {
    lv_obj_t *screen = lv_scr_act();
    lv_obj_set_style_bg_color(screen, lv_color_hex(0x090B10), 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_all(screen, 0, 0);

    lv_obj_t *title = lv_label_create(screen);
    lv_label_set_text(title, "ESPresense");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(title, lv_color_hex(0xF2F4F8), 0);
    lv_obj_align(title, LV_ALIGN_TOP_LEFT, 6, 4);

    wifiLabel = lv_label_create(screen);
    lv_obj_set_style_text_font(wifiLabel, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(wifiLabel, lv_color_hex(0xA9BCD0), 0);
    lv_obj_align(wifiLabel, LV_ALIGN_TOP_LEFT, 6, 24);

    mqttLabel = lv_label_create(screen);
    lv_obj_set_style_text_font(mqttLabel, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(mqttLabel, lv_color_hex(0xA9BCD0), 0);
    lv_obj_align(mqttLabel, LV_ALIGN_TOP_LEFT, 88, 24);

    eventLabel = lv_label_create(screen);
    lv_obj_set_width(eventLabel, tft.width() - 12);
    lv_label_set_long_mode(eventLabel, LV_LABEL_LONG_DOT);
    lv_obj_set_style_text_font(eventLabel, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(eventLabel, lv_color_hex(0xE6EDF7), 0);
    lv_obj_align(eventLabel, LV_ALIGN_TOP_LEFT, 6, 44);

    countLabel = lv_label_create(screen);
    lv_obj_set_style_text_font(countLabel, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(countLabel, lv_color_hex(0x74D3AE), 0);
    lv_obj_align(countLabel, LV_ALIGN_BOTTOM_LEFT, 6, -16);

    statusLabel = lv_label_create(screen);
    lv_obj_set_width(statusLabel, tft.width() - 12);
    lv_label_set_long_mode(statusLabel, LV_LABEL_LONG_DOT);
    lv_obj_set_style_text_font(statusLabel, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(statusLabel, lv_color_hex(0x99A6B5), 0);
    lv_obj_align(statusLabel, LV_ALIGN_BOTTOM_LEFT, 6, -3);
}

void updateDashboard() {
    DashboardState snapshot;
    portENTER_CRITICAL(&gStateMux);
    snapshot = gState;
    gState.dirty = false;
    portEXIT_CRITICAL(&gStateMux);

    if (!wifiLabel || !mqttLabel || !eventLabel || !countLabel || !statusLabel) return;

    lv_label_set_text_fmt(wifiLabel, "WiFi: %s", snapshot.wifiKnown ? (snapshot.wifiConnected ? "OK" : "OFF") : "--");
    lv_label_set_text_fmt(mqttLabel, "MQTT: %s", snapshot.mqttKnown ? (snapshot.mqttConnected ? "OK" : "OFF") : "--");
    lv_label_set_text_fmt(eventLabel, "Last: %s", snapshot.lastEvent);
    lv_label_set_text_fmt(countLabel, "Close %lu   Left %lu", static_cast<unsigned long>(snapshot.closeCount), static_cast<unsigned long>(snapshot.leftCount));
    lv_label_set_text(statusLabel, snapshot.statusLine);
}
#endif
}  // namespace

void Display::Setup() {
#ifdef M5STICK
    tft.begin();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);

    lv_init();

    const uint16_t width = tft.width();
    lv_disp_draw_buf_init(&drawBufferHandle, drawBuffer, nullptr, width * DRAW_BUFFER_LINES);

    lv_disp_drv_init(&displayDriver);
    displayDriver.hor_res = width;
    displayDriver.ver_res = tft.height();
    displayDriver.flush_cb = lvglFlush;
    displayDriver.draw_buf = &drawBufferHandle;
    display = lv_disp_drv_register(&displayDriver);

    createDashboard();
    updateDashboard();
    lv_obj_invalidate(lv_scr_act());
    lv_refr_now(display);

    lastTickMs = millis();
    lastHandlerMs = lastTickMs;
    lvglReady = true;
#endif
}

void Display::ConnectToWifi() {}

void Display::Loop() {
#ifdef M5STICK
    if (!lvglReady) return;

    const uint32_t now = millis();
    const uint32_t delta = now - lastTickMs;
    if (delta > 0) {
        lv_tick_inc(delta);
        lastTickMs = now;
    }

    if (now - lastHandlerMs >= LVGL_LOOP_MS) {
        lv_timer_handler();
        lastHandlerMs = now;
    }

    bool dirty = false;
    portENTER_CRITICAL(&gStateMux);
    dirty = gState.dirty;
    portEXIT_CRITICAL(&gStateMux);
    if (dirty) updateDashboard();
#endif
}

void Display::Status(const char *message) {
#ifdef M5STICK
    parseStatusMessage(message);
#endif
}
