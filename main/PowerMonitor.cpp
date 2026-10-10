#include "PowerMonitor.h"

#include <atomic>
#include <string>

#include "Settings.h"
#include "driver/gpio.h"
#include "globals.h"
#include "mqtt.h"
#include "string_utils.h"
#include "util.h"

// Defined in main.cpp: set once status=online has gone out.
extern bool online;

namespace PowerMonitor {
namespace {
// GPIO interrupts rather than PCNT: the ESP32-C3 in most plugs has no PCNT. Pulse rates are a
// few kHz at most (about 2.7 kHz at 3.5 kW on a BL0937).
std::atomic<uint32_t> cfPulses{0}, cf1Pulses{0};
void IRAM_ATTR onCf(void*) { cfPulses.fetch_add(1, std::memory_order_relaxed); }
void IRAM_ATTR onCf1(void*) { cf1Pulses.fetch_add(1, std::memory_order_relaxed); }

constexpr unsigned long WINDOW_MS = 2000;   // one CF1 reading per window
constexpr int WINDOWS_PER_MODE = 3;         // the first after a SEL switch is discarded (settling)
constexpr int WINDOWS_PER_PUBLISH = 5;      // 10 s
constexpr unsigned long SAVE_MS = 10 * 60 * 1000;  // energy to flash at most every 10 min
const char* const ENERGY_FILE = "/power_energy";

bool enabled = false;
bool bl0937 = true;
bool selInverted = false;
int cfPin = -1, cf1Pin = -1, selPin = -1;
float powerMult = 0, currentMult = 0, voltageMult = 0;  // per Hz (ESPHome's hlw8012 formulas)

bool currentMode = true;
int windowInMode = 0, windows = 0;
unsigned long windowStart = 0, lastSave = 0;
float power = 0, voltage = 0, current = 0;
double energyWs = 0, savedWs = 0;

// SEL picks what CF1 measures: high for current, as in ESPHome's hlw8012; sel_inverted is
// ESPHome's "inverted: true" on sel_pin (common on BL0937 plugs).
void applySel() {
    if (selPin >= 0) digitalWrite(selPin, currentMode != selInverted ? HIGH : LOW);
}

void attach(int pin, gpio_isr_t isr) {
    gpio_config_t c = {};
    c.pin_bit_mask = 1ULL << pin;
    c.mode = GPIO_MODE_INPUT;
    c.pull_up_en = GPIO_PULLUP_ENABLE;  // CF/CF1 are open-drain on some boards
    c.intr_type = GPIO_INTR_POSEDGE;
    gpio_config(&c);
    gpio_install_isr_service(0);  // ESP_ERR_INVALID_STATE if already installed; fine
    gpio_isr_handler_add((gpio_num_t)pin, isr, nullptr);
}

bool pubValue(const char* name, double v, int decimals) {
    return pub((roomsTopic + "/" + name).c_str(), 0, true, toStr(v, decimals).c_str());
}
}  // namespace

void ConnectToWifi(bool updating) {
    std::string text = Settings::json("power", "{}", "Power monitor");
    DynamicJsonDocument cfg(text.size() * 2 + 128);
    enabled = false;
    if (deserializeJson(cfg, text) || !cfg.is<JsonObject>()) return;
    std::string model = cfg["model"] | "";
    cfPin = cfg["cf"] | -1;
    cf1Pin = cfg["cf1"] | -1;
    selPin = cfg["sel"] | -1;
    selInverted = cfg["sel_inverted"] | false;
    float divider = cfg["voltage_divider"] | 2351.0f;
    float resistor = cfg["current_resistor"] | 0.001f;
    if ((model != "bl0937" && model != "hlw8012") || cfPin < 0 || resistor <= 0) return;
    bl0937 = model == "bl0937";
    if (bl0937) {
        const float vref = 1.218f;
        powerMult = vref * vref * divider / resistor / 1721506.0f;
        currentMult = vref / resistor / 94638.0f;
        voltageMult = vref * divider / 15397.0f;
    } else {
        const float vref = 2.43f, fosc = 3579000.0f;
        powerMult = vref * vref * divider / resistor * 64.0f / 24.0f / fosc;
        currentMult = vref / resistor * 512.0f / 24.0f / fosc;
        voltageMult = vref * divider * 256.0f / fosc;
    }
    energyWs = savedWs = toFloat(Settings::slurp(ENERGY_FILE)) * 3600000.0;
    enabled = true;
}

void Setup() {
    if (!enabled) return;
    if (selPin >= 0) {
        applySel();
        pinMode(selPin, OUTPUT);
    }
    attach(cfPin, onCf);
    if (cf1Pin >= 0) attach(cf1Pin, onCf1);
    windowStart = lastSave = millis();
}

void Loop() {
    if (!enabled) return;
    unsigned long now = millis();
    unsigned long dt = now - windowStart;
    if (dt < WINDOW_MS) return;
    windowStart = now;
    double secs = dt / 1000.0;

    uint32_t cf = cfPulses.exchange(0), cf1 = cf1Pulses.exchange(0);
    power = cf / secs * powerMult;
    energyWs += cf * powerMult;
    if (windowInMode++ > 0) {  // skip the settling window
        if (currentMode)
            current = cf1 / secs * currentMult;
        else
            voltage = cf1 / secs * voltageMult;
    }
    if (selPin >= 0 && windowInMode >= WINDOWS_PER_MODE) {
        currentMode = !currentMode;
        windowInMode = 0;
        applySel();
    }

    if (++windows % WINDOWS_PER_PUBLISH == 0 && online) {
        pubValue("power", power, 1);
        if (cf1Pin >= 0) {
            pubValue("voltage", voltage, 1);
            pubValue("current", current, 3);
        }
        pubValue("energy", energyWs / 3600000.0, 3);
    }
    if (now - lastSave >= SAVE_MS && energyWs != savedWs) {
        lastSave = now;
        savedWs = energyWs;
        Settings::spurt(ENERGY_FILE, toStr(energyWs / 3600000.0, 4));
    }
}

bool SendDiscovery() {
    if (!enabled)
        return sendDeleteDiscovery("sensor", "Power") && sendDeleteDiscovery("sensor", "Voltage")
            && sendDeleteDiscovery("sensor", "Current") && sendDeleteDiscovery("sensor", "Energy");
    bool cf1 = cf1Pin >= 0;
    return sendSensorDiscovery("Power", EC_NONE, "power", "W")
        && (cf1 ? sendSensorDiscovery("Voltage", EC_NONE, "voltage", "V") : sendDeleteDiscovery("sensor", "Voltage"))
        && (cf1 ? sendSensorDiscovery("Current", EC_NONE, "current", "A") : sendDeleteDiscovery("sensor", "Current"))
        && sendSensorDiscovery("Energy", EC_NONE, "energy", "kWh");
}
}  // namespace PowerMonitor
