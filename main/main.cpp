#define VAR_DECLS
#include "main.h"

void heapCapsAllocFailedHook(size_t requestedSize, uint32_t caps, const char *functionName) {
    ESP_EARLY_LOGE("heap", "%s failed to allocate %lu bytes with 0x%lX capabilities", functionName, static_cast<unsigned long>(requestedSize), static_cast<unsigned long>(caps));
}

/**
 * @brief Publish device telemetry and perform online/discovery announcements.
 *
 * Attempts to send status and discovery payloads when the device is not marked online
 * or discovery has not yet been announced, then builds and publishes a telemetry
 * JSON document containing device state, diagnostics, and aggregated counters.
 * Successful status/discovery sends update internal flags such as `online` and
 * `sentDiscovery`; failed telemetry publishes increment internal failure counters.
 *
 * @param totalSeen Total advertisement packets observed since the last report.
 * @param totalFpSeen Total distinct fingerprints seen since the last report.
 * @param totalFpQueried Total fingerprints queried (e.g., looked up) since the last report.
 * @param totalFpReported Total fingerprint reports published since the last report.
 * @param count Current count value (included only when a count identifier is configured).
 * @return `true` if the telemetry document was published successfully, `false` otherwise.
 * `false` is also returned when telemetry publishing is disabled or the function is rate-limited.
 */
bool sendTelemetry(unsigned int totalSeen, unsigned int totalFpSeen, unsigned int totalFpQueried, unsigned int totalFpReported, unsigned int count, unsigned int fingerprintCount) {
    if (!online) {
        if (
            pub(statusTopic.c_str(), 0, true, "online")
            && pub((roomsTopic + "/name").c_str(), 0, true, room.c_str())
            && pub((roomsTopic + "/max_distance").c_str(), 0, true, toStr(BleFingerprintCollection::maxDistance).c_str())
            && pub((roomsTopic + "/absorption").c_str(), 0, true, toStr(BleFingerprintCollection::absorption).c_str())
            && pub((roomsTopic + "/tx_ref_rssi").c_str(), 0, true, toStr(BleFingerprintCollection::txRefRssi).c_str())
            && pub((roomsTopic + "/rx_adj_rssi").c_str(), 0, true, toStr(BleFingerprintCollection::rxAdjRssi).c_str())
            && pub((roomsTopic + "/query").c_str(), 0, true, BleFingerprintCollection::query.c_str())
            && pub((roomsTopic + "/include").c_str(), 0, true, BleFingerprintCollection::include.c_str())
            && pub((roomsTopic + "/exclude").c_str(), 0, true, BleFingerprintCollection::exclude.c_str())
            && pub((roomsTopic + "/known_macs").c_str(), 0, true, BleFingerprintCollection::knownMacs.c_str())
            && pub((roomsTopic + "/known_irks").c_str(), 0, true, BleFingerprintCollection::knownIrks.c_str())
            && pub((roomsTopic + "/count_ids").c_str(), 0, true, BleFingerprintCollection::countIds.c_str())
            && Updater::SendOnline()
            && Motion::SendOnline()
            && Switch::SendOnline()
            && Button::SendOnline()
            && GUI::SendOnline()
        ) {
            online = true;
            reconnectTries = 0;
        } else {
            Log.println("Error sending status=online");
        }
    }

    if (discovery && !sentDiscovery) {
        if (sendConnectivityDiscovery()
            && sendTeleSensorDiscovery("Uptime", EC_DIAGNOSTIC, "{{ value_json.uptime }}", DEVICE_CLASS_NONE, "s")
            && sendTeleSensorDiscovery("Free Mem", EC_DIAGNOSTIC, "{{ value_json.freeHeap }}", DEVICE_CLASS_NONE, "bytes")
            && (BleFingerprintCollection::countIds.empty() ? sendDeleteDiscovery("sensor", "Count") : sendTeleSensorDiscovery("Count", EC_NONE, "{{ value_json.count }}"))
            && sendButtonDiscovery("Restart", EC_DIAGNOSTIC)
            && sendNumberDiscovery("Max Distance", EC_CONFIG)
            && sendNumberDiscovery("Absorption", EC_CONFIG)

            && Updater::SendDiscovery()
            && GUI::SendDiscovery()
            && Motion::SendDiscovery()
            && Switch::SendDiscovery()
            && Button::SendDiscovery()
            && Enrollment::SendDiscovery()
            && Battery::SendDiscovery()
            && CAN::SendDiscovery()
#ifdef SENSORS
            && DHT::SendDiscovery()
            && AHTX0::SendDiscovery()
            && BH1750::SendDiscovery()
            && BME280::SendDiscovery()
            && BMP180::SendDiscovery()
            && BMP280::SendDiscovery()
            && SHT::SendDiscovery()
            && TSL2561::SendDiscovery()
            && SensirionSGP30::SendDiscovery()
            && SensirionSCD4x::SendDiscovery()
            && HX711::SendDiscovery()
            && DS18B20::SendDiscovery()
#endif
        ) {
            sentDiscovery = true;
        } else {
            Log.println("Error sending discovery");
        }
    }

    if (!publishTele)
        return false;
    auto now = millis();

    if (now - lastTeleMillis < 15000)
        return false;

    lastTeleMillis = now;

    doc.clear();
    doc["ip"] = localIp;
    doc["uptime"] = esp_timer_get_time() / 1000000;
#ifdef FIRMWARE
    doc["firm"] = FIRMWARE;
#endif
    doc["rssi"] = Network::rssi();
    Battery::SendTelemetry();

#ifdef VERSION
    doc["ver"] = VERSION;
#else
    doc["ver"] = std::string(esp_app_get_description()->version) + "-" + getBuildTimestamp();
#endif

    if (!BleFingerprintCollection::countIds.empty())
        doc["count"] = count;
    if (totalSeen > 0)
        doc["adverts"] = totalSeen;
    if (totalFpSeen > 0)
        doc["seen"] = totalFpSeen;
    if (totalFpQueried > 0)
        doc["queried"] = totalFpQueried;
    if (totalFpReported > 0)
        doc["reported"] = totalFpReported;
    if (reportFailed > 0)
        doc["failed"] = reportFailed;
    if (teleFails > 0)
        doc["teleFails"] = teleFails;
    if (reconnectTries > 0)
        doc["reconnectTries"] = reconnectTries;
    auto maxHeap = maxAllocHeap();
    auto freeHeap = ::freeHeap();
    doc["freeHeap"] = freeHeap;
    doc["maxHeap"] = maxHeap;
    doc["fingerprints"] = fingerprintCount;
    doc["scanStack"] = uxTaskGetStackHighWaterMark(scanTaskHandle);
    doc["loopStack"] = uxTaskGetStackHighWaterMark(nullptr);
    doc["bleStack"] = bleStack;

    if (pub(teleTopic.c_str(), 0, false, doc)) return true;

    teleFails++;
    log_e("Error after 10 tries sending telemetry (%d times since boot)", teleFails);
    return false;
}

