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
        if (!info || !info->buf || !(info->dmac[0] & 1)) return;
        if (packMac(info->mac) != apMac.load(std::memory_order_relaxed)) return;
        if (slotFull.load(std::memory_order_acquire)) return;
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
        return esp_wifi_set_csi_rx_cb(&onCsi, nullptr) == ESP_OK
            && esp_wifi_set_csi_config(&cfg) == ESP_OK
            && esp_wifi_set_csi(true) == ESP_OK;
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
