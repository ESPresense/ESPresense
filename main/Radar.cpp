#include "Radar.h"

#include <string>

#include "C4001.h"
#include "GUI.h"
#include "LD2410.h"
#include "LD2450.h"
#include "Settings.h"
#include "driver/uart.h"
#include "globals.h"
#include "mqtt.h"
#include "soc/soc_caps.h"
#include "string_utils.h"
#include "util.h"

// Defined in main.cpp: set once status=online has gone out.
extern bool online;

namespace Radar {
namespace {
#if SOC_UART_HP_NUM > 2
constexpr uart_port_t RADAR_UART = UART_NUM_2;
#else
constexpr uart_port_t RADAR_UART = UART_NUM_1;
#endif
constexpr unsigned long PUBLISH_MS = 1000;  // numeric values, as ESPHome's ld2410 throttle
constexpr unsigned long STALE_MS = 5000;    // no frames: unplugged or wrong pin/baud

enum class Model : uint8_t { None, LD2410, LD2450, C4001 };
const char* const MODEL_NAMES[] = {"", "LD2410", "LD2450", "C4001"};

Model model = Model::None;
int rxPin = -1;
int baud = 0;

Ld2410Parser ld2410;
Ld2450Parser ld2450;
C4001Parser c4001;
Ld2410Reading r2410;
Ld2450Reading r2450;
C4001Reading r4001;

// What gets published: presence, moving and still (ON/OFF on change), then numeric values.
constexpr int MAX_VALUES = 13;  // LD2450: a count and 4 per target
struct State {
    bool present = false, moving = false, still = false;
    int values[MAX_VALUES] = {};
    int count = 0;
};
State cur, sent;
bool haveFrame = false, sentValues = false, warnedStale = false;
int8_t present = -1, moving = -1, still = -1;  // last published; -1 before the first
unsigned long lastFrame = 0, lastPublish = 0;

// Numeric topics in State::values order, per model.
const char* const LD2410_TOPICS[] = {"radar_moving_distance", "radar_still_distance", "radar_detection_distance",
                                     "radar_moving_energy", "radar_still_energy"};
const char* const LD2450_TOPICS[] = {"radar_targets",
                                     "radar_target_1_x", "radar_target_1_y", "radar_target_1_speed", "radar_target_1_distance",
                                     "radar_target_2_x", "radar_target_2_y", "radar_target_2_speed", "radar_target_2_distance",
                                     "radar_target_3_x", "radar_target_3_y", "radar_target_3_speed", "radar_target_3_distance"};
const char* const C4001_TOPICS[] = {"radar_distance", "radar_speed", "radar_energy"};

const char* const* topics() {
    switch (model) {
        case Model::LD2410: return LD2410_TOPICS;
        case Model::LD2450: return LD2450_TOPICS;
        default: return C4001_TOPICS;
    }
}

// Moving / still: the LD2410 reports them; the LD2450 and the C4001 in speed mode give a speed.
// A C4001 in presence mode only says present.
bool hasMotionState() { return model != Model::C4001; }

int cm(int mm) { return (mm + (mm < 0 ? -5 : 5)) / 10; }

// Feeds a byte to this model's parser and, on a complete frame, refreshes cur.
bool feed(uint8_t b) {
    State s;
    switch (model) {
        case Model::LD2410: {
            if (!ld2410.feed(b, r2410)) return false;
            s.present = r2410.moving || r2410.still;
            s.moving = r2410.moving;
            s.still = r2410.still;
            int v[] = {r2410.movingCm, r2410.stillCm, r2410.detectionCm, r2410.movingEnergy, r2410.stillEnergy};
            for (int x : v) s.values[s.count++] = x;
            break;
        }
        case Model::LD2450: {
            if (!ld2450.feed(b, r2450)) return false;
            s.values[s.count++] = r2450.count();
            for (auto& t : r2450.targets) {
                if (t.present) {
                    s.present = true;
                    (t.speed ? s.moving : s.still) = true;
                }
                s.values[s.count++] = cm(t.x);
                s.values[s.count++] = cm(t.y);
                s.values[s.count++] = t.speed * 10;  // mm/s: HA's speed class has no cm/s
                s.values[s.count++] = cm(t.distance());
            }
            break;
        }
        case Model::C4001: {
            if (!c4001.feed(b, r4001)) return false;
            s.present = r4001.present;
            if (r4001.speedMode) {
                s.values[s.count++] = r4001.distanceCm;
                s.values[s.count++] = r4001.speedCms * 10;
                s.values[s.count++] = (int)r4001.energy;
            }
            break;
        }
        default: return false;
    }
    cur = s;
    return true;
}

bool pubTopic(const char* name, const std::string& value) {
    return pub((roomsTopic + "/" + name).c_str(), 0, true, value.c_str());
}

// Publishes an ON/OFF topic when it changes; returns whether it did.
bool pubBinary(const char* name, bool on, int8_t& last) {
    if (last == (int8_t)on) return false;
    last = on;
    pubTopic(name, on ? "ON" : "OFF");
    return true;
}

bool sameValues(const State& a, const State& b) {
    if (a.count != b.count) return false;
    for (int i = 0; i < a.count; i++)
        if (a.values[i] != b.values[i]) return false;
    return true;
}

bool sensor(const char* name, const char* devClass, const char* units, const char* category = EC_NONE) {
    return sendSensorDiscovery(name, category, devClass, units);
}
}  // namespace

void ConnectToWifi(bool updating) {
    std::string text = Settings::json("radar", "{}", "Radar", {{"rx", PinUse::Input}});
    DynamicJsonDocument cfg(text.size() * 2 + 128);
    model = Model::None;
    if (deserializeJson(cfg, text) || !cfg.is<JsonObject>()) return;
    std::string m = cfg["model"] | "";
    Model chosen = m == "ld2410" ? Model::LD2410 : m == "ld2450" ? Model::LD2450 : m == "c4001" ? Model::C4001 : Model::None;
    rxPin = cfg["rx"] | -1;
    baud = cfg["baud"] | (chosen == Model::C4001 ? 9600 : 256000);
    if (chosen == Model::None || rxPin < 0 || baud <= 0) return;
    model = chosen;
}

void Setup() {
    if (model == Model::None) return;
    uart_config_t c = {};
    c.baud_rate = baud;
    c.data_bits = UART_DATA_8_BITS;
    c.parity = UART_PARITY_DISABLE;
    c.stop_bits = UART_STOP_BITS_1;
    c.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
    c.source_clk = UART_SCLK_DEFAULT;
    if (uart_driver_install(RADAR_UART, 512, 0, 0, nullptr, 0) != ESP_OK || uart_param_config(RADAR_UART, &c) != ESP_OK
        || uart_set_pin(RADAR_UART, UART_PIN_NO_CHANGE, rxPin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE) != ESP_OK) {
        Log.println("Radar: UART setup failed (is the CSE7766 power monitor using it?)");
        model = Model::None;
        return;
    }
    lastFrame = millis();
}

void SerialReport() {
    Log.print("Radar:        ");
    if (model != Model::None)
        Log.printf("%s, rx pin %d, %d baud\n", MODEL_NAMES[(int)model], rxPin, baud);
    else
        Log.println("disabled");
}

void Loop() {
    if (model == Model::None) return;
    uint8_t buf[64];
    int n;
    while ((n = uart_read_bytes(RADAR_UART, buf, sizeof(buf), 0)) > 0)
        for (int i = 0; i < n; i++)
            if (feed(buf[i])) {
                haveFrame = true;
                lastFrame = millis();
            }

    unsigned long t = millis();
    if (t - lastFrame > STALE_MS) {
        if (!warnedStale) {
            Log.printf("Radar: no %s data on pin %d at %d baud\n", MODEL_NAMES[(int)model], rxPin, baud);
            warnedStale = true;
        }
        int count = cur.count;
        cur = {};
        cur.count = count;  // keep the shape: zeros, not missing values
    } else if (!haveFrame) {
        return;
    } else {
        warnedStale = false;
    }

    if (pubBinary("radar", cur.present, present)) GUI::Radar(cur.present);
    if (hasMotionState()) {
        pubBinary("radar_moving", cur.moving, moving);
        pubBinary("radar_still", cur.still, still);
    }

    if (online && cur.count && t - lastPublish >= PUBLISH_MS && (!sentValues || !sameValues(cur, sent))) {
        lastPublish = t;
        auto names = topics();
        for (int i = 0; i < cur.count; i++) pubTopic(names[i], std::to_string(cur.values[i]));
        sent = cur;
        sentValues = true;
    }
}

// Removed radars, or another model's entities, are cleaned up by Mqtt::PruneStaleDiscovery.
bool SendDiscovery() {
    if (model == Model::None) return true;
    if (!sendBinarySensorDiscovery("radar", "Radar", EC_NONE, "occupancy")) return false;
    if (hasMotionState()
        && !(sendBinarySensorDiscovery("radar_moving", "Radar Moving", EC_NONE, "motion")
             && sendBinarySensorDiscovery("radar_still", "Radar Still", EC_NONE, "occupancy")))
        return false;
    switch (model) {
        case Model::LD2410:
            return sensor("Radar Moving Distance", "distance", "cm")
                && sensor("Radar Still Distance", "distance", "cm")
                && sensor("Radar Detection Distance", "distance", "cm")
                && sensor("Radar Moving Energy", DEVICE_CLASS_NONE, "%", EC_DIAGNOSTIC)
                && sensor("Radar Still Energy", DEVICE_CLASS_NONE, "%", EC_DIAGNOSTIC);
        case Model::LD2450:
            if (!sensor("Radar Targets", DEVICE_CLASS_NONE, "")) return false;
            for (int i = 1; i <= Ld2450Reading::TARGETS; i++)
                if (!(sensor(Sprintf("Radar Target %d X", i).c_str(), "distance", "cm")
                      && sensor(Sprintf("Radar Target %d Y", i).c_str(), "distance", "cm")
                      && sensor(Sprintf("Radar Target %d Speed", i).c_str(), "speed", "mm/s")
                      && sensor(Sprintf("Radar Target %d Distance", i).c_str(), "distance", "cm")))
                    return false;
            return true;
        default:
            // Only filled in speed mode; a presence-mode module leaves them unknown.
            return sensor("Radar Distance", "distance", "cm")
                && sensor("Radar Speed", "speed", "mm/s")
                && sensor("Radar Energy", DEVICE_CLASS_NONE, "", EC_DIAGNOSTIC);
    }
}

// On every (re)connect: state published before MQTT came up was lost.
bool SendOnline() {
    if (model == Model::None || present < 0) return true;
    if (!pubTopic("radar", present ? "ON" : "OFF")) return false;
    if (hasMotionState() && !(pubTopic("radar_moving", moving ? "ON" : "OFF") && pubTopic("radar_still", still ? "ON" : "OFF")))
        return false;
    sentValues = false;  // and the numbers on the next Loop
    return true;
}
}  // namespace Radar