/**
 * @brief Configure network settings, initialize network-connected subsystems, and establish connectivity.
 *
 * Reads runtime settings (Wi‑Fi, Ethernet type, MQTT, discovery/publication flags, timeouts, hostname, room),
 * initializes and connects subsystem components that require network access, registers wait-loop callbacks for
 * captive portal and connection progress, and attempts to bring up Ethernet or Wi‑Fi. Logs network and device
 * information and prepares MQTT/topic strings for later use.
 *
 * On unrecoverable connection failure this function will reboot the device (calls ESP.restart()). The captive
 * portal wait callback can also trigger a restart if the portal timeout elapses.
 */
void setupNetwork() {
    Log.println("Setup network");
    GUI::Connected(false, false);

    room = Settings::string("room", ESPMAC, "Room");
    Settings::string("wifi-ssid", "", "WiFi SSID");
    Settings::pstring("wifi-password", "", "WiFi Password");
    auto wifiTimeout = Settings::integer("wifi_timeout", DEFAULT_WIFI_TIMEOUT, "Seconds to wait for WiFi before captive portal (-1 = forever)");
    auto portalTimeout = 1000UL * Settings::integer("portal_timeout", DEFAULT_PORTAL_TIMEOUT, "Seconds to wait in captive portal before rebooting");
    if (Network::supportsEthernet()) {
        std::vector<std::string> ethernetTypes = {"None", "WT32-ETH01", "ESP32-POE", "WESP32", "QuinLED-ESP32", "TwilightLord-ESP32", "ESP32Deux", "KIT-VE", "LilyGO-T-ETH-POE", "GL-inet GL-S10 v2.1 Ethernet", "EST-PoE-32", "LilyGO-T-ETH-Lite (RTL8201)", "ESP32-POE_A1", "WESP32 Rev7+ (RTL8201)"};
        ethernetType = Settings::dropdown("eth", ethernetTypes, 0, "Ethernet Type");
    } else {
        ethernetType = 0;
    }

    mqttHost = Settings::string("mqtt_host", DEFAULT_MQTT_HOST, "Server");
    mqttPort = Settings::integer("mqtt_port", DEFAULT_MQTT_PORT, "Port");
    mqttUser = Settings::pstring("mqtt_user", DEFAULT_MQTT_USER, "Username");
    mqttPass = Settings::pstring("mqtt_pass", DEFAULT_MQTT_PASSWORD, "Password");
    discovery = Settings::checkbox("discovery", true, "Send to discovery topic");
    homeAssistantDiscoveryPrefix = Settings::string("discovery_prefix", DEFAULT_HA_DISCOVERY_PREFIX, "Home Assistant discovery topic prefix");
    publishTele = Settings::checkbox("pub_tele", true, "Send to telemetry topic");
    publishDevices = Settings::checkbox("pub_devices", true, "Send to devices topic");

    bool updating = Settings::exists("/update");

    Updater::ConnectToWifi(updating);

    // Settings registered after this belong to /wifi/extras
    Settings::markExtra();
    BleFingerprintCollection::ConnectToWifi(updating);

    // Settings registered after this belong to /wifi/hardware
    Settings::markEndpoint("hardware");
    GUI::ConnectToWifi(updating);

    Motion::ConnectToWifi(updating);
    Switch::ConnectToWifi(updating);
    Button::ConnectToWifi(updating);

#ifdef SENSORS
    DHT::ConnectToWifi(updating);
    I2C::ConnectToWifi(updating);

    AHTX0::ConnectToWifi(updating);
    BH1750::ConnectToWifi(updating);
    BME280::ConnectToWifi(updating);
    BMP180::ConnectToWifi(updating);
    BMP280::ConnectToWifi(updating);
    SHT::ConnectToWifi(updating);
    TSL2561::ConnectToWifi(updating);
    SensirionSGP30::ConnectToWifi(updating);
    SensirionSCD4x::ConnectToWifi(updating);
    HX711::ConnectToWifi(updating);
    DS18B20::ConnectToWifi(updating);
#endif

    static unsigned int connectProgress = 0;
    Network::onWaitLoop = []() {
        GUI::Wifi(connectProgress++);
        SerialImprov::Loop(true);
        return 50;
    };
    static unsigned int portalProgress = 0;
    static unsigned long portalTimeoutMs = portalTimeout;
    Network::onPortalWaitLoop = []() {
        GUI::Portal(portalProgress++);
        SerialImprov::Loop(false);
        if (millis() > portalTimeoutMs) esp_restart();
        return 50;
    };
    std::string hostname = "espresense-" + kebabify(room);

    if (!Network::connect(ethernetType, 20, wifiTimeout, hostname))
        esp_restart();

    GUI::Connected(true, false);

#ifdef FIRMWARE
    Log.println(std::string("Firmware:     ") + FIRMWARE);
#endif
#ifdef VERSION
    Log.println(std::string("Version:      ") + VERSION);
#endif
    Log.printf("WiFi BSSID:   %s (channel=%d rssi=%d)\r\n", Network::bssid().c_str(), Network::channel(), Network::rssi());
    Log.print("IP address:   ");
    Log.println(Network::localIP());
    Log.print("DNS address:  ");
    Log.println(Network::dnsIP());
    Log.print("Hostname:     ");
    Log.println(Network::hostname());
    Log.print("Room:         ");
    Log.println(room);
    Log.printf("Mqtt server:  %s:%d\r\n", mqttHost.c_str(), mqttPort);
    Log.printf("Max Distance: %.2f\r\n", BleFingerprintCollection::maxDistance);
    GUI::SerialReport();
    Motion::SerialReport();
    Switch::SerialReport();
    Button::SerialReport();
#ifdef SENSORS
    I2C::SerialReport();
    DHT::SerialReport();
    AHTX0::SerialReport();
    BH1750::SerialReport();
    BME280::SerialReport();
    BMP180::SerialReport();
    BMP280::SerialReport();
    SHT::SerialReport();
    TSL2561::SerialReport();
    SensirionSGP30::SerialReport();
    SensirionSCD4x::SerialReport();
    HX711::SerialReport();
    DS18B20::SerialReport();
#endif
    Log.print("Query:        ");
    Log.println(BleFingerprintCollection::query);
    Log.print("Include:      ");
    Log.println(BleFingerprintCollection::include);
    Log.print("Exclude:      ");
    Log.println(BleFingerprintCollection::exclude);
    Log.print("Known Macs:   ");
    Log.println(BleFingerprintCollection::knownMacs);
    Log.print("Count Ids:    ");
    Log.println(BleFingerprintCollection::countIds);

    localIp = Network::localIP();
    id = slugify(room);
    roomsTopic = CHANNEL + std::string("/rooms/") + id;
    statusTopic = roomsTopic + "/status";
    teleTopic = roomsTopic + "/telemetry";
    setTopic = roomsTopic + "/+/set";
    configTopic = CHANNEL + std::string("/settings/+/config");
    HttpWebServer::Init();
    Updater::MarkOtaSuccess();
}

