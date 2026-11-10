// Dev-only reference experiment. The pinned ESPectre SDK owns capture, sampling,
// calibration, weights, hit filtering, and radio policy; no detector is reimplemented.
#if defined(SENSORS) && defined(CSI_REFERENCE)
#include "CsiMotion.h"
#include "Settings.h"
#include "globals.h"
#include "espectre_sdk.h"
#include "esp_system.h"
#include "esp_timer.h"
#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <atomic>

namespace CsiMotion {
namespace {
espectre::RuntimeFrontendController runtime;
class Listener : public espectre::IRuntimeListener {
 public:
    char fault[160] = {};
    void on_runtime_fault(const char* message) override {
        std::snprintf(fault, sizeof(fault), "%s", message);
    }
} listener;
long mode = 0;
std::time_t expires = 0;
bool attempted = false;
bool running = false;
bool expired = false;
int64_t lastReport = 0;
int64_t monotonicExpiry = 0;
std::atomic<uint32_t> receivedAdverts{0};

void expire() {
    // Persist disarm before restarting. Reboot restores the ordinary WiFi and
    // coexistence policy, not just the SDK's detector state. A later reboot
    // cannot extend a capture's absolute deadline.
    if (!Settings::spurt("/csi_reference_mode", "0")) {
        Log.println("CSIREF fault: cannot persist disarm; absolute deadline remains expired");
    }
    runtime.shutdown();
    Log.println("CSIREF finished: deadline reached; restoring normal radio by reboot");
    esp_restart();
}
}

void NoteAdvert() {
    // Count callbacks delivered by BLE to the application, before fingerprint
    // filtering. A fingerprint New/Del event is a different measurement.
    receivedAdverts.fetch_add(1, std::memory_order_relaxed);
}

void ConnectToWifi(bool updating) {
    mode = Settings::dropdown("csi_reference_mode", {"Off", "Lightweight", "High Accuracy"}, 0,
                              "Dev-only pinned upstream reference detector");
    const auto deadline = Settings::string("csi_reference_until", "0", "Absolute UTC expiry (Unix seconds)");
    expires = static_cast<std::time_t>(std::strtoll(deadline.c_str(), nullptr, 10));
    if (updating || mode < 1 || mode > 2 || expires <= 0) mode = 0;
}

void SerialReport() {
    Log.printf("CSIREF mode=%ld until=%lld sdk=%s upstream=3edc7bdb4262e01b868c3d589fea59cfd0e8f555\n",
               mode, static_cast<long long>(expires), espectre::espectre_sdk_version());
}

void Loop() {
    if (!mode || expired) {
        const int64_t tick = esp_timer_get_time();
        if (tick - lastReport >= 1000000) {
            lastReport = tick;
            Log.printf("CSIREF {\"t_us\":%lld,\"utc\":%lld,\"detector\":\"off\",\"ready\":false,\"adverts\":%lu}\n",
                       static_cast<long long>(tick), static_cast<long long>(std::time(nullptr)),
                       static_cast<unsigned long>(receivedAdverts.load(std::memory_order_relaxed)));
        }
        return;
    }
    if (monotonicExpiry && esp_timer_get_time() >= monotonicExpiry) {
        expired = true; expire(); return;
    }
    const auto now = std::time(nullptr);
    // NTP has to establish a valid clock before any active traffic. Missing or
    // implausible deadlines never arm the reference radio.
    if (now < 1700000000) return;
    if (now >= expires) { expired = true; expire(); return; }
    if (!attempted) {
        attempted = true;
        if (expires - now > 48 * 60 * 60) {
            Log.println("CSIREF fault: deadline exceeds 48-hour experiment limit");
            return;
        }
        monotonicExpiry = esp_timer_get_time() + static_cast<int64_t>(expires - now) * 1000000;
        espectre::RuntimeConfig config;
        config.device_id = espectre::derive_runtime_device_id();
        config.detection_algorithm = mode == 1 ? espectre::DetectionAlgorithm::LIGHTWEIGHT
                                               : espectre::DetectionAlgorithm::HIGH_ACCURACY;
        config.csi_target_pps = 100;
        config.traffic_generator_mode = espectre::TrafficGeneratorMode::PING;
        // Deliberately retain upstream AUTO capture, 1s window, calibration,
        // weights, thresholds, temporal admission, and 4/3 motion hits.
        running = runtime.set_config(config) && runtime.setup(&listener);
        Log.printf("CSIREF setup=%s\n", running ? "ok" : "failed");
    }
    if (listener.fault[0]) {
        Log.printf("CSIREF fault: %s\n", listener.fault);
        listener.fault[0] = 0;
    }
    if (!running) return;
    runtime.loop();
    const int64_t tick = esp_timer_get_time();
    if (tick - lastReport < 250000) return;
    lastReport = tick;
    const auto& s = runtime.snapshot();
    const auto d = runtime.diagnostics();
    Log.printf("CSIREF {\"t_us\":%lld,\"utc\":%lld,\"detector\":\"%s\",\"ready\":%s,"
               "\"motion\":%s,\"score\":%.6f,\"threshold\":%.6f,\"calibrating\":%s,"
               "\"accepted\":%llu,\"admitted\":%llu,\"slots\":%lu,\"window_slots\":%lu,"
               "\"queue_drops\":%llu,\"tx\":%lu,\"rx\":%lu,\"heap\":%lu,\"largest\":%lu,\"adverts\":%lu}\n",
               static_cast<long long>(tick), static_cast<long long>(now), s.detector_name,
               s.ready_to_publish ? "true" : "false",
               s.motion_state == espectre::MotionState::MOTION ? "true" : "false",
               static_cast<double>(s.movement_metric), static_cast<double>(s.threshold),
               s.calibrating ? "true" : "false",
               static_cast<unsigned long long>(d.csi.accepted_total),
               static_cast<unsigned long long>(d.csi.admitted_total),
               static_cast<unsigned long>(d.csi.occupancy_slots),
               static_cast<unsigned long>(d.csi.window_slots),
               static_cast<unsigned long long>(d.csi.pending_frame_drops_total),
               static_cast<unsigned long>(d.traffic.tx_packets_total),
               static_cast<unsigned long>(d.traffic.rx_packets_total),
               static_cast<unsigned long>(d.platform.free_memory_bytes),
               static_cast<unsigned long>(d.platform.largest_free_memory_block_bytes),
               static_cast<unsigned long>(receivedAdverts.load(std::memory_order_relaxed)));
}

// Research output stays on the TCP log. It must not drive household automations
// or register an unvalidated motion entity in Home Assistant.
bool SendDiscovery() { return true; }
void SendTelemetry() {}
}
#endif
