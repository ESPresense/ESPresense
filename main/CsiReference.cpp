// Dev-only reference experiment. The pinned ESPectre SDK owns capture, sampling,
// calibration, weights, hit filtering, and radio policy; no detector is reimplemented.
#if defined(SENSORS) && defined(CSI_REFERENCE)
#include "CsiMotion.h"
#include "CsiReferenceCoex.h"
#include "CsiReferenceFormat.h"
#include "CsiReferenceScan.h"
#include "Ble.h"
#include "defaults.h"
#include "Settings.h"
#include "globals.h"
#include "espectre_sdk.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "esp_wifi.h"
#include <cstring>
#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <atomic>
#include <type_traits>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

// The schedule tables and their stock bytes were read from the ESP32-C3
// library only. Other chips keep their own library untouched: no override,
// no readback, and a schedule request is ignored.
#ifdef ESP32C3
extern "C" {
// Strong definitions of the coexistence library's two weak schedules for
// "BLE active, BT idle, Wi-Fi connected". The initial bytes equal the
// library's, so every boot starts on the stock split; only an armed, expiring
// trial changes the shares, and a reboot restores them. Pilot 2047 found the
// scanning controller on the 60/40 table, not the 50/50 one.
__attribute__((used)) volatile uint8_t coex_schm_ble_mesh_traffic_wifi_conn[10] = {2, 1, 60, 3, 0, 0, 40, 0, 0, 0};
__attribute__((used)) volatile uint8_t coex_schm_ble_default_bt_idle_wifi_conn[10] = {2, 1, 50, 3, 0, 0, 50, 0, 0, 0};
// Vendor accessors (esp_coex private header), used read-only for telemetry.
void* coex_schm_curr_phase_get(void);
int coex_schm_curr_phase_idx_get(void);
}
static_assert(sizeof(coex_schm_ble_mesh_traffic_wifi_conn) == sizeof(CSI_REFERENCE_COEX_SCAN_TABLE)
              && sizeof(coex_schm_ble_default_bt_idle_wifi_conn) == sizeof(CSI_REFERENCE_COEX_DEFAULT_TABLE),
              "overrides must keep the library table sizes");
#endif

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
long scanProfile = 0;
bool trialRequested = false;
std::atomic<bool> trialStarted{false};
// Armed radio-sharing plan. Loaded at boot, applied by the worker only after
// the one-shot arm is consumed, gone after the expiry reboot.
CsiReferenceCoexRequest coexRequest;
CsiReferenceScanCycle scanCycle;
char coexText[16] = "0";
char scanCycleText[16] = "0";
std::atomic<bool> scanPaused{false};
bool planApplied = false;       // Worker owned from here down.
bool assistDone = false;
bool assistTimedOut = false;
int64_t assistStart = 0;
int64_t assistPoll = 0;
uint32_t assistMs = 0;
int64_t planStart = 0;
constexpr int64_t ASSIST_LIMIT_US = 90 * 1000000LL;

// Read back the schedule the library is running right now. A successful API
// return proves nothing about the radio, so telemetry carries the live bytes.
struct CoexLive {
    char hex[2 * 26 + 1] = {};
    int phase = -1;
    bool own = false;
};
void readCoexLive(CoexLive& live) {
    live = {};
#ifdef ESP32C3
    for (int attempt = 0; attempt < 3; ++attempt) {
        const int before = coex_schm_curr_phase_idx_get();
        const auto* phase = static_cast<const uint8_t*>(coex_schm_curr_phase_get());
        const int after = coex_schm_curr_phase_idx_get();
        if (!phase || before < 0 || before > 5 || before != after) continue;
        const uint8_t* table = phase - 2 - 4 * before;
        const unsigned phases = table[0];
        if (phases == 0 || phases > 6 || unsigned(before) >= phases) continue;
        static const char digits[] = "0123456789abcdef";
        for (unsigned i = 0; i < 2 + 4 * phases; ++i) {
            live.hex[2 * i] = digits[table[i] >> 4];
            live.hex[2 * i + 1] = digits[table[i] & 15];
        }
        live.phase = before;
        live.own = table == const_cast<const uint8_t*>(coex_schm_ble_mesh_traffic_wifi_conn)
                   || table == const_cast<const uint8_t*>(coex_schm_ble_default_bt_idle_wifi_conn);
        return;
    }
#endif
}