void onMqttMessage(const char *topic, const char *payload) {
    std::string const top = topic;
    std::string pay = payload;

    auto setPos = top.rfind("/set");
    auto configPos = top.rfind("/config");
    if (configPos != std::string::npos && configPos > 1) {
        auto idPos = top.rfind('/', configPos - 1);
        if (idPos == std::string::npos) goto skip;
        auto id = top.substr(idPos + 1, configPos - idPos - 1);
        Log.printf("%u Config | %s to %s\r\n", (unsigned)xPortGetCoreID(), id.c_str(), pay.c_str());
        BleFingerprintCollection::Config(id, pay);
    } else if (setPos != std::string::npos && setPos > 1) {
        auto commandPos = top.rfind('/', setPos - 1);
        if (commandPos == std::string::npos) goto skip;
        auto command = top.substr(commandPos + 1, setPos - commandPos - 1);
        Log.printf("%u Set    | %s to %s\r\n", (unsigned)xPortGetCoreID(), command.c_str(), pay.c_str());

        bool changed = false;
        if (command == "restart" || command == "reboot")
            esp_restart();
        else if (command == "wifi-ssid" || command == "wifi-password")
            spurt("/" + command, pay);
        else if (command == "name")
            spurt("/room", pay.empty() ? ESPMAC : pay);
        else if (GUI::Command(command, pay))
            ;
        else if (Motion::Command(command, pay))
            ;
        else if (BleFingerprintCollection::Command(command, pay))
            changed = true;
        else if (Enrollment::Command(command, pay))
            changed = true;
        else if (Updater::Command(command, pay))
            changed = true;
        else if (Switch::Command(command, pay))
            changed = true;
        else if (Button::Command(command, pay))
            changed = true;
        if (changed) online = false;
    } else {
    skip:
        Log.printf("%u Unknown| %s to %s\r\n", (unsigned)xPortGetCoreID(), topic, payload);
    }
}

