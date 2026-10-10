#include "Settings.h"

#include <sys/stat.h>
#include <unistd.h>

#include <climits>
#include <cstdio>
#include <memory>

#include "HttpStatic.h"
#include "Logger.h"
#include "Network.h"
#include "esp_spiffs.h"
#include "util.h"

namespace Settings {
namespace {

constexpr const char* BASE = "/spiffs";
constexpr const char* MASKED_PASSWORD = "***###***";
constexpr const char* CONTENT_JSON = "application/json; charset=utf-8";

using Type = SettingType;

struct Param {
    Type type;
    std::string name, label, value, init;
    long min = LONG_MIN, max = LONG_MAX;
    std::vector<std::string> options;
    bool board = false;  // exported in board templates (markBoard)
    SettingSpec spec_;   // filled by spec()

    const SettingSpec* spec() {
        spec_ = {type, min, max, options.size()};
        return &spec_;
    }
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
                value = v.empty() ? "0" : "1";
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

Param* add(Type type, const std::string& name, const std::string& init, const std::string& label) {
    findOrCreate("main");
    auto* p = new Param();
    p->type = type;
    p->name = name;
    p->label = label.empty() ? name : label;
    p->init = init;
    p->fill();
    endpoints[current].params.push_back(p);
    return p;
}

Param* last() { return endpoints[current].params.back(); }

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

    if (startsWith(p, "/wifi/options/")) {
        std::string name = p.substr(14);
        for (auto& e : endpoints)
            for (auto* prm : e.params)
                if (prm->name == name && prm->type == Type::Dropdown) {
                    std::string out = "[";
                    for (size_t i = 0; i < prm->options.size(); i++)
                        out += (i ? ",\"" : "\"") + Param::encode(prm->options[i]) + "\"";
                    out += "]";
                    return sendJson(req, out);
                }
        return sendText(req, "404 Not Found", "Dropdown not found");
    }

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

    std::string out = "{\"values\":{";
    bool comma = false;
    for (auto* prm : endpoints[idx].params) {
        auto s = prm->jsonValue();
        if (s.empty()) continue;
        if (comma) out += ",";
        out += s;
        comma = true;
    }
    out += "},\"defaults\":{";
    comma = false;
    for (auto* prm : endpoints[idx].params) {
        auto s = prm->jsonDefault();
        if (s.empty()) continue;
        if (comma) out += ",";
        out += s;
        comma = true;
    }
    out += "}}";
    return sendJson(req, out);
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

    bool ok = true;
    std::string v;
    for (auto* prm : endpoints[idx].params) {
        formValue(body, prm->name, v);
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

std::string string(const std::string& name, const std::string& init, const std::string& label) {
    auto* p = add(Type::String, name, init, label);
    return p->value.empty() ? p->init : p->value;
}

std::string pstring(const std::string& name, const std::string& init, const std::string& label) {
    auto* p = add(Type::Password, name, init, label);
    return p->value.empty() ? p->init : p->value;
}

long dropdown(const std::string& name, const std::vector<std::string>& options, long init, const std::string& label) {
    auto* p = add(Type::Dropdown, name, toStr(init), label);
    p->options = options;
    // The UI binds a <select> to values.<name> only, so an unsaved dropdown must still report
    // its default or the control renders blank.
    if (p->value.empty()) p->value = p->init;
    return toInt(p->value);
}

long integer(const std::string& name, long init, const std::string& label) {
    auto* p = add(Type::Int, name, toStr(init), label);
    return toInt(p->value.empty() ? p->init : p->value);
}

long integer(const std::string& name, long min, long max, long init, const std::string& label) {
    long rv = integer(name, init, label);
    last()->min = min;
    last()->max = max;
    return rv;
}

float floating(const std::string& name, float init, const std::string& label) {
    auto* p = add(Type::Float, name, toStr(init), label);
    return toFloat(p->value.empty() ? p->init : p->value);
}

float floating(const std::string& name, long min, long max, float init, const std::string& label) {
    float rv = floating(name, init, label);
    last()->min = min;
    last()->max = max;
    return rv;
}

bool checkbox(const std::string& name, bool init, const std::string& label) {
    auto* p = add(Type::Bool, name, toStr((int)init), label);
    if (p->value.empty()) p->value = p->init;
    return toInt(p->value) != 0;
}

std::string json(const std::string& name, const std::string& init, const std::string& label) {
    auto* p = add(Type::Json, name, init, label);
    return p->value.empty() ? p->init : p->value;
}

void markExtra() { current = findOrCreate("extras"); }
void markEndpoint(const std::string& name) { current = findOrCreate(name); }
void markBoard() { last()->board = true; }

const SettingSpec* spec(const std::string& key) {
    Param* p = findParam(key);
    return templatable(p) ? p->spec() : nullptr;
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
        d["label"] = p->label.c_str();
        p->put(d["from"], from);
        p->put(d["to"], c.value);
        if (dryRun) continue;
        p->value = c.value;  // not set(): its form semantics make any present checkbox "on"
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
