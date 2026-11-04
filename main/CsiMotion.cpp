#ifdef SENSORS
#include "CsiMotion.h"

#include <atomic>
#include <cstring>
#include <memory>
#include <new>

#include "CsiWindow.h"
#include "Settings.h"
#include "esp_wifi.h"
#include "globals.h"
#include "mqtt.h"

namespace CsiMotion
{
    // Short enough to line up with BLE room changes, which happen every few seconds; at the
    // 7-23 frames/s seen on house networks a window still holds 35-115 frames.
    static const unsigned long kWindowMs = 5000;
    // After arming, readings run two orders of magnitude high while the receiver settles.
    static const unsigned long kSettleMs = 30000;
    // Below this the channel isn't sampled often enough for a window to mean anything.
    static const uint32_t kMinFrames = 10;
    // Frames queued between the WiFi task and the loop. Frames arrive in bursts (group frames after
    // each DTIM beacon, MQTT traffic in bursts too); a single slot dropped about half of them on the
    // HIL rack and 8 slots still dropped a third on a busy esp32c3 or esp32s3.
    static const size_t kQueue = 16;
    // Frame kinds tracked per window (see kindOf). More than this in one window are dropped.
    static const size_t kKinds = 4;

    struct Slot {
        int8_t csi[CsiWindow::kMaxBytes];
        size_t len;
        uint16_t kind;
    };

    struct Kind {
        uint16_t key = 0;
        CsiWindow window;
    };

    static bool enabled = false;
    static bool armed = false;
    static unsigned long armedAt = 0;
    static unsigned long windowStart = 0;
    static float fps = 0;
    static std::unique_ptr<Kind[]> kinds;
    static size_t kindCount = 0;
    static std::unique_ptr<Slot[]> queue;
    static std::atomic<uint32_t> head{0}, tail{0};
    static std::atomic<uint32_t> dropped{0};
    static std::atomic<uint64_t> apMac{0};

    static uint64_t packMac(const uint8_t* mac)
    {
        uint64_t packed = 0;
        memcpy(&packed, mac, 6);
        return packed;
    }

    // Frames are only compared with frames of the same kind: group or unicast, and the rate and
    // PHY mode the AP sent them at. Mixing kinds reads as movement (rate adaptation and
    // beamforming reshape the channel estimate), which is why all-frames-together was noisy.
    static uint16_t kindOf(const wifi_csi_info_t* info)
    {
        uint16_t key = (info->dmac[0] & 1) ? 0x8000 : 0;
#if !CONFIG_SOC_WIFI_HE_SUPPORT
        key |= (uint16_t)(info->rx_ctrl.sig_mode & 3) << 8;
        // rate is only meaningful for legacy frames; HT frames carry their MCS and width instead.
        if (info->rx_ctrl.sig_mode) return key | (info->rx_ctrl.mcs & 0x7f) | (info->rx_ctrl.cwb ? 0x80 : 0);
#endif
        key |= info->rx_ctrl.rate & 0xff;
        return key;
    }

    // WiFi task context, for every frame: a fixed-size copy and nothing else. Loop does the maths.
    //
    // Every frame from the AP counts, group-addressed or unicast to this node. Some APs send no
    // group frames the CSI path sees (seen 2026-11-02 on a Ubiquiti AP), while the node's own
    // MQTT traffic arrives on every network, at 10-20 frames/s.
    static void onCsi(void*, wifi_csi_info_t* info)
    {
        if (!info || !info->buf) return;
        if (packMac(info->mac) != apMac.load(std::memory_order_relaxed)) return;
        const uint32_t h = head.load(std::memory_order_relaxed);
        if (h - tail.load(std::memory_order_acquire) >= kQueue) { dropped++; return; }
        Slot& s = queue[h % kQueue];
        const size_t len = info->len < sizeof(s.csi) ? info->len : sizeof(s.csi);
        memcpy(s.csi, info->buf, len);
        s.len = len;
        s.kind = kindOf(info);
        head.store(h + 1, std::memory_order_release);
    }