// Every 3 s: bring the network back if it dropped, kick MQTT, and give up (restart) after 50
// tries. esp_mqtt reconnects on its own once the network is up; this is the backstop.
void reconnect(TimerHandle_t) {
    if (Network::isOnline() && Mqtt::connected()) {
        reconnectTries = 0;
        return;
    }
    Log.printf("%u Reconnect timer\r\n", (unsigned)xPortGetCoreID());
    if (reconnectTries++ > 50) {
        Log.println("Too many reconnect attempts; Restarting");
        esp_restart();
    }
    if (!Network::isOnline()) {
        Log.printf("%u Reconnecting to Network...\r\n", (unsigned)xPortGetCoreID());
        if (!Network::connect(ethernetType, 2, 40, Network::hostname(), false))
            esp_restart();
    }
    Log.printf("%u Reconnecting to MQTT...\r\n", (unsigned)xPortGetCoreID());
    Mqtt::reconnect();
}

void connectToMqtt() {
    Mqtt::Setup(mqttHost, mqttPort, mqttUser, mqttPass, Network::hostname(), statusTopic);
    reconnectTimer = xTimerCreate("reconnectionTimer", pdMS_TO_TICKS(3000), pdTRUE, (void *)nullptr, reconnect);
    xTimerStart(reconnectTimer, 0);
}

