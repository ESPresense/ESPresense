// Dev-only reference experiment. The pinned ESPectre SDK owns capture, sampling,
// calibration, weights, hit filtering, and radio policy; no detector is reimplemented.
#if defined(SENSORS) && defined(CSI_REFERENCE)
#include "CsiMotion.h"
#include "CsiReferenceFormat.h"
#include "Settings.h"
#include "globals.h"
#include "espectre_sdk.h"
#include "esp_system.h"
#include "esp_timer.h"
#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <atomic>
#include <type_traits>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

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
bool bleScanEnabled = true;  // Loaded before scanTask starts; reboot to change.
bool externalHost = false;  // Explicit diagnostic; PING remains the default.

// The SDK has one owner, independent of serial/MQTT and the ordinary loop's
// per-fingerprint yields. Match the pinned native frontend: 8 KiB, priority 5,
// 10 ms cadence. Capture, admission, calibration and detector code stay intact.
struct ReferenceReport {
    espectre::RuntimeSnapshot snapshot{};
    espectre::RuntimeDiagnosticsSnapshot diagnostics{};
    int64_t tick = 0;
    uint64_t utc = 0;
    uint32_t adverts = 0;
    uint32_t sequence = 0;
    uint32_t telemetryOverwrites = 0;
    uint32_t workerLoops = 0;
    uint32_t loopGapMaxUs = 0;
    uint32_t stackFree = 0;
    uint32_t faultSequence = 0;
    char fault[160] = {};
    bool active = false;
    bool attempted = false;
    bool running = false;
};
static_assert(std::is_trivially_copyable<ReferenceReport>::value,
              "FreeRTOS mailbox requires a value-only snapshot");
QueueHandle_t reports = nullptr;
bool workerStartAttempted = false;  // Main-loop owned.
ReferenceReport pending;           // Worker owned.

void fault(const char* message) {
    ++pending.faultSequence;
    std::snprintf(pending.fault, sizeof(pending.fault), "%s", message);
}

void expire() {
    runtime.shutdown();
    // Arming was durably consumed before setup. Reboot restores the ordinary
    // radio policy without a flash write at expiry or a possible reboot loop.
    if (!attempted) {
        mode = 0;
        return;
    }
    // Deadline enforcement must not wait on serial or the reporting loop.
    esp_restart();
}
}

void NoteAdvert() {
    // Count callbacks delivered by BLE to the application, before fingerprint
    // filtering. A fingerprint New/Del event is a different measurement.
    receivedAdverts.fetch_add(1, std::memory_order_relaxed);
}

bool BleScanEnabled() { return bleScanEnabled; }

void ConnectToWifi(bool updating) {
    bleScanEnabled = Settings::checkbox("csi_reference_ble_scan", true,
                                       "Reference control: retain ordinary BLE scanning");
    externalHost = Settings::checkbox("csi_reference_external_host", false,
                                      "Reference control: external unicast UDP traffic");
    mode = Settings::dropdown("csi_reference_mode", {"Off", "Lightweight", "High Accuracy"}, 0,
                              "Dev-only pinned upstream reference detector");
    const bool arm = Settings::checkbox("csi_reference_arm", false, "One-shot reference trial arming");
    const auto deadline = Settings::string("csi_reference_until", "0", "Absolute UTC expiry (Unix seconds)");
    expires = static_cast<std::time_t>(std::strtoll(deadline.c_str(), nullptr, 10));
    if (updating || !arm || mode < 1 || mode > 2 || expires <= 0) mode = 0;
}

void SerialReport() {
    char deadline[21];
    Log.printf("CSIREF mode=%ld until=%s sdk=%s upstream=3edc7bdb4262e01b868c3d589fea59cfd0e8f555\n",
               mode, csiReferenceDecimal(expires > 0 ? static_cast<uint64_t>(expires) : 0, deadline),
               espectre::espectre_sdk_version());
}

