#include "Settings.h"

#include <sys/stat.h>
#include <unistd.h>

#include <climits>
#include <cstdio>
#include <memory>
#include <optional>

#include "HttpStatic.h"
#include "Logger.h"
#include "Network.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_bit_defs.h"
#include "esp_private/esp_gpio_reserve.h"
#include "esp_spiffs.h"
#include "soc/soc_caps.h"
#include "util.h"

namespace Settings {
namespace {

constexpr const char* BASE = "/spiffs";
constexpr const char* MASKED_PASSWORD = "***###***";
constexpr const char* CONTENT_JSON = "application/json; charset=utf-8";

using Type = SettingType;

struct Param;
// json(..., pins): a side table, since only a handful of settings hold GPIOs in their JSON.
// Allocated once at registration and never freed, so a SettingSpec can point into it.
std::vector<std::pair<const Param*, const PinFields*>> jsonPins;
const PinFields* pinFieldsOf(const Param* p) {
    for (auto& e : jsonPins)
        if (e.first == p) return e.second;
    return nullptr;
}

struct Param {
    Type type;
    // Only what storage, /wifi and template checks need: every setting stays in RAM for the
    // node's lifetime, so labels and dropdown option text (the UI has its own) aren't kept.
    std::string name, value, init;
    long min = LONG_MIN, max = LONG_MAX;
    uint8_t options = 0;  // dropdown option count; the stored value is the index
    bool board = false;   // exported in board templates (markBoard)
    PinUse pin = PinUse::None;  // pinInput()/pinOutput()/pinAnalog()

    SettingSpec spec() const { return {type, min, max, options, pin, pinFieldsOf(this)}; }
    std::string effective() const { return value.empty() ? init : value; }

    std::string filename() const { return "/" + name; }
    void fill() { value = slurp(filename()); }
    bool store() const { return spurt(filename(), value); }

    void set(const std::string& v) {
        switch (type) {
            case Type::Password:
                if (v == MASKED_PASSWORD) return;
                value = v;
                break;
            case Type::Bool:
                // The UI posts unchecked boxes as "0"; an empty value is off too.
                value = v.empty() || v == "0" || v == "false" ? "0" : "1";
                break;
            case Type::Json: {
                // Sent to the UI unquoted, so only a valid array or object may be stored.
                DynamicJsonDocument d(v.size() * 2 + 64);
                if (v.empty() || (!deserializeJson(d, v) && (d.is<JsonArray>() || d.is<JsonObject>())))
                    value = v;
                else
                    Log.printf("%s: not a JSON array or object, kept the old value\n", name.c_str());
                break;
            }
            default:
                value = v;
        }
    }

    std::string json(const std::string& v) const {
        if (v.empty()) return "";
        switch (type) {
            case Type::Password:
                return jsonString(name, MASKED_PASSWORD);
            case Type::Int:
                return jsonNumeric(name, toStr(toInt(v)));
            case Type::Float:
                return jsonNumeric(name, toStr(toFloat(v)));
            case Type::Bool:
                return jsonNumeric(name, toInt(v) ? "true" : "false");
            case Type::Json:
                return jsonNumeric(name, v);  // already JSON text
            default:
                return jsonString(name, v);
        }
    }
    // Typed JSON value; strtod keeps "0.10" as 0.1 rather than float-widening it to 0.100000001.
    // Template, not JsonVariant: obj["k"] for a missing key converts to a null variant that
    // set() silently ignores; the proxy itself adds the member.
    template <class Dst>
    void put(Dst dst, const std::string& v) const {
        switch (type) {
            case Type::Int:
            case Type::Dropdown:
                dst.set(toInt(v));
                break;
            case Type::Float:
                dst.set(strtod(v.c_str(), nullptr));
                break;
            case Type::Bool:
                dst.set(toInt(v) != 0);
                break;
            case Type::Password:
                dst.set(MASKED_PASSWORD);
                break;
            case Type::Json:
                dst.set(serialized(v));
                break;
            default:
                dst.set(v);
        }
    }
    // Equal once parsed: "0.50" and "0.5", or "05" and "5", are not a change.
    bool same(const std::string& a, const std::string& b) const {
        switch (type) {
            case Type::Float:
                return strtod(a.c_str(), nullptr) == strtod(b.c_str(), nullptr);
            case Type::Int:
            case Type::Dropdown:
            case Type::Bool:
                return toInt(a) == toInt(b);
            default:
                return a == b;
        }
    }
    std::string jsonValue() const { return json(value); }
    std::string jsonDefault() const { return type == Type::Password ? "" : json(init); }