bool reportBuffer(BleFingerprint *f) {
    if (!Mqtt::connected()) return false;
    auto report = f->getReport();
    std::string const topic = Sprintf(CHANNEL "/devices/%s/%s/%s", f->getId().c_str(), id.c_str(), report.getId().c_str());
    return pub(topic.c_str(), 0, false, report.getPayload().c_str());
}

bool reportDevice(BleFingerprint *f) {
    doc.clear();
    JsonObject obj = doc.to<JsonObject>();
    if (!f->report(&obj))
        return false;

    std::string const devicesTopic = Sprintf(CHANNEL "/devices/%s/%s", f->getId().c_str(), id.c_str());
    if (pub(devicesTopic.c_str(), 0, false, doc))
        return true;

    reportFailed++;
    return false;
}

unsigned int totalSeen = 0;
unsigned int totalFpSeen = 0;
unsigned int totalFpQueried = 0;
unsigned int totalFpReported = 0;

void reportSetup() {
    connectToMqtt();
}

void reportLoop() {
    if (!Mqtt::connected()) return;

    yield();
    auto fingerprintCount = BleFingerprintCollection::Size();

    unsigned int count = 0;
    size_t cursor = 0;
    while (auto lease = BleFingerprintCollection::AcquireNext(cursor, false)) {
        if (lease.fingerprint->shouldCount())
            count++;
        BleFingerprintCollection::Release(lease);
    }

    GUI::Count(count);

    yield();
    sendTelemetry(totalSeen, totalFpSeen, totalFpQueried, totalFpReported, count, fingerprintCount);
    yield();

    auto reported = 0;
    cursor = 0;
    while (auto lease = BleFingerprintCollection::AcquireNext(cursor, false)) {
        auto *f = lease.fingerprint;
        auto seen = f->getSeenCount();
        if (seen) {
            totalSeen += seen;
            totalFpSeen++;
        }

        if (f->hasReport()) {
            if (reportBuffer(f))
                f->clearReport();
        }
        if (reportDevice(f)) {
            totalFpReported++;
            reported++;
        }
        BleFingerprintCollection::Release(lease);
        yield();
    }
}

void onAdvert(const Ble::Advert &advert) {
    bleStack = uxTaskGetStackHighWaterMark(nullptr);
    BleFingerprintCollection::Seen(&advert);
}

void scanTask(void *parameter) {
    Ble::Init("ESPresense", onAdvert);
    Ble::DeleteAllBonds();
    Enrollment::Setup();

    if (!Ble::StartScan())
        log_e("Error starting continuous ble scan");

    while (true) {
        size_t cursor = 0;
        while (auto lease = BleFingerprintCollection::AcquireNext(cursor, false)) {
            if (lease.fingerprint->query())
                totalFpQueried++;
            BleFingerprintCollection::Release(lease);
        }

        Enrollment::Loop();

        if (!Ble::IsScanning()) {
            if (!Ble::StartScan())
                log_e("Error re-starting continuous ble scan");
            delay(3000);  // If we stopped scanning, don't query for 3 seconds in order for us to catch any missed broadcasts
        } else {
            delay(100);
        }
    }
}

/**
 * @brief Initialize hardware, peripherals, services, and background tasks required by the program.
 *
 * Configures serial logging and ESP log levels, registers a heap allocation-failure callback, initializes
 * platform-specific power/hardware (when enabled), filesystem, network, OTA/updater, GUI and application
 * subsystems (motion, switches, buttons, battery, CAN, NTP, optional sensors), enables TCP remote logging,
 * starts the BLE scan task, and performs MQTT/reporting setup.
 */
void setup() {
#ifdef LOG_LEVEL_DEBUG
    esp_log_level_set("*", ESP_LOG_DEBUG);
#else
    esp_log_level_set("*", ESP_LOG_ERROR);
#endif
    Log.printf("Pre-Setup Free Mem: %lu\r\n", static_cast<unsigned long>(freeHeap()));
    heap_caps_register_failed_alloc_callback(heapCapsAllocFailedHook);

    SerialImprov::Setup();
    Settings::begin();
    Network::Setup();
#if M5STICK
    AXP192::Setup();
#endif

    GUI::Setup(true);
    BleFingerprintCollection::Setup();
    setupNetwork();
    Log.enableTcp(6053);
    Updater::Setup();
    GUI::Setup(false);
    Motion::Setup();
    Switch::Setup();
    Button::Setup();
    Battery::Setup();
    CAN::Setup();
    NTP::Setup();
#ifdef SENSORS
    DHT::Setup();
    I2C::Setup();
    AHTX0::Setup();
    BH1750::Setup();
    BME280::Setup();
    BMP180::Setup();
    BMP280::Setup();
    SHT::Setup();
    TSL2561::Setup();
    SensirionSGP30::Setup();
    SensirionSCD4x::Setup();
    HX711::Setup();
    DS18B20::Setup();
#endif
    xTaskCreatePinnedToCore(scanTask, "scanTask", SCAN_TASK_STACK_SIZE, nullptr, 1, &scanTaskHandle, CONFIG_BT_NIMBLE_PINNED_TO_CORE);
    reportSetup();
    Log.printf("Post-Setup Free Mem: %lu\r\n", static_cast<unsigned long>(freeHeap()));
    Log.println();
}

