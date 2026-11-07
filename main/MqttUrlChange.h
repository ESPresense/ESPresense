#pragma once
// Moving a node to another broker over MQTT: espresense/rooms/<room>/mqtt-url/set (#2647).
//
// A bad URL would cut the node off from the only channel that could fix it, so the change is a
// trial. apply() keeps the old server and the trial URL next to mqtt_host, then the node
// restarts. On boot the trial is live while mqtt_host still equals the trial URL. The first MQTT
// connection commits it. If the network is up but MQTT doesn't connect within trialMs, the
// node puts the old server back, keeps the failed URL to report, and restarts.
//
// Every step writes in an order that a power cut can't turn into a wrong broker: mqtt_host
// changes last on apply and first on rollback, and a trial whose URL isn't mqtt_host any more
// (a rollback that got that far, or a fix in the web UI) is dropped.
// Pure, so host tests can run it.
#include <string>

#include "MqttUrl.h"

namespace MqttUrlChange {

constexpr const char* kHost = "mqtt_host";
constexpr const char* kPrev = "mqtt_host_prev";    // the server to go back to
constexpr const char* kTrial = "mqtt_host_trial";  // the URL on trial
constexpr const char* kFailed = "mqtt_url_failed"; // a URL that didn't connect, to report
constexpr unsigned long trialMs = 60000;

// Why a set is refused, or "" to go ahead. Only this node's own topic, never a retained message,
// so one stray publish can't take the fleet offline.
inline std::string refuse(const std::string& topic, const std::string& ownTopic, bool retained,
                          const std::string& url, const std::string& currentUri) {
    if (topic != ownTopic) return "only on this node's own topic, not a wildcard";
    if (retained) return "not from a retained message";
    if (!MqttUrl::isUrl(url)) return "needs a URL, like mqtts://broker";
    if (!MqttUrl::supported(url)) return "only mqtt:// and mqtts:// are supported";
    if (url.size() <= MqttUrl::scheme(url).size() + 3) return "no host";
    if (url == currentUri) return "already the server";
    return "";
}

template <typename Put>
bool apply(Put put, const std::string& currentUri, const std::string& url) {
    return put(kPrev, currentUri) && put(kTrial, url) && put(kHost, url);
}

enum class Boot { None, Trial, Stale };

template <typename Get>
Boot atBoot(Get get) {
    auto trial = get(kTrial), prev = get(kPrev);
    if (trial.empty() && prev.empty()) return Boot::None;
    return !trial.empty() && !prev.empty() && get(kHost) == trial ? Boot::Trial : Boot::Stale;
}

// Commit, or drop a stale trial. The trial key goes first: without it nothing reads kPrev.
template <typename Put>
bool clear(Put put) {
    return put(kTrial, "") && put(kPrev, "");
}

template <typename Get, typename Put>
bool rollback(Get get, Put put) {
    auto prev = get(kPrev), trial = get(kTrial);
    return put(kHost, prev) && put(kFailed, trial) && clear(put);
}

enum class Step { Wait, Commit, Rollback };

struct Clock {
    bool running = false;
    unsigned long start = 0;
};

// Called often during a trial. The clock only runs while the network is up: a broker can't be
// judged when there's no network to reach it over.
inline Step step(bool mqttConnected, bool networkOnline, unsigned long now, Clock& clock) {
    if (mqttConnected) return Step::Commit;
    if (!networkOnline) {
        clock.running = false;
        return Step::Wait;
    }
    if (!clock.running) clock = {true, now};
    return now - clock.start >= trialMs ? Step::Rollback : Step::Wait;
}

// For reporting a URL: drops any user:password@.
inline std::string redact(const std::string& url) {
    auto start = url.find("://");
    start = start == std::string::npos ? 0 : start + 3;
    auto end = url.find_first_of("/?#", start);
    auto at = url.rfind('@', end == std::string::npos ? std::string::npos : end);
    if (at == std::string::npos || at < start) return url;
    return url.substr(0, start) + url.substr(at + 1);
}

}  // namespace MqttUrlChange