// The SDK has one owner, independent of serial/MQTT and the ordinary loop's
// per-fingerprint yields. Match the pinned native frontend: 8 KiB, priority 5,
// 10 ms cadence. Capture, admission, calibration and detector code stay intact.
struct ReferenceReport {
    espectre::RuntimeSnapshot snapshot{};
    espectre::RuntimeDiagnosticsSnapshot diagnostics{};
    int64_t tick = 0;
    uint64_t utc = 0;
    uint32_t adverts = 0;
    unsigned scanWindow = 0;
    unsigned requestedWindow = 0;
    bool trialActive = false;
    CoexLive coexLive{};
    bool planApplied = false;
    bool scanPaused = false;
    bool assistTimedOut = false;
    uint32_t assistMs = 0;
    uint32_t sequence = 0;
    uint32_t telemetryOverwrites = 0;
    uint32_t workerLoops = 0;
    uint32_t loopGapMaxUs = 0;
    uint32_t stackFree = 0;
    int wifiPs = -1;
    int wifiPsGetError = -1;
    uint32_t faultSequence = 0;
    char fault[160] = {};
    bool active = false;
    bool attempted = false;
    bool running = false;
};
static_assert(std::is_trivially_copyable<ReferenceReport>::value,
              "FreeRTOS mailbox requires a value-only snapshot");
