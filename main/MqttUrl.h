#pragma once
// The MQTT broker is one setting (mqtt_host, "Server"): a URL such as mqtts://broker:9883, or a
// bare hostname, which means mqtt://<host> on port 1883. esp-mqtt parses the URL, so the scheme
// picks the transport (mqtt, mqtts; WebSocket is compiled out) and the port defaults to
// that scheme's.
// Pure, so host tests can run it.
#include <cctype>
#include <cstdlib>
#include <string>

namespace MqttUrl {

// A bare IPv6 address needs brackets in a URL. One colon is host:port, so it's left alone.
inline std::string bracketed(const std::string& host) {
    auto first = host.find(':');
    bool ipv6 = first != std::string::npos && host.find(':', first + 1) != std::string::npos;
    return !host.empty() && host.front() != '[' && ipv6 ? "[" + host + "]" : host;
}

inline bool isUrl(const std::string& server) { return server.find("://") != std::string::npos; }

// Lower-cased, as esp-mqtt matches it.
inline std::string scheme(const std::string& uri) {
    auto p = uri.find("://");
    std::string s = p == std::string::npos ? std::string() : uri.substr(0, p);
    for (char& c : s) c = (char)tolower((unsigned char)c);
    return s;
}

// What esp-mqtt gets: the URL as entered, or mqtt://<host> for a bare hostname.
inline std::string uri(const std::string& server) {
    return isUrl(server) ? server : "mqtt://" + bracketed(server);
}

inline bool tls(const std::string& uri) { return scheme(uri) == "mqtts"; }

// ws:// and wss:// would need CONFIG_MQTT_TRANSPORT_WEBSOCKET, which is off to save flash.
inline bool supported(const std::string& uri) {
    auto s = scheme(uri);
    return s == "mqtt" || s == "mqtts";
}

// The URL for the pre-URL settings: host, port ("" = default 1883) and the TLS checkbox. The port
// is left out when it's the scheme's default.
inline std::string fromParts(const std::string& host, const std::string& port, bool useTls) {
    std::string url = std::string(useTls ? "mqtts" : "mqtt") + "://" + bracketed(host);
    long p = port.empty() ? 1883 : strtol(port.c_str(), nullptr, 10);
    if (p > 0 && p != (useTls ? 8883 : 1883)) url += ":" + std::to_string(p);
    return url;
}

// Why mqtt-url/set is refused, or "" to go ahead. Only this node's own topic, never a retained
// message, so one stray publish can't move the whole fleet.
inline std::string refuseSet(const std::string& topic, const std::string& ownTopic, bool retained,
                             const std::string& url, const std::string& currentUri) {
    if (topic != ownTopic) return "only on this node's own topic, not a wildcard";
    if (retained) return "not from a retained message";
    if (!isUrl(url)) return "needs a URL, like mqtts://broker";
    if (!supported(url)) return "only mqtt:// and mqtts:// are supported";
    if (url.size() <= scheme(url).size() + 3) return "no host";
    if (url == currentUri) return "already the server";
    return "";
}

}  // namespace MqttUrl
