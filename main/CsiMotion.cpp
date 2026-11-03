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
    // Sized for the 2-3 frames/s measured on a typical network (see onCsi).
    static const unsigned long kWindowMs = 30000;
    // Below this the channel isn't sampled often enough for a window to mean anything.
    static const uint32_t kMinFrames = 10;

    struct Slot {
        int8_t csi[CsiWindow::kMaxBytes];
        size_t len;
    };

    static bool enabled = false;
    static bool armed = false;
    static unsigned long windowStart = 0;
    static float fps = 0;
    static std::unique_ptr<CsiWindow> window;
    static std::unique_ptr<Slot> slot;
    static std::atomic<bool> slotFull{false};
    static std::atomic<uint64_t> apMac{0};
    // DIAG: where frames go.
    static std::atomic<uint32_t> dTotal{0}, dNonGroup{0}, dWrongMac{0}, dBusy{0}, dOk{0};
    static std::atomic<uint64_t> dLastGroupSrc{0}, dLastAnySrc{0};
    static std::atomic<int> dLastLen{0}, dLastRate{-1}, dLastSig{-1}, dLastGroupRate{-1}, dLastGroupSig{-1};

    static uint64_t packMac(const uint8_t* mac)
    {
        uint64_t packed = 0;
        memcpy(&packed, mac, 6);
        return packed;
    }

    // WiFi task context, for every frame: a fixed-size copy and nothing else. Loop does the maths.
    // A frame that arrives while the last one is unread is dropped; the window copes.
    //
    // Only the AP's group-addressed frames count. It sends them at a fixed basic rate with no
    // beamforming, so they compare with each other. Frames unicast to this node vary with rate
    // adaptation and read noisier on a still bench. Beacons would qualify but don't arrive
    // here, so the sample rate is however much broadcast and multicast the network carries.
    static void onCsi(void*, wifi_csi_info_t* info)
    {
        if (!info || !info->buf) return;
        dTotal++;
        dLastAnySrc.store(packMac(info->mac), std::memory_order_relaxed);
        dLastLen.store(info->len, std::memory_order_relaxed);
        dLastRate.store(info->rx_ctrl.rate, std::memory_order_relaxed);
#if !CONFIG_SOC_WIFI_HE_SUPPORT
        dLastSig.store(info->rx_ctrl.sig_mode, std::memory_order_relaxed);
#endif
        if (!(info->dmac[0] & 1)) { dNonGroup++; return; }
        dLastGroupSrc.store(packMac(info->mac), std::memory_order_relaxed);
        dLastGroupRate.store(info->rx_ctrl.rate, std::memory_order_relaxed);
#if !CONFIG_SOC_WIFI_HE_SUPPORT
        dLastGroupSig.store(info->rx_ctrl.sig_mode, std::memory_order_relaxed);
#endif
        if (packMac(info->mac) != apMac.load(std::memory_order_relaxed)) { dWrongMac++; return; }
        if (slotFull.load(std::memory_order_acquire)) { dBusy++; return; }
        dOk++;
        const size_t len = info->len < sizeof(slot->csi) ? info->len : sizeof(slot->csi);
        memcpy(slot->csi, info->buf, len);
        slot->len = len;
        slotFull.store(true, std::memory_order_release);
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
        esp_err_t a = esp_wifi_set_csi_rx_cb(&onCsi, nullptr);
        esp_err_t b = esp_wifi_set_csi_config(&cfg);
        esp_err_t c = esp_wifi_set_csi(true);
        Log.printf("CSIDIAG arm: rx_cb=%s config=%s csi=%s\n", esp_err_to_name(a), esp_err_to_name(b), esp_err_to_name(c));
        return a == ESP_OK && b == ESP_OK && c == ESP_OK;
    }

    // Follows the AP the node is on. Ethernet nodes have none, so they never start.
    static void trackAp()
    {
        wifi_ap_record_t ap;
        const uint64_t mac = esp_wifi_sta_get_ap_info(&ap) == ESP_OK ? packMac(ap.bssid) : 0;
        if (mac == apMac.load(std::memory_order_relaxed)) {
            if (mac && !armed) armed = arm();
            return;
        }
        apMac.store(mac, std::memory_order_relaxed);
        window->reset();  // a different AP is a different channel, not movement
        if (mac) armed = arm();
    }

    void ConnectToWifi(bool updating)
    {
        enabled = Settings::checkbox("csi_enable", false, "WiFi CSI movement sensor (experimental)");
        if (!enabled || updating) { enabled = false; return; }
        window.reset(new (std::nothrow) CsiWindow());
        slot.reset(new (std::nothrow) Slot());
        if (!window || !slot) {
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

        if (slotFull.load(std::memory_order_acquire)) {
            window->add(slot->csi, slot->len);
            slotFull.store(false, std::memory_order_release);
        }

        if (millis() - windowStart < kWindowMs) return;
        windowStart = millis();

        fps = window->frames() * 1000.0f / kWindowMs;
        if (window->frames() >= kMinFrames)
            pub((roomsTopic + "/csi_variance").c_str(), 0, 0, Sprintf("%.2f", window->variance()).c_str());
        window->reset();
        trackAp();
        {
            wifi_ap_record_t ap;
            bool have = esp_wifi_sta_get_ap_info(&ap) == ESP_OK;
            wifi_ps_type_t ps = WIFI_PS_NONE;
            esp_wifi_get_ps(&ps);
            auto m = [](uint64_t v) { const uint8_t* b = (const uint8_t*)&v; return Sprintf("%02x:%02x:%02x:%02x:%02x:%02x", b[0], b[1], b[2], b[3], b[4], b[5]); };
            Log.printf("CSIDIAG win: armed=%d ap=%s ch=%d rssi=%d ps=%d | total=%u nonGroup=%u wrongMac=%u busy=%u ok=%u | lastAny=%s len=%d rate=%d sig=%d | lastGroup=%s rate=%d sig=%d | apMac=%s\n",
                armed, have ? m(packMac(ap.bssid)).c_str() : "-", have ? ap.primary : -1, have ? ap.rssi : 0, (int)ps,
                (unsigned)dTotal.exchange(0), (unsigned)dNonGroup.exchange(0), (unsigned)dWrongMac.exchange(0), (unsigned)dBusy.exchange(0), (unsigned)dOk.exchange(0),
                m(dLastAnySrc.load()).c_str(), dLastLen.load(), dLastRate.load(), dLastSig.load(),
                m(dLastGroupSrc.load()).c_str(), dLastGroupRate.load(), dLastGroupSig.load(), m(apMac.load()).c_str());
        }
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