struct PolicyWarning {
    char message[192] = {};
    int line = 0;
    unsigned level = 0;
    bool truncated = false;
};
QueueHandle_t policyWarnings = nullptr;
std::atomic<uint32_t> policyWarningDrops{0};
bool policyLogEnabled(void*, espectre::LogLevel level, const char* tag) {
    return tag && std::strcmp(tag, "WiFiLifecycle") == 0
           && static_cast<unsigned>(level) <= static_cast<unsigned>(espectre::LogLevel::WARNING);
}
void policyLogWrite(void*, espectre::LogLevel level, const char*, int line, const char* format, va_list args) {
    PolicyWarning warning;
    warning.line = line;
    warning.level = static_cast<unsigned>(level);
    const int n = std::vsnprintf(warning.message, sizeof(warning.message), format, args);
    warning.truncated = n < 0 || static_cast<size_t>(n) >= sizeof(warning.message);
    // SDK/event tasks never wait for a console or network consumer.
    if (xQueueSend(policyWarnings, &warning, 0) != pdTRUE)
        policyWarningDrops.fetch_add(1, std::memory_order_relaxed);
}
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
    if (!trialStarted.load(std::memory_order_acquire)) {
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
bool BleScanPaused() { return scanPaused.load(std::memory_order_relaxed); }
unsigned RequestedBleScanWindowUnits() {
    return csiReferenceScanWindowUnits(scanProfile, trialStarted.load(std::memory_order_acquire), BLE_SCAN_WINDOW);
}

void ConnectToWifi(bool updating) {
    bleScanEnabled = Settings::checkbox("csi_reference_ble_scan", true,
                                       "Reference control: retain ordinary BLE scanning");
    externalHost = Settings::checkbox("csi_reference_external_host", false,
                                      "Reference control: external unicast UDP traffic");
    scanProfile = Settings::dropdown("csi_reference_scan_profile", {"Full", "Quarter", "Eighth"}, 0,
                                     "Armed bench scan-window cost diagnostic; resets to full after reboot");
    mode = Settings::dropdown("csi_reference_mode", {"Off", "Lightweight", "High Accuracy"}, 0,
                              "Dev-only pinned upstream reference detector");
    const auto coex = Settings::string("csi_reference_coex", "0",
                                       "Armed bench Wi-Fi/BLE schedule P,W,B; 0 keeps the library split");
    const auto cycle = Settings::string("csi_reference_scan_cycle", "0",
                                        "Armed bench BLE scan pause C,T seconds; 0 scans continuously");
#ifdef ESP32C3
    coexRequest = csiReferenceParseCoex(coex.c_str());
#else
    coexRequest = {};
#endif
    scanCycle = csiReferenceParseScanCycle(cycle.c_str());
    // Echo only what parsed, in canonical form; a rejected request reads "0".
    std::snprintf(coexText, sizeof(coexText), "0");
    std::snprintf(scanCycleText, sizeof(scanCycleText), "0");
    if (coexRequest.valid)
        std::snprintf(coexText, sizeof(coexText), "%u,%u,%u", unsigned(coexRequest.period),
                      unsigned(coexRequest.wifi), unsigned(coexRequest.bt));
    if (scanCycle.valid)
        std::snprintf(scanCycleText, sizeof(scanCycleText), "%u,%u", unsigned(scanCycle.pauseSeconds),
                      unsigned(scanCycle.periodSeconds));
    const bool arm = Settings::checkbox("csi_reference_arm", false, "One-shot reference trial arming");
    const auto deadline = Settings::string("csi_reference_until", "0", "Absolute UTC expiry (Unix seconds)");
    expires = static_cast<std::time_t>(std::strtoll(deadline.c_str(), nullptr, 10));
    trialRequested = !updating && arm && mode >= 0 && mode <= 2 && expires > 0
                     && (mode > 0 || (scanProfile >= 1 && scanProfile <= 2)
                         || coexRequest.valid || scanCycle.valid);
    if (!trialRequested) mode = 0;
}

void SerialReport() {
    char deadline[21];
    Log.printf("CSIREF mode=%ld until=%s sdk=%s upstream=3edc7bdb4262e01b868c3d589fea59cfd0e8f555\n",
               mode, csiReferenceDecimal(expires > 0 ? static_cast<uint64_t>(expires) : 0, deadline),
               espectre::espectre_sdk_version());
}

namespace {
// Runs only once the one-shot arm is consumed. With a whole-second plan the
// SDK first calibrates with scanning paused (bounded); then the requested
// schedule shares and scan pause cycle take effect until the expiry reboot.
void serviceRadioPlan(bool sdkRunning) {
    const int64_t now = esp_timer_get_time();
    if (!assistDone) {
        if (sdkRunning && csiReferenceNeedsCalibrationAssist(coexRequest, scanCycle)) {
            if (!assistStart) {
                assistStart = now;
#ifdef ESP32C3
                // The table the library runs once the scan is cancelled.
                csiReferenceApplyCoex(coex_schm_ble_default_bt_idle_wifi_conn, CSI_REFERENCE_COEX_PAUSED_SHARE);
#endif
            }
            bool finished = false;
            if (now - assistPoll >= 250000) {
                assistPoll = now;
                finished = runtime.snapshot().ready_to_publish;
            }
            if (!finished && now - assistStart >= ASSIST_LIMIT_US) finished = assistTimedOut = true;
            if (!finished) {
                scanPaused.store(true, std::memory_order_relaxed);
                return;
            }
            assistMs = static_cast<uint32_t>((now - assistStart) / 1000);
        }
        assistDone = true;
    }
    if (!planApplied) {
        planApplied = true;
        planStart = now;
#ifdef ESP32C3
        // Scanning runs the 60/40 table. The 50/50 table is what runs while a
        // scan is cancelled: give it to Wi-Fi during a pause cycle, otherwise
        // it follows the request (or returns to stock after an assist).
        csiReferenceApplyCoex(coex_schm_ble_mesh_traffic_wifi_conn, coexRequest);
        csiReferenceApplyCoex(coex_schm_ble_default_bt_idle_wifi_conn,
                              scanCycle.valid ? CSI_REFERENCE_COEX_PAUSED_SHARE
                              : coexRequest.valid ? coexRequest : CSI_REFERENCE_COEX_DEFAULT_SHARE);
#endif
    }
    scanPaused.store(csiReferenceScanPausedAt(static_cast<uint64_t>(now - planStart) / 1000, scanCycle),
                     std::memory_order_relaxed);
}

void serviceRuntime() {
    if (expired || (!trialRequested && !trialStarted.load(std::memory_order_acquire))) return;
    if (monotonicExpiry && esp_timer_get_time() >= monotonicExpiry) {
        expired = true; expire(); return;
    }
    const auto now = std::time(nullptr);
    // NTP has to establish a valid clock before any active traffic. Missing or
    // implausible deadlines never arm the reference radio.
    if (now < 1700000000) return;
    if (now >= expires) { expired = true; expire(); return; }
    if (!trialStarted.load(std::memory_order_acquire)) {
        if (expires - now > 48 * 60 * 60) {
            fault("deadline exceeds 48-hour experiment limit");
            mode = 0;
            trialRequested = false;
            return;
        }
        if (!Settings::spurt("/csi_reference_arm", "0")) {
            fault("cannot consume one-shot arm; sensing remains off");
            mode = 0;
            trialRequested = false;
            return;
        }
        monotonicExpiry = esp_timer_get_time() + static_cast<int64_t>(expires - now) * 1000000;
        trialStarted.store(true, std::memory_order_release);
    }
    // Reduced BLE-only baselines share the same persistent/monotonic expiry.
    if (!mode) {
        serviceRadioPlan(false);
        return;
    }
    if (!attempted) {
        attempted = true;
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
        wifi_ps_type_t powerSave;
        pending.wifiPsGetError = esp_wifi_get_ps(&powerSave);
        if (pending.wifiPsGetError == ESP_OK) pending.wifiPs = static_cast<int>(powerSave);
        if (!running) fault("SDK setup failed");
    }
    if (listener.fault[0]) {
        fault(listener.fault);
        listener.fault[0] = 0;
    }
    if (running) runtime.loop();
    serviceRadioPlan(running);
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
            pending.scanWindow = Ble::ConfiguredScanWindowUnits();
            pending.requestedWindow = RequestedBleScanWindowUnits();
            pending.trialActive = trialStarted.load(std::memory_order_acquire);
            readCoexLive(pending.coexLive);
            pending.planApplied = planApplied;
            pending.scanPaused = scanPaused.load(std::memory_order_relaxed);
            pending.assistTimedOut = assistTimedOut;
            pending.assistMs = assistMs;
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
        policyWarnings = xQueueCreate(8, sizeof(PolicyWarning));
        if (!reports || !policyWarnings
            || !espectre::set_log_sink({nullptr, policyLogEnabled, policyLogWrite})
            || xTaskCreate(referenceWorker, "csiref", 8192, nullptr, 5, nullptr) != pdPASS) {
            espectre::clear_log_sink();
            if (policyWarnings) vQueueDelete(policyWarnings);
            policyWarnings = nullptr;
            if (reports) vQueueDelete(reports);
            reports = nullptr;
            Log.println("CSIREF fault: cannot start reference worker; sensing remains off");
            return;
        }
    }
    PolicyWarning warning;
    for (unsigned i = 0; i < 8 && policyWarnings && xQueueReceive(policyWarnings, &warning, 0) == pdTRUE; ++i) {
        // Inspected WiFiLifecycle warning templates contain policy/error data,
        // not household tag identifiers. Bounded queue loss stays visible.
        Log.printf("CSIREF sdk_policy level=%u line=%d truncated=%s queue_drops=%lu message=%s\n",
                   warning.level, warning.line, warning.truncated ? "true" : "false",
                   static_cast<unsigned long>(policyWarningDrops.load(std::memory_order_relaxed)), warning.message);
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
        Log.printf("CSIREF setup=%s wifi_ps_at_setup=%d wifi_ps_get_error=%d policy_warning_drops=%lu\n",
                   report.running ? "ok" : "failed", report.wifiPs, report.wifiPsGetError,
                   static_cast<unsigned long>(policyWarningDrops.load(std::memory_order_relaxed)));
    }
    const auto& s = report.snapshot;
    const auto& d = report.diagnostics;
    char timestamp[21], utc[21], accepted[21], admitted[21], drops[21];
    char callbacks[21], filtered[21], provenance[21], invalidFirst[21];
    char missing[21], excess[21], stale[21], outOfOrder[21];
    char scanCondition[448];
    std::snprintf(scanCondition, sizeof(scanCondition),
                  "\"scan_profile\":%ld,\"scan_window_units\":%u,\"scan_interval_units\":%u,"
                  "\"scan_requested_units\":%u,\"trial_active\":%s,"
                  "\"coex\":\"%s\",\"scan_cycle\":\"%s\",\"plan_applied\":%s,\"scan_paused\":%s,"
                  "\"assist_ms\":%lu,\"assist_timed_out\":%s,"
                  "\"coex_live\":\"%s\",\"coex_phase\":%d,\"coex_own\":%s",
                  scanProfile, report.scanWindow, unsigned(BLE_SCAN_INTERVAL), report.requestedWindow,
                  report.trialActive ? "true" : "false",
                  coexText, scanCycleText, report.planApplied ? "true" : "false",
                  report.scanPaused ? "true" : "false",
                  static_cast<unsigned long>(report.assistMs), report.assistTimedOut ? "true" : "false",
                  report.coexLive.hex, report.coexLive.phase, report.coexLive.own ? "true" : "false");
    if (!report.active) {
        Log.printf("CSIREF {\"t_us\":%s,\"utc\":%s,\"detector\":\"off\",\"ready\":false,"
                   "\"adverts\":%lu,\"heap\":%lu,\"worker_loops\":%lu,\"loop_gap_max_us\":%lu,"
                   "\"worker_stack_free\":%lu,\"report_seq\":%lu,\"telemetry_overwrites\":%lu,"
                   "\"ble_scan\":%s,\"software_reset\":%s,\"source\":\"%s\",%s}\n",
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
                   externalHost ? "external_host" : "ping", scanCondition);
        return;
    }
    Log.printf("CSIREF {\"t_us\":%s,\"utc\":%s,\"detector\":\"%s\",\"ready\":%s,"
               "\"motion\":%s,\"score\":%.6f,\"threshold\":%.6f,\"calibrating\":%s,"
               "\"accepted\":%s,\"admitted\":%s,\"slots\":%lu,\"window_slots\":%lu,"
               "\"queue_drops\":%s,\"generated\":%lu,\"tx\":%lu,\"rx\":%lu,\"heap\":%lu,\"largest\":%lu,\"adverts\":%lu,"
               "\"worker_loops\":%lu,\"loop_gap_max_us\":%lu,\"worker_stack_free\":%lu,"
               "\"report_seq\":%lu,\"telemetry_overwrites\":%lu,\"ble_scan\":%s,\"source\":\"%s\",%s,"
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
               externalHost ? "external_host" : "ping", scanCondition,
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