namespace {
void serviceRuntime() {
    if (!mode || expired) return;
    if (monotonicExpiry && esp_timer_get_time() >= monotonicExpiry) {
        expired = true; expire(); return;
    }
    const auto now = std::time(nullptr);
    // NTP has to establish a valid clock before any active traffic. Missing or
    // implausible deadlines never arm the reference radio.
    if (now < 1700000000) return;
    if (now >= expires) { expired = true; expire(); return; }
    if (!attempted) {
        if (expires - now > 48 * 60 * 60) {
            fault("deadline exceeds 48-hour experiment limit");
            mode = 0;
            return;
        }
        if (!Settings::spurt("/csi_reference_arm", "0")) {
            fault("cannot consume one-shot arm; sensing remains off");
            mode = 0;
            return;
        }
        attempted = true;
        monotonicExpiry = esp_timer_get_time() + static_cast<int64_t>(expires - now) * 1000000;
        espectre::RuntimeConfig config;
        config.device_id = espectre::derive_runtime_device_id();
        config.detection_algorithm = mode == 1 ? espectre::DetectionAlgorithm::LIGHTWEIGHT
                                               : espectre::DetectionAlgorithm::HIGH_ACCURACY;
        config.csi_target_pps = 100;
        config.traffic_generator_mode = externalHost ? espectre::TrafficGeneratorMode::EXTERNAL_HOST
                                                     : espectre::TrafficGeneratorMode::PING;
        if (externalHost) config.csi_traffic_multicast_group.clear();  // Unicast only, no group join.
        // Deliberately retain upstream AUTO capture, 1s window, calibration,
        // weights, thresholds, temporal admission, and 4/3 motion hits.
        running = runtime.set_config(config) && runtime.setup(&listener);
        if (!running) fault("SDK setup failed");
    }
    if (listener.fault[0]) {
        fault(listener.fault);
        listener.fault[0] = 0;
    }
    if (running) runtime.loop();
}

void referenceWorker(void*) {
    int64_t previousTick = 0;
    uint32_t gapMax = 0;
    while (true) {
        const int64_t begin = esp_timer_get_time();
        if (previousTick) {
            const auto gap = static_cast<uint32_t>(begin - previousTick);
            if (gap > gapMax) gapMax = gap;
        }
        previousTick = begin;
        ++pending.workerLoops;
        serviceRuntime();
        const int64_t tick = esp_timer_get_time();
        const bool active = mode && !expired && running;
        if (tick - lastReport >= (active ? 250000 : 1000000)) {
            lastReport = tick;
            pending.tick = tick;
            pending.utc = static_cast<uint64_t>(std::time(nullptr));
            pending.adverts = receivedAdverts.load(std::memory_order_relaxed);
            pending.active = active;
            pending.attempted = attempted;
            pending.running = running;
            pending.loopGapMaxUs = gapMax;
            pending.stackFree = uxTaskGetStackHighWaterMark(nullptr);
            gapMax = 0;
            if (active) {
                pending.snapshot = runtime.snapshot();
                pending.diagnostics = runtime.diagnostics();
            } else {
                pending.snapshot = {};
                pending.diagnostics = {};
                pending.diagnostics.platform.free_memory_bytes = esp_get_free_heap_size();
            }
            ++pending.sequence;
            if (uxQueueMessagesWaiting(reports)) ++pending.telemetryOverwrites;
            // Logging backpressure can replace telemetry, never CSI frames.
            // The distinct overwrite counter makes that loss visible.
            xQueueOverwrite(reports, &pending);
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
}  // namespace

void Loop() {
    if (!workerStartAttempted) {
        workerStartAttempted = true;
        reports = xQueueCreate(1, sizeof(ReferenceReport));
        if (!reports || xTaskCreate(referenceWorker, "csiref", 8192, nullptr, 5, nullptr) != pdPASS) {
            if (reports) vQueueDelete(reports);
            reports = nullptr;
            Log.println("CSIREF fault: cannot start reference worker; sensing remains off");
            return;
        }
    }
    ReferenceReport report;
    if (!reports || xQueueReceive(reports, &report, 0) != pdTRUE) return;
    static uint32_t lastFault = 0;
    if (report.faultSequence != lastFault) {
        lastFault = report.faultSequence;
        Log.printf("CSIREF fault: %s\n", report.fault);
    }
    static bool setupLogged = false;
    if (report.attempted && !setupLogged) {
        setupLogged = true;
        Log.printf("CSIREF setup=%s\n", report.running ? "ok" : "failed");
    }
    const auto& s = report.snapshot;
    const auto& d = report.diagnostics;
    char timestamp[21], utc[21], accepted[21], admitted[21], drops[21];
    char callbacks[21], filtered[21], provenance[21], invalidFirst[21];
    char missing[21], excess[21], stale[21], outOfOrder[21];
    if (!report.active) {
        Log.printf("CSIREF {\"t_us\":%s,\"utc\":%s,\"detector\":\"off\",\"ready\":false,"
                   "\"adverts\":%lu,\"heap\":%lu,\"worker_loops\":%lu,\"loop_gap_max_us\":%lu,"
                   "\"worker_stack_free\":%lu,\"report_seq\":%lu,\"telemetry_overwrites\":%lu,"
                   "\"ble_scan\":%s,\"software_reset\":%s,\"source\":\"%s\"}\n",
                   csiReferenceDecimal(static_cast<uint64_t>(report.tick), timestamp),
                   csiReferenceDecimal(report.utc, utc),
                   static_cast<unsigned long>(report.adverts),
                   static_cast<unsigned long>(d.platform.free_memory_bytes),
                   static_cast<unsigned long>(report.workerLoops),
                   static_cast<unsigned long>(report.loopGapMaxUs),
                   static_cast<unsigned long>(report.stackFree),
                   static_cast<unsigned long>(report.sequence),
                   static_cast<unsigned long>(report.telemetryOverwrites),
                   bleScanEnabled ? "true" : "false",
                   esp_reset_reason() == ESP_RST_SW ? "true" : "false",
                   externalHost ? "external_host" : "ping");
        return;
    }
    Log.printf("CSIREF {\"t_us\":%s,\"utc\":%s,\"detector\":\"%s\",\"ready\":%s,"
               "\"motion\":%s,\"score\":%.6f,\"threshold\":%.6f,\"calibrating\":%s,"
               "\"accepted\":%s,\"admitted\":%s,\"slots\":%lu,\"window_slots\":%lu,"
               "\"queue_drops\":%s,\"generated\":%lu,\"tx\":%lu,\"rx\":%lu,\"heap\":%lu,\"largest\":%lu,\"adverts\":%lu,"
               "\"worker_loops\":%lu,\"loop_gap_max_us\":%lu,\"worker_stack_free\":%lu,"
               "\"report_seq\":%lu,\"telemetry_overwrites\":%lu,\"ble_scan\":%s,\"source\":\"%s\","
               "\"callbacks\":%s,\"filtered\":%s,\"provenance_rejected\":%s,\"invalid_first_word\":%s,"
               "\"missing_slots\":%s,\"excess\":%s,\"stale\":%s,\"out_of_order\":%s}\n",
               csiReferenceDecimal(static_cast<uint64_t>(report.tick), timestamp),
               csiReferenceDecimal(report.utc, utc), s.detector_name,
               s.ready_to_publish ? "true" : "false",
               s.motion_state == espectre::MotionState::MOTION ? "true" : "false",
               static_cast<double>(s.movement_metric), static_cast<double>(s.threshold),
               s.calibrating ? "true" : "false",
               csiReferenceDecimal(d.csi.accepted_total, accepted),
               csiReferenceDecimal(d.csi.admitted_total, admitted),
               static_cast<unsigned long>(d.csi.occupancy_slots),
               static_cast<unsigned long>(d.csi.window_slots),
               csiReferenceDecimal(d.csi.pending_frame_drops_total, drops),
               static_cast<unsigned long>(d.traffic.generator_packets_total),
               static_cast<unsigned long>(d.traffic.tx_packets_total),
               static_cast<unsigned long>(d.traffic.rx_packets_total),
               static_cast<unsigned long>(d.platform.free_memory_bytes),
               static_cast<unsigned long>(d.platform.largest_free_memory_block_bytes),
               static_cast<unsigned long>(report.adverts),
               static_cast<unsigned long>(report.workerLoops),
               static_cast<unsigned long>(report.loopGapMaxUs),
               static_cast<unsigned long>(report.stackFree),
               static_cast<unsigned long>(report.sequence),
               static_cast<unsigned long>(report.telemetryOverwrites),
               bleScanEnabled ? "true" : "false",
               externalHost ? "external_host" : "ping",
               csiReferenceDecimal(d.csi.callbacks_total, callbacks),
               csiReferenceDecimal(d.csi.filtered_total, filtered),
               csiReferenceDecimal(d.csi.provenance_rejected_total, provenance),
               csiReferenceDecimal(d.csi.invalid_first_word_total, invalidFirst),
               csiReferenceDecimal(d.csi.missing_slots_total, missing),
               csiReferenceDecimal(d.csi.excess_total, excess),
               csiReferenceDecimal(d.csi.stale_total, stale),
               csiReferenceDecimal(d.csi.out_of_order_total, outOfOrder));
}

// Research output stays on the TCP log. It must not drive household automations
// or register an unvalidated motion entity in Home Assistant.
bool SendDiscovery() { return true; }
void SendTelemetry() {}
}
#endif