    // L-LTF only: every frame then has the same layout, whatever rate the AP sent it at.
    static bool arm()
    {
        wifi_csi_config_t cfg = {};
#if CONFIG_SOC_WIFI_HE_SUPPORT
        cfg.enable = 1;
        cfg.acquire_csi_legacy = 1;
#else
        cfg.lltf_en = true;
        cfg.channel_filter_en = true;
#endif
        return esp_wifi_set_csi_rx_cb(&onCsi, nullptr) == ESP_OK
            && esp_wifi_set_csi_config(&cfg) == ESP_OK
            && esp_wifi_set_csi(true) == ESP_OK;
    }

    static void resetKinds()
    {
        for (size_t i = 0; i < kindCount; i++) kinds[i].window.reset();
        kindCount = 0;
    }

    // Follows the AP the node is on. Ethernet nodes have none, so they never start.
    static void trackAp()
    {
        wifi_ap_record_t ap;
        const uint64_t mac = esp_wifi_sta_get_ap_info(&ap) == ESP_OK ? packMac(ap.bssid) : 0;
        if (mac == apMac.load(std::memory_order_relaxed)) {
            if (mac && !armed && (armed = arm())) armedAt = millis();
            return;
        }
        apMac.store(mac, std::memory_order_relaxed);
        resetKinds();  // a different AP is a different channel, not movement
        armed = mac && arm();
        armedAt = millis();
    }

    static void add(const Slot& s)
    {
        for (size_t i = 0; i < kindCount; i++)
            if (kinds[i].key == s.kind) return kinds[i].window.add(s.csi, s.len);
        if (kindCount == kKinds) return;
        kinds[kindCount].key = s.kind;
        kinds[kindCount++].window.add(s.csi, s.len);
    }

    void ConnectToWifi(bool updating)
    {
        // TEMPORARY for ESPA-439 HIL soak evidence only: default flipped true so the [soak]
        // HIL run exercises CSI for its full 4h instead of sitting idle at the real default.
        // Revert to false before merge.
        enabled = Settings::checkbox("csi_enable", true, "WiFi CSI movement sensor (experimental)");
        if (!enabled || updating) { enabled = false; return; }
        kinds.reset(new (std::nothrow) Kind[kKinds]);
        queue.reset(new (std::nothrow) Slot[kQueue]);
        if (!kinds || !queue) {
            Log.println("CSI: not enough memory");
            enabled = false;
        }
    }

    void SerialReport()
    {
        if (!enabled) return;
        Log.print("CSI:          ");
        Log.println("enabled");
    }

    void Loop()
    {
        if (!enabled) return;

        for (uint32_t t = tail.load(std::memory_order_relaxed); t != head.load(std::memory_order_acquire); t++) {
            add(queue[t % kQueue]);
            tail.store(t + 1, std::memory_order_release);
        }

        if (millis() - windowStart < kWindowMs) return;
        windowStart = millis();

        // Publish the kind with the most frames: on most networks that is the node's own traffic.
        const Kind* best = nullptr;
        std::string summary;
        for (size_t i = 0; i < kindCount; i++) {
            const Kind& k = kinds[i];
            if (!best || k.window.frames() > best->window.frames()) best = &k;
            summary += Sprintf(" %s/%u/%u:%u=%.1f", (k.key & 0x8000) ? "grp" : "uni", (unsigned)((k.key >> 8) & 3),
                (unsigned)(k.key & 0xff), (unsigned)k.window.frames(), k.window.variance());
        }
        const uint32_t frames = best ? best->window.frames() : 0;
        fps = frames * 1000.0f / kWindowMs;
        const bool settling = !armed || millis() - armedAt < kSettleMs;
        Log.printf("CSI: window%s dropped=%u%s\n", summary.c_str(), (unsigned)dropped.exchange(0), settling ? " (settling)" : "");
        const bool publish = frames >= kMinFrames && !settling;
        if (publish)
            pub((roomsTopic + "/csi_variance").c_str(), 0, 0, Sprintf("%.2f", best->window.variance()).c_str());
        resetKinds();
        trackAp();
    }

    bool SendDiscovery()
    {
        if (!enabled) return true;

        return sendSensorDiscovery("CSI Variance", EC_DIAGNOSTIC);
    }

    void SendTelemetry()
    {
        if (!enabled) return;
        doc["csi_fps"] = serialized(Sprintf("%.1f", fps));
    }
}

#endif