/**
 * @brief Executes the main periodic processing and dispatches per-subsystem loop handlers.
 *
 * Runs the primary report processing and repeatedly invokes each subsystem's Loop method.
 * Every 5 seconds it performs a slow maintenance check that reads available heap memory,
 * logs a low-memory warning when free memory is less than 20,000 bytes, and runs the updater
 * loop when free memory exceeds 70,000 bytes.
 *
 * Subsystems invoked each iteration include GUI, Motion, Switch, Button, HTTP server,
 * SerialImprov, NTP, and (conditionally) AXP192 and various sensor modules.
 */
void loop() {
    reportLoop();
    static unsigned long lastSlowLoop = 0;
    if (millis() - lastSlowLoop > 5000) {
        lastSlowLoop = millis();
        auto freeHeap = ::freeHeap();
        auto maxAlloc = maxAllocHeap();
        // maxAlloc as well as freeHeap: #2309's node logged "Low memory: ~20000 bytes free"
        // for twenty minutes without the one number that explained the crash — its largest
        // free block was already under the 2312 byte request that kept failing.
        if (freeHeap < 20000) Log.printf("Low memory: %lu bytes free, largest block %lu\r\n", static_cast<unsigned long>(freeHeap), static_cast<unsigned long>(maxAlloc));
        if (freeHeap > 70000) Updater::Loop();

        // ponytail: watchdog, not a leak fix. A heap-starved node limps forever (mqtt and
        // telemetry both fail, nothing recovers), so reboot after 60s stuck below the
        // threshold mqtt already refuses to publish under. Same escape hatch as the stuck
        // BLE controller restart in BleFingerprintCollection::CleanupOldFingerprints().
        static uint8_t lowHeapPasses = 0;
        switch (heapWatchdogTick(freeHeap, maxAlloc, MQTT_MIN_FREE_MEMORY, MIN_MAX_ALLOC_HEAP, lowHeapPasses)) {
            case HeapTrip::FreeHeap:
                Log.printf("Out of memory for 60s (%lu bytes free), restarting\r\n", static_cast<unsigned long>(freeHeap));
                esp_restart();
                break;
            case HeapTrip::MaxAlloc:
                Log.printf("Heap too fragmented for 60s (largest block %lu, %lu bytes free), restarting\r\n", static_cast<unsigned long>(maxAlloc), static_cast<unsigned long>(freeHeap));
                esp_restart();
                break;
            case HeapTrip::None:
                break;
        }
    }
    GUI::Loop();
    Motion::Loop();
    Switch::Loop();
    Button::Loop();
    HttpWebServer::Loop();
    SerialImprov::Loop(false);
    NTP::Loop();
#if M5STICK
    AXP192::Loop();
#endif
#ifdef SENSORS
    DHT::Loop();
    AHTX0::Loop();
    BH1750::Loop();
    BME280::Loop();
    BMP180::Loop();
    BMP280::Loop();
    SHT::Loop();
    TSL2561::Loop();
    SensirionSGP30::Loop();
    SensirionSCD4x::Loop();
    HX711::Loop();
    DS18B20::Loop();
#endif

}

// The Arduino loopTask, minus Arduino: setup() once, then loop() forever, yielding each pass so
// the idle task (and its watchdog) get their turn.
static void loopTask(void *) {
    setup();
    while (true) {
        loop();
        vTaskDelay(1);
    }
}

extern "C" void app_main(void) {
    xTaskCreatePinnedToCore(loopTask, "loopTask", LOOP_TASK_STACK_SIZE, nullptr, 1, nullptr, LOOP_TASK_CORE);
}
