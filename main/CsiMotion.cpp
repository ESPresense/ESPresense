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
#ifdef CSI_CAPTURE_DEBUG
        uint32_t ts;            // millis() at capture; dev builds only, see dumpCapture().
        bool firstWordInvalid;
#endif
    };

    struct Kind {
        uint16_t key = 0;
        CsiWindow window;
    };

    static bool enabled = false;
#ifdef CSI_CAPTURE_DEBUG
    static bool capture = false;
#endif
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
#ifdef CSI_CAPTURE_DEBUG
        s.ts = millis();
        s.firstWordInvalid = info->first_word_invalid;
#endif
        head.store(h + 1, std::memory_order_release);
    }

    // HT-LTF, so the subcarrier layout matches ESpectre's 56-live-bin HT20 reference
    // (github.com/francescopace/espectre, src/cpp/core/csi_format.h). L-LTF stays on as the
    // fallback for legacy-rate frames, which have no HT-LTF to report.
    static bool arm()
    {
        wifi_csi_config_t cfg = {};
#if CONFIG_SOC_WIFI_HE_SUPPORT
        cfg.enable = 1;
        cfg.acquire_csi_legacy = 1;
        cfg.acquire_csi_ht20 = 1;
#else
        cfg.lltf_en = true;
        cfg.htltf_en = true;
        cfg.stbc_htltf2_en = true;
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

#ifdef CSI_CAPTURE_DEBUG
    // Dev-only: every subcarrier present in the frame, unsliced. Deliberately not trimmed to
    // ESpectre's 56 live HT20 bins here - classic-MAC chips (esp32/esp32c3/esp32s3) deliver
    // CSI in Espressif's native "0..31,-32..-1" bin order (DC at bin 0), while HE chips
    // (esp32c6) deliver it already centered on DC (bin 32); telling the two apart is an
    // empirical check on real data (espectre's detect_ht20_bin_layout()), not something to
    // bake into firmware ahead of a capture. The offline tool does that rotation and the
    // 56-bin / 12-subcarrier selection from this raw dump.
    // Not named `s`: Sprintf's expansion declares its own local `s`, which would shadow a
    // same-named parameter and fail to compile.
    static void dumpCapture(const Slot& frame)
    {
        if (!capture || frame.len < 4) return;
        const size_t subcarriers = frame.len / 2;
        std::string line = Sprintf("CSICAP %lu %u %u %u", (unsigned long)frame.ts, frame.kind,
            (unsigned)frame.firstWordInvalid, (unsigned)subcarriers);
        for (size_t i = 0; i < subcarriers; i++) {
            const int imag = frame.csi[i * 2];
            const int real = frame.csi[i * 2 + 1];
            line += Sprintf(" %.1f", sqrtf(static_cast<float>(imag * imag + real * real)));
        }
        Log.println(line);
    }
#endif

    void ConnectToWifi(bool updating)
    {
        enabled = Settings::checkbox("csi_enable", false, "WiFi CSI movement sensor (experimental)");
#ifdef CSI_CAPTURE_DEBUG
        capture = Settings::checkbox("csi_capture", false, "Dump raw CSI amplitudes to the TCP log (dev build only)");
#endif
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
#ifdef CSI_CAPTURE_DEBUG
            dumpCapture(queue[t % kQueue]);
#endif
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
