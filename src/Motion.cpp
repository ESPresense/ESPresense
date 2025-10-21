#include "Motion.h"

#include <AsyncMqttClient.h>
#include <HeadlessWiFiSettings.h>

#include "GUI.h"
#include "defaults.h"
#include "globals.h"
#include "mqtt.h"
#include "string_utils.h"

namespace Motion {
int8_t lastMotionValue = -1;

int8_t pirType, pirPin, pirDetected = -1;
float pirTimeout = 0;
int8_t lastPirValue = -1;
unsigned long lastPirMilli = 0;

int8_t radarType, radarPin, radarDetected = -1;
float radarTimeout = 0;
int8_t lastRadarValue = -1;
unsigned long lastRadarMilli = 0;
bool online;

void Setup() {
    std::vector<uint8_t> pinTypes = {INPUT_PULLUP, INPUT_PULLUP, INPUT_PULLDOWN, INPUT_PULLDOWN, INPUT, INPUT};
    if (pirPin >= 0) pinMode(pirPin, pinTypes[pirType]);
    if (radarPin >= 0) pinMode(radarPin, pinTypes[radarType]);
}

/**
 * @brief Presents Wi-Fi setup UI to configure PIR and Radar input settings.
 *
 * Displays configuration controls for PIR and Radar pin type, pin number (use -1 to disable),
 * and debounce timeout (seconds). Stores the chosen values into the module's global state so
 * subsequent setup and runtime loops use the configured pins and timeouts.
 *
 * Globals modified:
 * - pirType, pirPin, pirTimeout, pirDetected
 * - radarType, radarPin, radarTimeout, radarDetected
 *
 * The pin type control offers the available pull/floating options and the timeout control
 * defaults to the module's default debounce timeout.
 */
void ConnectToWifi() {
    std::vector<String> pinTypes = {"Pullup", "Pullup Inverted", "Pulldown", "Pulldown Inverted", "Floating", "Floating Inverted"};
    pirType = HeadlessWiFiSettings.dropdown("pir_type", pinTypes, 0, "PIR motion pin type");
    pirPin = HeadlessWiFiSettings.integer("pir_pin", -1, "PIR motion pin (-1 for disable)");
    pirTimeout = HeadlessWiFiSettings.floating("pir_timeout", 0, 300, DEFAULT_DEBOUNCE_TIMEOUT, "PIR motion timeout (in seconds)");
    pirDetected = pirType & 0x01 ? LOW : HIGH;

    radarType = HeadlessWiFiSettings.dropdown("radar_type", pinTypes, 0, "Radar motion pin type");
    radarPin = HeadlessWiFiSettings.integer("radar_pin", -1, "Radar motion pin (-1 for disable)");
    radarTimeout = HeadlessWiFiSettings.floating("radar_timeout", 0, 300, DEFAULT_DEBOUNCE_TIMEOUT, "Radar motion timeout (in seconds)");
    radarDetected = radarType & 0x01 ? LOW : HIGH;
}

/**
 * @brief Reports whether the PIR and Radar sensors are enabled.
 *
 * Prints the enabled/disabled status of the PIR and Radar sensors to the logging output.
 */
void SerialReport() {
    Log.print("PIR Sensor:   ");
    Log.println(pirPin >= 0 ? "enabled" : "disabled");
    Log.print("Radar Sensor: ");
    Log.println(radarPin >= 0 ? "enabled" : "disabled");
}

/**
 * @brief Monitors the PIR input and publishes PIR on/off events when the sensor state changes.
 *
 * If the PIR pin is disabled, the function returns immediately. When motion is detected it updates
 * the last detection timestamp and maintains a HIGH state for the configured timeout period.
 * On a change of PIR state this publishes "ON" or "OFF" to the room's "/pir" topic and updates
 * the internal lastPirValue.
 */
static void pirLoop() {
    if (pirPin < 0) return;
    bool const detected = digitalRead(pirPin) == pirDetected;
    if (detected) lastPirMilli = millis();
    unsigned long const since = millis() - lastPirMilli;
    int const pirValue = (detected || since < (pirTimeout * 1000)) ? HIGH : LOW;

    if (lastPirValue == pirValue) return;
    pub((roomsTopic + "/pir").c_str(), 0, true, pirValue == HIGH ? "ON" : "OFF");
    lastPirValue = pirValue;
}

static void radarLoop() {
    if (radarPin < 0) return;
    bool const detected = digitalRead(radarPin) == radarDetected;
    if (detected) lastRadarMilli = millis();
    unsigned long const since = millis() - lastRadarMilli;
    int const radarValue = (detected || since < (radarTimeout * 1000)) ? HIGH : LOW;

    if (lastRadarValue == radarValue) return;
    pub((roomsTopic + "/radar").c_str(), 0, true, radarValue == HIGH ? "ON" : "OFF");
    lastRadarValue = radarValue;
}

void Loop() {
    pirLoop();
    radarLoop();
    int const motionValue = (lastRadarValue == HIGH || lastPirValue == HIGH) ? HIGH : LOW;
    if (lastMotionValue == motionValue) return;
    GUI::Motion(lastPirValue == HIGH, lastRadarValue == HIGH);
    pub((roomsTopic + "/motion").c_str(), 0, true, motionValue == HIGH ? "ON" : "OFF");
    lastMotionValue = motionValue;
}

bool SendDiscovery() {
    if (pirPin < 0 && radarPin < 0) return true;

    if (pirPin >= 0)
        if (!sendNumberDiscovery("Pir Timeout", EC_CONFIG)) return false;
    if (radarPin >= 0)
        if (!sendNumberDiscovery("Radar Timeout", EC_CONFIG)) return false;
    return sendBinarySensorDiscovery("Motion", EC_NONE, "motion");
}

bool Command(String& command, String& pay) {
    if (command == "pir_timeout") {
        pirTimeout = pay.toInt();
        spurt("/pir_timeout", pay);
    } else if (command == "radar_timeout") {
        radarTimeout = pay.toInt();
        spurt("/radar_timeout", pay);
    } else
        return false;
    return true;
}

bool SendOnline() {
    if (online) return true;
    if (!pub((roomsTopic + "/pir_timeout").c_str(), 0, true, String(pirTimeout).c_str())) return false;
    if (!pub((roomsTopic + "/radar_timeout").c_str(), 0, true, String(radarTimeout).c_str())) return false;
    online = true;
    return true;
}
}  // namespace Motion