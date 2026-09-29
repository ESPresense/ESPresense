#ifdef COEXIST_TEST
#include "CoexistTest.h"

#include <atomic>
#include <cstdint>
#include <cstdlib>
#include <cstring>

#include "Logger.h"
#include "Network.h"
#include "esp_netif.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "lwip/inet.h"
#include "lwip/ip_addr.h"
#include "mbedtls/base64.h"
#include "ping/ping_sock.h"

// board_id per the ratified ECF1 enum (0=esp32, 1=esp32-s3, 2=esp32-c3, 3=esp32-c6) -
// reuses the exact chip-target macros envs.cmake already defines for production, so
// there is no new plumbing for board identity.
#if defined(ESP32S3)
#define COEXIST_BOARD_ID 1
#elif defined(ESP32C3)
#define COEXIST_BOARD_ID 2
#elif defined(ESP32C6)
#define COEXIST_BOARD_ID 3
#else
#define COEXIST_BOARD_ID 0
#endif

#ifndef COEXIST_DWELL_SECS
#define COEXIST_DWELL_SECS 20
#endif

namespace CoexistTest {
namespace {

// ESPA-218: -1 is a true CSI-fully-disabled control bucket, added alongside the
// original ESPA-196 sweep. R=0 was already CSI-enabled-but-unpinged (no self-ping
// traffic, since the ping session is only started for rateHz>0) - it was never a
// clean "CSI off" baseline, so it couldn't tell whether the 18-52% BLE duty loss
// measured at R=10 in ESPA-196 comes from the self-ping traffic itself or from CSI
// extraction/callback overhead. Comparing -1 vs 0 isolates CSI-extraction-alone
// cost; comparing 0 vs 10+ isolates self-ping-traffic cost. Same-run A/B/C, so it
// doesn't need a precision controlled BLE reference (ESPA-217) - ambient BLE
// conditions are shared across all three buckets within one dwell cycle.
constexpr int kRatesHz[] = {-1, 0, 10, 20, 30, 50, 100};
constexpr int kDwellSecs = COEXIST_DWELL_SECS;

// This bench only has one physical unit per chip family, and (per DT, ESPA-196
// interaction 051a1d4b) the classic /dev/esp32 unit is a normal dual-core board -
// it will NOT exercise the single-core cliff risk the June coexistence-model doc
// flagged as the open question. ESP32-C3 and ESP32-C6 are always single-core
// RISC-V parts (Espressif spec, not board-specific), and both are already wired
// on this same bench - they are the real stand-in for that case, not a
// substitute. `kCoexistCore` mirrors this repo's own existing NimBLE core-pin
// choice per chip (sdkconfig.defaults.<target>: NimBLE on core 1 on esp32/s3,
// core 0 - the only core - on c3/c6) by pinning this task to the *other* core
// on dual-core targets, and to the only core on single-core targets. That is
// the exact contention geometry the modeled knee was reasoning about.
#if CONFIG_FREERTOS_UNICORE
constexpr BaseType_t kCoexistCore = 0;
#else
constexpr BaseType_t kCoexistCore = (CONFIG_BT_NIMBLE_PINNED_TO_CORE == 0) ? 1 : 0;
#endif

constexpr size_t kMaxCsiLen = 600;   // generous upper bound on esp-csi record length (HT40 incl.)
constexpr size_t kHeaderLen = 24;    // magic..csi_len per the ratified §B v1 layout
constexpr size_t kMaxFrameLen = kHeaderLen + kMaxCsiLen + 1;  // + crc8
constexpr size_t kB64BufLen = 4 * ((kMaxFrameLen + 2) / 3) + 4;

std::atomic<uint32_t> g_bleAdvertsSeen{0};
std::atomic<uint32_t> g_pingSent{0};
std::atomic<uint32_t> g_pingRecv{0};
std::atomic<uint32_t> g_csiCaptured{0};
std::atomic<uint32_t> g_csiDropped{0};
std::atomic<uint32_t> g_seq{0};
std::atomic<int> g_currentRateHz{0};
std::atomic<bool> g_sweepRunning{false};

Ble::AdvertCallback g_realAdvertCb = nullptr;

void advertHook(const Ble::Advert &a) {
    g_bleAdvertsSeen.fetch_add(1, std::memory_order_relaxed);
    if (g_realAdvertCb) g_realAdvertCb(a);
}

void onPingSuccess(esp_ping_handle_t, void *) {
    g_pingSent.fetch_add(1, std::memory_order_relaxed);
    g_pingRecv.fetch_add(1, std::memory_order_relaxed);
}

void onPingTimeout(esp_ping_handle_t, void *) {
    g_pingSent.fetch_add(1, std::memory_order_relaxed);
}

// CRC-8/SMBUS: poly 0x07, init 0x00, no reflect-in/out, xorout 0x00 - ratified as the
// §B crc8 definition 2026-06-09 (matches QA's ecf1_adapter.py assumption).
uint8_t crc8Smbus(const uint8_t *data, size_t len) {
    uint8_t crc = 0x00;
    for (size_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (int b = 0; b < 8; b++) {
            crc = (crc & 0x80) ? static_cast<uint8_t>((crc << 1) ^ 0x07) : static_cast<uint8_t>(crc << 1);
        }
    }
    return crc;
}

struct QueuedFrame {
    uint8_t *bytes;
    size_t len;
};

QueueHandle_t g_csiQueue = nullptr;

// Runs in WiFi driver task context - must stay fast and non-blocking. Building the
// frame is a fixed-size memcpy job; the slow part (base64 + serial write) is pushed
// to csiEmitterTask over a bounded queue so a busy UART can never back-pressure the
// WiFi task. A dropped frame here just increments csi_dropped, same spirit as the
// original design's csi_drop counter.
void onCsi(void *, wifi_csi_info_t *info) {
    if (!g_sweepRunning.load(std::memory_order_relaxed)) return;
    g_csiCaptured.fetch_add(1, std::memory_order_relaxed);

    uint16_t csiLen = info->len;
    if (csiLen > kMaxCsiLen) csiLen = kMaxCsiLen;  // clamp; still emits a valid (truncated) frame

    auto *frame = static_cast<uint8_t *>(malloc(kHeaderLen + csiLen + 1));
    if (!frame) {
        g_csiDropped.fetch_add(1, std::memory_order_relaxed);
        return;
    }

    size_t off = 0;
    frame[off++] = 0xC5;  // magic
    frame[off++] = 1;     // ver
    frame[off++] = COEXIST_BOARD_ID;
    frame[off++] = static_cast<uint8_t>(g_currentRateHz.load(std::memory_order_relaxed));
    uint32_t seq = g_seq.fetch_add(1, std::memory_order_relaxed);
    memcpy(&frame[off], &seq, 4);
    off += 4;
    int64_t tsUs = esp_timer_get_time();
    memcpy(&frame[off], &tsUs, 8);
    off += 8;
    frame[off++] = static_cast<uint8_t>(info->rx_ctrl.rssi);
    frame[off++] = static_cast<uint8_t>(info->rx_ctrl.channel);
// esp32c6 is WiFi 6 (HE) capable (SOC_WIFI_HE_SUPPORT=1), which routes rx_ctrl through
// the HE-variant esp_wifi_rxctrl_t (esp_wifi_he_types.h) instead of the plain
// wifi_pkt_rx_ctrl_t the other three boards use - and that struct does not expose
// .cwb/.sig_mode at all (confirmed against the actual v5.4.4 header, not assumed).
// 0xFF sentinel instead of a fabricated 0/1 so this is honestly "not available" rather
// than silently wrong.
#if CONFIG_SOC_WIFI_HE_SUPPORT
    frame[off++] = 0xFF;  // bw: not exposed on HE targets
    frame[off++] = 0xFF;  // sig_mode: not exposed on HE targets
#else
    frame[off++] = static_cast<uint8_t>(info->rx_ctrl.cwb);       // 0=HT20, 1=HT40
    frame[off++] = static_cast<uint8_t>(info->rx_ctrl.sig_mode);  // 0=non-HT, 1=HT
#endif
    uint16_t nSub = csiLen / 2;
    memcpy(&frame[off], &nSub, 2);
    off += 2;
    memcpy(&frame[off], &csiLen, 2);
    off += 2;
    memcpy(&frame[off], info->buf, csiLen);
    off += csiLen;
    frame[off] = crc8Smbus(frame, off);
    off += 1;

    QueuedFrame qf{frame, off};
    if (xQueueSend(g_csiQueue, &qf, 0) != pdTRUE) {
        free(frame);
        g_csiDropped.fetch_add(1, std::memory_order_relaxed);
    }
}

void csiEmitterTask(void *) {
    unsigned char b64[kB64BufLen];
    for (;;) {
        QueuedFrame qf;
        if (xQueueReceive(g_csiQueue, &qf, portMAX_DELAY) != pdTRUE) continue;
        size_t b64Len = 0;
        if (mbedtls_base64_encode(b64, sizeof(b64), &b64Len, qf.bytes, qf.len) == 0) {
            b64[b64Len] = '\0';
            Log.printf("COEXIST_B %s\r\n", reinterpret_cast<const char *>(b64));
        }
        free(qf.bytes);
    }
}

// Resolves the STA gateway to self-ping. Uses the standard ESP-IDF ping-example
// idiom (inet_addr_to_ip4addr over ip_2_ip4) rather than touching ip_addr_t's
// union layout directly, since that layout varies with LWIP_IPV6 and this repo
// builds with CONFIG_LWIP_IPV6=n.
bool resolveGatewayTarget(ip_addr_t *out) {
    esp_netif_t *staNetif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
    if (!staNetif) return false;
    esp_netif_ip_info_t ipInfo;
    if (esp_netif_get_ip_info(staNetif, &ipInfo) != ESP_OK) return false;
    if (ipInfo.gw.addr == 0) return false;
    struct in_addr addr4;
    addr4.s_addr = ipInfo.gw.addr;
    memset(out, 0, sizeof(*out));
    inet_addr_to_ip4addr(ip_2_ip4(out), &addr4);
    // IP_SET_TYPE_VAL, not `out->type = ...` directly: this repo builds with
    // CONFIG_LWIP_IPV6=n, so LWIP_IPV6=0 and ip_addr_t collapses to a bare
    // ip4_addr_t with no .type member at all (confirmed against the actual
    // esp-lwip source, not assumed) - IP_SET_TYPE_VAL is a no-op in that
    // configuration and the real union-tagging write in a dual-stack build,
    // so this line is correct either way instead of only in one of them.
    IP_SET_TYPE_VAL(*out, IPADDR_TYPE_V4);
    return true;
}

void sweepTask(void *) {
    while (!Network::isOnline()) vTaskDelay(pdMS_TO_TICKS(500));

    wifi_csi_config_t csiCfg = {};
// esp32c6 is WiFi 6 (HE) capable, so wifi_csi_config_t is actually the bitfield
// wifi_csi_acquire_config_t from esp_wifi_he_types.h - a completely different field
// set from the other three boards' plain struct (confirmed against the real v5.4.4
// header). Only touching the field names present in BOTH HE sub-variants
// (MAC_VERSION_NUM==3 vs not) to stay portable across esp32c6 and any future HE part.
#if CONFIG_SOC_WIFI_HE_SUPPORT
    csiCfg.enable = 1;
    csiCfg.acquire_csi_legacy = 1;
    csiCfg.acquire_csi_ht20 = 1;
    csiCfg.acquire_csi_ht40 = 1;
#else
    csiCfg.lltf_en = true;
    csiCfg.htltf_en = true;
    csiCfg.stbc_htltf2_en = true;
    csiCfg.ltf_merge_en = true;
    csiCfg.channel_filter_en = true;
    csiCfg.manu_scale = false;
#endif
    esp_wifi_set_csi_rx_cb(&onCsi, nullptr);
    esp_wifi_set_csi_config(&csiCfg);
    // Enable/disable is now toggled per-bucket inside the loop below (ESPA-218's
    // rate=-1 control bucket needs it off; every other bucket needs it on).

    // Loop the whole sweep for as long as the HIL run keeps power on: a short PR
    // smoke test gets one (partial) pass, a soak run gets dozens - more repeated
    // samples per rate, not a different code path.
    for (;;) {
        for (int rateHz : kRatesHz) {
            g_currentRateHz.store(rateHz, std::memory_order_relaxed);
            g_bleAdvertsSeen.store(0, std::memory_order_relaxed);
            g_csiCaptured.store(0, std::memory_order_relaxed);
            g_csiDropped.store(0, std::memory_order_relaxed);
            g_pingSent.store(0, std::memory_order_relaxed);
            g_pingRecv.store(0, std::memory_order_relaxed);
            g_sweepRunning.store(true, std::memory_order_relaxed);

            // rate=-1 is the CSI-off control bucket (see kRatesHz comment) - every
            // other bucket (including rate=0) keeps CSI capture enabled exactly as
            // ESPA-196 originally had it.
            esp_wifi_set_csi(rateHz != -1);

            esp_ping_handle_t pingHdl = nullptr;
            ip_addr_t target;
            if (rateHz > 0 && resolveGatewayTarget(&target)) {
                esp_ping_config_t cfg = ESP_PING_DEFAULT_CONFIG();
                cfg.target_addr = target;
                cfg.count = ESP_PING_COUNT_INFINITE;
                cfg.interval_ms = 1000 / static_cast<uint32_t>(rateHz);
                cfg.timeout_ms = cfg.interval_ms < 500 ? cfg.interval_ms : 500;
                esp_ping_callbacks_t cbs = {};
                cbs.on_ping_success = onPingSuccess;
                cbs.on_ping_timeout = onPingTimeout;
                if (esp_ping_new_session(&cfg, &cbs, &pingHdl) == ESP_OK) {
                    esp_ping_start(pingHdl);
                } else {
                    pingHdl = nullptr;
                }
            }

            vTaskDelay(pdMS_TO_TICKS(kDwellSecs * 1000));

            g_sweepRunning.store(false, std::memory_order_relaxed);
            if (pingHdl) {
                esp_ping_stop(pingHdl);
                esp_ping_delete_session(pingHdl);
            }

            Log.printf(
                "COEXIST_A board=%d rate=%d dur_s=%d ble_adverts_seen=%lu ping_sent=%lu "
                "ping_recv=%lu csi_captured=%lu csi_dropped=%lu free_heap=%lu "
                "min_free_heap=%lu uptime_ms=%lu\r\n",
                COEXIST_BOARD_ID, rateHz, kDwellSecs,
                static_cast<unsigned long>(g_bleAdvertsSeen.load()),
                static_cast<unsigned long>(g_pingSent.load()),
                static_cast<unsigned long>(g_pingRecv.load()),
                static_cast<unsigned long>(g_csiCaptured.load()),
                static_cast<unsigned long>(g_csiDropped.load()),
                static_cast<unsigned long>(esp_get_free_heap_size()),
                static_cast<unsigned long>(esp_get_minimum_free_heap_size()),
                static_cast<unsigned long>(esp_timer_get_time() / 1000));
        }
    }
}

}  // namespace

Ble::AdvertCallback WrapAdvertCallback(Ble::AdvertCallback real) {
    g_realAdvertCb = real;
    return &advertHook;
}

void Begin() {
    // ESPA-196: esp32c3's smoke test showed free_heap at ~8.9KB right after boot
    // (vs ~43KB on esp32) and an OOM reboot ~94s in - c3 is already RAM-tight in
    // production (see sdkconfig.defaults.esp32c3's own "200 fingerprints on C3"
    // comment), and CSI enable reserves its own buffers on top of that. 3072
    // matches this repo's own proven-safe size for scanTask (SCAN_TASK_STACK_SIZE,
    // defaults.h) - a real precedent, not a guess - down from this file's original
    // 4096. Queue depth 4 (was 8) halves the worst-case in-flight buffer backlog
    // under backpressure. Neither is a fix for CSI's own buffer cost, only for the
    // fixed overhead this test harness adds on top of it.
    g_csiQueue = xQueueCreate(4, sizeof(QueuedFrame));
    xTaskCreatePinnedToCore(csiEmitterTask, "coexistEmit", 3072, nullptr, 1, nullptr, kCoexistCore);
    xTaskCreatePinnedToCore(sweepTask, "coexistSweep", 3072, nullptr, 1, nullptr, kCoexistCore);
}

}  // namespace CoexistTest
#endif  // COEXIST_TEST