    static std::string encode(const std::string& raw) {
        std::string r;
        for (unsigned char c : raw) {
            switch (c) {
                case '"': r += "\\\""; break;
                case '\\': r += "\\\\"; break;
                case '\b': r += "\\b"; break;
                case '\f': r += "\\f"; break;
                case '\n': r += "\\n"; break;
                case '\r': r += "\\r"; break;
                case '\t': r += "\\t"; break;
                default:
                    if (c < 0x20) r += Sprintf("\\u%04x", c);
                    else r += (char)c;
            }
        }
        return r;
    }
    static std::string jsonString(const std::string& n, const std::string& v) { return "\"" + encode(n) + "\":\"" + encode(v) + "\""; }
    static std::string jsonNumeric(const std::string& n, const std::string& v) { return "\"" + encode(n) + "\":" + v; }
};

struct Endpoint {
    std::string name;
    std::vector<Param*> params;
};
std::vector<Endpoint> endpoints;
size_t current = 0;

size_t findOrCreate(const std::string& name) {
    if (endpoints.empty()) endpoints.push_back({"main", {}});
    for (size_t i = 0; i < endpoints.size(); i++)
        if (endpoints[i].name == name) return i;
    endpoints.push_back({name, {}});
    return endpoints.size() - 1;
}

int find(const std::string& name) {
    if (endpoints.empty()) endpoints.push_back({"main", {}});
    for (size_t i = 0; i < endpoints.size(); i++)
        if (endpoints[i].name == name) return i;
    return -1;
}

Param* add(Type type, const std::string& name, const std::string& init) {
    findOrCreate("main");
    auto* p = new Param();
    p->type = type;
    p->name = name;
    p->init = init;
    p->fill();
    endpoints[current].params.push_back(p);
    return p;
}

Param* last() { return endpoints[current].params.back(); }

PinRules pinRules;

Param* findParam(const std::string& key) {
    for (auto& e : endpoints)
        for (auto* p : e.params)
            if (p->name == key) return p;
    return nullptr;
}

// Settings a template may carry: no secrets.
bool templatable(const Param* p) { return p && p->type != Type::Password; }

std::string path(const std::string& fn) { return std::string(BASE) + fn; }

// --- HTTP ---------------------------------------------------------------------------------------

std::string uriPath(httpd_req_t* req) {
    std::string u = req->uri;
    auto q = u.find('?');
    return q == std::string::npos ? u : u.substr(0, q);
}

esp_err_t sendJson(httpd_req_t* req, const std::string& body) {
    HttpWebServer::commonHeaders(req);
    httpd_resp_set_type(req, CONTENT_JSON);
    return httpd_resp_send(req, body.c_str(), body.size());
}

esp_err_t sendText(httpd_req_t* req, const char* status, const char* text) {
    HttpWebServer::commonHeaders(req);
    httpd_resp_set_status(req, status);
    httpd_resp_set_type(req, "text/plain");
    return httpd_resp_send(req, text, HTTPD_RESP_USE_STRLEN);
}

esp_err_t getHandler(httpd_req_t* req) {
    std::string p = uriPath(req);
    Log.printf("GET %s\n", p.c_str());

    if (p == "/wifi/scan") {
        std::string out = "{\"networks\":{";
        bool comma = false;
        for (auto& n : Network::scanNetworks()) {
            if (comma) out += ",";
            out += "\"" + Param::encode(n.first) + "\":" + toStr(n.second);
            comma = true;
        }
        out += "}}";
        return sendJson(req, out);
    }

    std::string name = p.size() <= 6 ? "main" : p.substr(6);
    int idx = find(name);
    if (idx < 0) return sendText(req, "404 Not Found", "Endpoint not found");

    // Streamed in chunks: on a C3 with ~8 KB as the largest free block, building the whole
    // values+defaults document in one string could fail to allocate and abort.
    HttpWebServer::commonHeaders(req);
    httpd_resp_set_type(req, CONTENT_JSON);
    std::string chunk;
    bool ok = true;
    auto put = [&](const std::string& s) {
        chunk += s;
        if (chunk.size() >= 512) {
            ok = ok && httpd_resp_send_chunk(req, chunk.data(), chunk.size()) == ESP_OK;
            chunk.clear();
        }
    };
    auto section = [&](bool defaults) {
        bool comma = false;
        for (auto* prm : endpoints[idx].params) {
            auto s = defaults ? prm->jsonDefault() : prm->jsonValue();
            if (s.empty()) continue;
            if (comma) put(",");
            put(s);
            comma = true;
        }
    };
    put("{\"values\":{");
    section(false);
    put("},\"defaults\":{");
    section(true);
    // The GPIOs a pin picker may offer, from the same masks the save check uses (pins()).
    std::string usable, inputOnly, adc1;
    auto add = [](std::string& list, int i) {
        if (!list.empty()) list += ",";
        list += std::to_string(i);
    };
    for (int i = 0; i < 64; i++) {
        if (!((pinRules.exists >> i) & 1) || ((pinRules.reserved >> i) & 1)) continue;
        add(((pinRules.output >> i) & 1) ? usable : inputOnly, i);
        if ((pinRules.adc1 >> i) & 1) add(adc1, i);
    }
    put("},\"pins\":{\"usable\":[" + usable + (usable.empty() || inputOnly.empty() ? "" : ",") + inputOnly + "],\"inputOnly\":[" + inputOnly + "],\"adc1\":[" + adc1 + "]}}");
    if (ok && !chunk.empty()) ok = httpd_resp_send_chunk(req, chunk.data(), chunk.size()) == ESP_OK;
    return ok ? httpd_resp_send_chunk(req, nullptr, 0) : ESP_FAIL;
}

// Form-encoded body -> value for key, decoded. False (and `out` empty) when absent.
bool formValue(const std::string& body, const std::string& key, std::string& out) {
    out.clear();
    size_t pos = 0;
    while (pos <= body.size()) {
        size_t amp = body.find('&', pos);
        std::string pair = body.substr(pos, amp == std::string::npos ? std::string::npos : amp - pos);
        size_t eq = pair.find('=');
        std::string k = pair.substr(0, eq);
        if (HttpWebServer::urlDecode(k.c_str(), k.size()) == key) {
            if (eq != std::string::npos) out = HttpWebServer::urlDecode(pair.c_str() + eq + 1, pair.size() - eq - 1);
            return true;
        }
        if (amp == std::string::npos) break;
        pos = amp + 1;
    }
    return false;
}

esp_err_t postHandler(httpd_req_t* req) {
    std::string p = uriPath(req);
    Log.printf("POST %s\n", p.c_str());
    std::string name = p.size() <= 6 ? "main" : p.substr(6);
    int idx = find(name);
    if (idx < 0) return sendText(req, "404 Not Found", "Endpoint not found");

    std::string body;
    if (!HttpWebServer::readBody(req, body)) return sendText(req, "413 Payload Too Large", "Body too large");

    // Refuse the whole save if a pin can't work on this chip, before anything is changed.
    // Keys missing from the body keep their stored value (#2582).
    std::string v;
    bool setsPin = false;
    for (auto* prm : endpoints[idx].params) {
        if (prm->pin == PinUse::None && !pinFieldsOf(prm)) continue;
        if (!formValue(body, prm->name, v)) continue;
        std::string err = checkPins(prm->name, prm->spec(), v, pinRules, setsPin);
        if (!err.empty()) {
            Log.printf("POST %s refused: %s\n", p.c_str(), err.c_str());
            return sendText(req, "400 Bad Request", err.c_str());
        }
    }

    bool ok = true;
    for (auto* prm : endpoints[idx].params) {
        if (!formValue(body, prm->name, v)) continue;
        prm->set(v);
        if (!prm->store()) ok = false;
    }
    if (!ok) {
        Log.println("Error writing to flash filesystem");
        return sendText(req, "500 Internal Server Error", "Error writing to flash filesystem");
    }
    HttpWebServer::commonHeaders(req);
    return httpd_resp_send(req, nullptr, 0);
}

}  // namespace

void begin() {
    static bool begun = false;
    if (begun) return;
    begun = true;
    // ESP-IDF reserves the flash (and any PSRAM) pins before app_main; drivers reserve theirs as
    // they start, so snapshot now, while only the memory pins are taken.
    pinRules.exists = SOC_GPIO_VALID_GPIO_MASK;
    pinRules.output = SOC_GPIO_VALID_OUTPUT_GPIO_MASK;
    pinRules.reserved = 0;
    pinRules.adc1 = 0;
    for (int i = 0; i < 64; i++) {
        if (!((pinRules.exists >> i) & 1)) continue;
        if (esp_gpio_is_reserved(BIT64(i))) pinRules.reserved |= BIT64(i);
        // The same lookup Battery uses to read the pin.
        adc_unit_t unit;
        adc_channel_t channel;
        if (adc_oneshot_io_to_channel(i, &unit, &channel) == ESP_OK && unit == ADC_UNIT_1) pinRules.adc1 |= BIT64(i);
    }
    // Two halves: newlib nano printf has no %llx.
    Log.printf("Flash/PSRAM GPIO mask: 0x%08lx%08lx\n", (unsigned long)(pinRules.reserved >> 32), (unsigned long)pinRules.reserved);
    Log.printf("ADC1 GPIO mask: 0x%08lx%08lx\n", (unsigned long)(pinRules.adc1 >> 32), (unsigned long)pinRules.adc1);
    esp_vfs_spiffs_conf_t conf = {};
    conf.base_path = BASE;
    conf.partition_label = "spiffs";
    conf.max_files = 4;
    conf.format_if_mount_failed = true;
    esp_err_t err = esp_vfs_spiffs_register(&conf);
    if (err != ESP_OK) Log.printf("SPIFFS mount failed: %s\n", esp_err_to_name(err));
}

std::string slurp(const std::string& fn) {
    FILE* f = fopen(path(fn).c_str(), "r");
    if (!f) return "";
    std::string r;
    char buf[128];
    size_t n;
    while ((n = fread(buf, 1, sizeof(buf), f)) > 0) r.append(buf, n);
    fclose(f);
    return r;
}

bool spurt(const std::string& fn, const std::string& content) {
    if (content.empty()) return exists(fn) ? remove(fn) : true;
    FILE* f = fopen(path(fn).c_str(), "w");
    if (!f) return false;
    size_t w = fwrite(content.data(), 1, content.size(), f);
    fclose(f);
    return w == content.size();
}

bool exists(const std::string& fn) {
    struct stat st;
    return stat(path(fn).c_str(), &st) == 0;
}

bool remove(const std::string& fn) { return unlink(path(fn).c_str()) == 0; }

std::string string(const std::string& name, const std::string& init, const char* /*label*/) {
    auto* p = add(Type::String, name, init);
    return p->value.empty() ? p->init : p->value;
}

std::string pstring(const std::string& name, const std::string& init, const char* /*label*/) {
    auto* p = add(Type::Password, name, init);
    return p->value.empty() ? p->init : p->value;
}

long dropdown(const std::string& name, const std::vector<std::string>& options, long init, const char* /*label*/) {
    auto* p = add(Type::Dropdown, name, toStr(init));
    p->options = options.size();
    // The UI binds a <select> to values.<name> only, so an unsaved dropdown must still report
    // its default or the control renders blank.
    if (p->value.empty()) p->value = p->init;
    return toInt(p->value);
}

long integer(const std::string& name, long init, const char* /*label*/) {
    auto* p = add(Type::Int, name, toStr(init));
    return toInt(p->value.empty() ? p->init : p->value);
}

long integer(const std::string& name, long min, long max, long init, const char* label) {
    long rv = integer(name, init, label);
    last()->min = min;
    last()->max = max;
    return rv;
}

static long pin(const std::string& name, PinUse use, long init, const char* label) {
    long rv = integer(name, -1, SOC_GPIO_PIN_COUNT - 1, init, label);
    last()->pin = use;
    return rv;
}
long pinInput(const std::string& name, long init, const char* label) { return pin(name, PinUse::Input, init, label); }
long pinOutput(const std::string& name, long init, const char* label) { return pin(name, PinUse::Output, init, label); }
long pinAnalog(const std::string& name, long init, const char* label) { return pin(name, PinUse::Analog, init, label); }

float floating(const std::string& name, float init, const char* /*label*/) {
    auto* p = add(Type::Float, name, toStr(init));
    return toFloat(p->value.empty() ? p->init : p->value);
}

float floating(const std::string& name, long min, long max, float init, const char* label) {
    float rv = floating(name, init, label);
    last()->min = min;
    last()->max = max;
    return rv;
}

bool checkbox(const std::string& name, bool init, const char* /*label*/) {
    auto* p = add(Type::Bool, name, toStr((int)init));
    if (p->value.empty()) p->value = p->init;
    return toInt(p->value) != 0;
}

std::string json(const std::string& name, const std::string& init, const char* /*label*/) {
    auto* p = add(Type::Json, name, init);
    return p->value.empty() ? p->init : p->value;
}

std::string json(const std::string& name, const std::string& init, const char* label,
                 const std::vector<std::pair<std::string, PinUse>>& pins) {
    std::string rv = json(name, init, label);
    jsonPins.push_back({last(), new PinFields(pins)});
    return rv;
}

void markExtra() { current = findOrCreate("extras"); }
void markEndpoint(const std::string& name) { current = findOrCreate(name); }
void markBoard() { last()->board = true; }
const PinRules& pins() { return pinRules; }

std::optional<SettingSpec> spec(const std::string& key) {
    Param* p = findParam(key);
    if (!templatable(p)) return std::nullopt;
    return p->spec();
}

void serializeBoard(JsonObject out) {
    for (auto& e : endpoints)
        for (auto* p : e.params) {
            if (!templatable(p) || !(p->board || e.name == "hardware")) continue;
            if (p->same(p->effective(), p->init)) continue;  // defaults stay out of shared templates
            p->put(out[p->name.c_str()], p->effective());   // names live forever: stored by pointer
        }
}

bool apply(const std::vector<SettingChange>& changes, bool dryRun, JsonArray diff, size_t& changed) {
    bool ok = true;
    changed = 0;
    for (auto& c : changes) {
        Param* p = findParam(c.key);
        if (!templatable(p)) continue;
        std::string from = p->effective();
        if (p->same(from, c.value)) continue;
        changed++;
        JsonObject d = diff.createNestedObject();
        d["key"] = p->name.c_str();
        p->put(d["from"], from);
        p->put(d["to"], c.value);
        if (dryRun) continue;
        p->value = c.value;  // not set(): its form semantics (masked passwords) don't apply
        if (!p->store()) ok = false;
    }
    return ok;
}

void registerHttp(httpd_handle_t server) {
    httpd_uri_t get = {};
    get.uri = "/wifi*";
    get.method = HTTP_GET;
    get.handler = getHandler;
    httpd_register_uri_handler(server, &get);
    httpd_uri_t post = get;
    post.method = HTTP_POST;
    post.handler = postHandler;
    httpd_register_uri_handler(server, &post);
}

}  // namespace Settings
