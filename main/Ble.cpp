#include "Ble.h"

#include <cstring>

#include "Logger.h"
#include "defaults.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "host/ble_att.h"
#include "host/ble_gatt.h"
#include "host/ble_store.h"
#include "host/util/util.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"
#include "store/config/ble_store_config.h"
#include "util.h"

extern "C" void ble_store_config_init(void);

namespace Ble {

// --- advertisement parsing -----------------------------------------------------------------------

template <typename F>
void Advert::each(F fn) const {
    size_t i = 0;
    while (i + 1 < len) {
        uint8_t l = data[i];
        if (l == 0 || i + 1 + l > len) break;
        if (fn(data[i + 1], data + i + 2, (size_t)l - 1)) return;
        i += 1 + l;
    }
}

bool Advert::haveName() const {
    bool r = false;
    each([&](uint8_t t, const uint8_t*, size_t) { return r = (t == BLE_HS_ADV_TYPE_COMP_NAME || t == BLE_HS_ADV_TYPE_INCOMP_NAME); });
    return r;
}

std::string Advert::getName() const {
    std::string r;
    each([&](uint8_t t, const uint8_t* d, size_t l) {
        if (t != BLE_HS_ADV_TYPE_COMP_NAME && t != BLE_HS_ADV_TYPE_INCOMP_NAME) return false;
        r.assign((const char*)d, l);
        return true;
    });
    return r;
}

bool Advert::haveTXPower() const {
    bool r = false;
    each([&](uint8_t t, const uint8_t*, size_t l) { return r = (t == BLE_HS_ADV_TYPE_TX_PWR_LVL && l >= 1); });
    return r;
}

int8_t Advert::getTXPower() const {
    int8_t r = 0;
    each([&](uint8_t t, const uint8_t* d, size_t l) {
        if (t != BLE_HS_ADV_TYPE_TX_PWR_LVL || l < 1) return false;
        r = (int8_t)d[0];
        return true;
    });
    return r;
}

static size_t uuidWidth(uint8_t t) {
    switch (t) {
        case BLE_HS_ADV_TYPE_INCOMP_UUIDS16: case BLE_HS_ADV_TYPE_COMP_UUIDS16: return 2;
        case BLE_HS_ADV_TYPE_INCOMP_UUIDS32: case BLE_HS_ADV_TYPE_COMP_UUIDS32: return 4;
        case BLE_HS_ADV_TYPE_INCOMP_UUIDS128: case BLE_HS_ADV_TYPE_COMP_UUIDS128: return 16;
    }
    return 0;
}

static size_t svcDataWidth(uint8_t t) {
    switch (t) {
        case BLE_HS_ADV_TYPE_SVC_DATA_UUID16: return 2;
        case BLE_HS_ADV_TYPE_SVC_DATA_UUID32: return 4;
        case BLE_HS_ADV_TYPE_SVC_DATA_UUID128: return 16;
    }
    return 0;
}

static ble_uuid_any_t uuidFrom(const uint8_t* d, size_t w) {
    ble_uuid_any_t u = {};
    ble_uuid_init_from_buf(&u, d, w);
    return u;
}

size_t Advert::getServiceUUIDCount() const {
    size_t n = 0;
    each([&](uint8_t t, const uint8_t*, size_t l) {
        size_t w = uuidWidth(t);
        if (w) n += l / w;
        return false;
    });
    return n;
}

ble_uuid_any_t Advert::getServiceUUID(size_t idx) const {
    ble_uuid_any_t r = {};
    size_t n = 0;
    each([&](uint8_t t, const uint8_t* d, size_t l) {
        size_t w = uuidWidth(t);
        if (!w) return false;
        for (size_t o = 0; o + w <= l; o += w, n++)
            if (n == idx) {
                r = uuidFrom(d + o, w);
                return true;
            }
        return false;
    });
    return r;
}

size_t Advert::getServiceDataCount() const {
    size_t n = 0;
    each([&](uint8_t t, const uint8_t*, size_t l) {
        size_t w = svcDataWidth(t);
        if (w && l >= w) n++;
        return false;
    });
    return n;
}

ble_uuid_any_t Advert::getServiceDataUUID(size_t idx) const {
    ble_uuid_any_t r = {};
    size_t n = 0;
    each([&](uint8_t t, const uint8_t* d, size_t l) {
        size_t w = svcDataWidth(t);
        if (!w || l < w) return false;
        if (n++ != idx) return false;
        r = uuidFrom(d, w);
        return true;
    });
    return r;
}

std::string Advert::getServiceData(size_t idx) const {
    std::string r;
    size_t n = 0;
    each([&](uint8_t t, const uint8_t* d, size_t l) {
        size_t w = svcDataWidth(t);
        if (!w || l < w) return false;
        if (n++ != idx) return false;
        r.assign((const char*)d + w, l - w);
        return true;
    });
    return r;
}

bool Advert::haveManufacturerData() const {
    bool r = false;
    each([&](uint8_t t, const uint8_t*, size_t) { return r = (t == BLE_HS_ADV_TYPE_MFG_DATA); });
    return r;
}

std::string Advert::getManufacturerData() const {
    std::string r;
    each([&](uint8_t t, const uint8_t* d, size_t l) {
        if (t != BLE_HS_ADV_TYPE_MFG_DATA) return false;
        r.assign((const char*)d, l);
        return true;
    });
    return r;
}

std::string uuidStr(const ble_uuid_any_t& u) {
    char buf[BLE_UUID_STR_LEN];
    return ble_uuid_to_str(&u.u, buf);
}

std::string addrStr(const ble_addr_t& a) {
    return Sprintf("%02x:%02x:%02x:%02x:%02x:%02x", a.val[5], a.val[4], a.val[3], a.val[2], a.val[1], a.val[0]);
}

ble_uuid_any_t uuid16(uint16_t v) {
    ble_uuid_any_t u = {};
    u.u16.u.type = BLE_UUID_TYPE_16;
    u.u16.value = v;
    return u;
}

ble_uuid_any_t uuid128(uint32_t first, uint16_t second, uint16_t third, uint64_t fourth) {
    ble_uuid_any_t u = {};
    u.u128.u.type = BLE_UUID_TYPE_128;
    memcpy(u.u128.value + 12, &first, 4);
    memcpy(u.u128.value + 10, &second, 2);
    memcpy(u.u128.value + 8, &third, 2);
    memcpy(u.u128.value, &fourth, 8);
    return u;
}

// --- host ----------------------------------------------------------------------------------------

namespace {
AdvertCallback advertCb = nullptr;
SemaphoreHandle_t syncSem;
uint8_t ownAddrType = BLE_OWN_ADDR_PUBLIC;
volatile bool scanning = false;
int gapEvent(struct ble_gap_event* ev, void* arg);

// Peripheral state
Server::Callbacks serverCb = {};
std::string modelNumber = "ESPresense";
uint16_t hrValHandle = 0;
int peripheralConns = 0;
bool advertising = false;

const ble_uuid16_t uHrm = BLE_UUID16_INIT(0x180D), uHrMeas = BLE_UUID16_INIT(0x2A37);
const ble_uuid16_t uDevInfo = BLE_UUID16_INIT(0x180A), uManuf = BLE_UUID16_INIT(0x2A29), uAppearance = BLE_UUID16_INIT(0x2A01), uModel = BLE_UUID16_INIT(0x2A24);

int gattAccess(uint16_t conn, uint16_t attr, struct ble_gatt_access_ctxt* ctxt, void* arg) {
    if (ctxt->op != BLE_GATT_ACCESS_OP_READ_CHR) return BLE_ATT_ERR_UNLIKELY;
    const ble_uuid_t* u = ctxt->chr->uuid;
    if (ble_uuid_cmp(u, &uManuf.u) == 0) return os_mbuf_append(ctxt->om, "ESPresense", 10) ? BLE_ATT_ERR_INSUFFICIENT_RES : 0;
    if (ble_uuid_cmp(u, &uModel.u) == 0) return os_mbuf_append(ctxt->om, modelNumber.data(), modelNumber.size()) ? BLE_ATT_ERR_INSUFFICIENT_RES : 0;
    if (ble_uuid_cmp(u, &uAppearance.u) == 0) {
        uint16_t appearance = 0x4142;
        return os_mbuf_append(ctxt->om, &appearance, 2) ? BLE_ATT_ERR_INSUFFICIENT_RES : 0;
    }
    if (ble_uuid_cmp(u, &uHrMeas.u) == 0) {
        uint8_t v[2] = {0, 0};
        return os_mbuf_append(ctxt->om, v, 2) ? BLE_ATT_ERR_INSUFFICIENT_RES : 0;
    }
    return BLE_ATT_ERR_UNLIKELY;
}

struct ble_gatt_chr_def chr(const ble_uuid16_t& u, ble_gatt_chr_flags flags, uint16_t* valHandle) {
    struct ble_gatt_chr_def d = {};
    d.uuid = &u.u;
    d.access_cb = gattAccess;
    d.flags = flags;
    d.val_handle = valHandle;
    return d;
}

const struct ble_gatt_chr_def hrmChrs[] = {
    chr(uHrMeas, BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_READ_ENC, &hrValHandle),
    {},
};
const struct ble_gatt_chr_def devInfoChrs[] = {
    chr(uManuf, BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_READ_ENC, nullptr),
    chr(uAppearance, BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_READ_ENC, nullptr),
    chr(uModel, BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_READ_ENC, nullptr),
    {},
};
struct ble_gatt_svc_def svc(const ble_uuid16_t& u, const struct ble_gatt_chr_def* chrs) {
    struct ble_gatt_svc_def d = {};
    d.type = BLE_GATT_SVC_TYPE_PRIMARY;
    d.uuid = &u.u;
    d.characteristics = chrs;
    return d;
}
const struct ble_gatt_svc_def services[] = {svc(uHrm, hrmChrs), svc(uDevInfo, devInfoChrs), {}};

void onReset(int reason) { Log.printf("BLE host reset; reason=%d\r\n", reason); }

void onSync() {
    ble_hs_util_ensure_addr(0);
    ble_hs_id_infer_auto(0, &ownAddrType);
    xSemaphoreGive(syncSem);
}

void hostTask(void*) {
    nimble_port_run();
    nimble_port_freertos_deinit();
}

// Client-side wait state: one outstanding operation at a time (the scan task is the only caller).
struct Pending {
    SemaphoreHandle_t sem;
    int status = 0;
    uint16_t startHandle = 0, endHandle = 0, valHandle = 0;
    std::string value;
    Client* client = nullptr;
} pending;

bool waitPending(int ms) { return xSemaphoreTake(pending.sem, pdMS_TO_TICKS(ms)) == pdTRUE; }

int gapEvent(struct ble_gap_event* ev, void* arg) {
    switch (ev->type) {
        case BLE_GAP_EVENT_DISC:
            if (advertCb) {
                Advert a(ev->disc);
                advertCb(a);
            }
            return 0;
        case BLE_GAP_EVENT_DISC_COMPLETE:
            scanning = false;
            return 0;
        case BLE_GAP_EVENT_CONNECT: {
            struct ble_gap_conn_desc desc;
            bool ok = ev->connect.status == 0 && ble_gap_conn_find(ev->connect.conn_handle, &desc) == 0;
            if (ok && desc.role == BLE_GAP_ROLE_SLAVE) {
                peripheralConns++;
                advertising = false;
                if (serverCb.onConnect) serverCb.onConnect(ev->connect.conn_handle, desc.peer_ota_addr);
                return 0;
            }
            if (pending.client) {
                pending.status = ev->connect.status;
                pending.client->conn = ok ? ev->connect.conn_handle : BLE_HS_CONN_HANDLE_NONE;
                xSemaphoreGive(pending.sem);
            }
            return 0;
        }
        case BLE_GAP_EVENT_DISCONNECT:
            if (ev->disconnect.conn.role == BLE_GAP_ROLE_SLAVE) {
                if (peripheralConns > 0) peripheralConns--;
                if (serverCb.onDisconnect) serverCb.onDisconnect(ev->disconnect.conn.conn_handle, ev->disconnect.reason);
            } else if (pending.client && pending.client->conn == ev->disconnect.conn.conn_handle) {
                pending.client->conn = BLE_HS_CONN_HANDLE_NONE;
                pending.status = BLE_HS_ENOTCONN;
                xSemaphoreGive(pending.sem);
            }
            return 0;
        case BLE_GAP_EVENT_ADV_COMPLETE:
            advertising = false;
            return 0;
        case BLE_GAP_EVENT_CONN_UPDATE_REQ:
            return 0;  // accept the peer's parameters
        case BLE_GAP_EVENT_ENC_CHANGE: {
            struct ble_gap_conn_desc desc;
            if (ble_gap_conn_find(ev->enc_change.conn_handle, &desc) == 0)
                Log.printf("Encrypt connection %s conn: %d!\r\n", desc.sec_state.encrypted ? "success" : "failed", ev->enc_change.conn_handle);
            return 0;
        }
        case BLE_GAP_EVENT_MTU:
            Log.printf("MTU updated: %u for connection ID: %u\r\n", ev->mtu.value, ev->mtu.conn_handle);
            return 0;
        case BLE_GAP_EVENT_SUBSCRIBE:
            Log.printf("Client ID: %u %s notifications\r\n", ev->subscribe.conn_handle, ev->subscribe.cur_notify ? "subscribed to" : "unsubscribed from");
            return 0;
        case BLE_GAP_EVENT_REPEAT_PAIRING: {
            // Peer lost its bond: drop ours and let it pair again.
            struct ble_gap_conn_desc desc;
            if (ble_gap_conn_find(ev->repeat_pairing.conn_handle, &desc) == 0) ble_store_util_delete_peer(&desc.peer_id_addr);
            return BLE_GAP_REPEAT_PAIRING_RETRY;
        }
        default:
            return 0;
    }
}

int onSvcDisc(uint16_t, const struct ble_gatt_error* err, const struct ble_gatt_svc* svc, void*) {
    if (err->status == 0 && svc) {
        pending.startHandle = svc->start_handle;
        pending.endHandle = svc->end_handle;
        return 0;
    }
    pending.status = err->status == BLE_HS_EDONE ? 0 : err->status;
    xSemaphoreGive(pending.sem);
    return 0;
}

int onChrDisc(uint16_t, const struct ble_gatt_error* err, const struct ble_gatt_chr* chr, void*) {
    if (err->status == 0 && chr) {
        pending.valHandle = chr->val_handle;
        return 0;
    }
    pending.status = err->status == BLE_HS_EDONE ? 0 : err->status;
    xSemaphoreGive(pending.sem);
    return 0;
}

int onRead(uint16_t, const struct ble_gatt_error* err, struct ble_gatt_attr* attr, void*) {
    pending.status = err->status;
    if (err->status == 0 && attr && attr->om) {
        uint16_t len = OS_MBUF_PKTLEN(attr->om);
        pending.value.resize(len);
        os_mbuf_copydata(attr->om, 0, len, &pending.value[0]);
    }
    xSemaphoreGive(pending.sem);
    return 0;
}

int onWrite(uint16_t, const struct ble_gatt_error* err, struct ble_gatt_attr*, void*) {
    pending.status = err->status;
    xSemaphoreGive(pending.sem);
    return 0;
}
}  // namespace

void Init(const char* deviceName, AdvertCallback cb) {
    advertCb = cb;
    syncSem = xSemaphoreCreateBinary();
    pending.sem = xSemaphoreCreateBinary();
    nimble_port_init();
    ble_hs_cfg.reset_cb = onReset;
    ble_hs_cfg.sync_cb = onSync;
    ble_hs_cfg.store_status_cb = ble_store_util_status_rr;
    ble_hs_cfg.sm_io_cap = BLE_SM_IO_CAP_NO_IO;
    ble_hs_cfg.sm_bonding = 1;
    ble_hs_cfg.sm_mitm = 1;
    ble_hs_cfg.sm_sc = 1;
    ble_hs_cfg.sm_our_key_dist = BLE_SM_PAIR_KEY_DIST_ENC | BLE_SM_PAIR_KEY_DIST_ID;
    ble_hs_cfg.sm_their_key_dist = BLE_SM_PAIR_KEY_DIST_ENC | BLE_SM_PAIR_KEY_DIST_ID;
    ble_svc_gap_init();
    ble_svc_gatt_init();
    ble_svc_gap_device_name_set(deviceName);
    ble_gatts_count_cfg(services);
    ble_gatts_add_svcs(services);
    ble_store_config_init();
    ble_att_set_preferred_mtu(23);
    nimble_port_freertos_init(hostTask);
    xSemaphoreTake(syncSem, portMAX_DELAY);
}

void DeleteAllBonds() { ble_store_clear(); }

bool StartScan() {
    if (scanning) return true;
    struct ble_gap_disc_params p = {};
    p.passive = 1;
    p.filter_duplicates = 0;
    p.itvl = BLE_SCAN_INTERVAL;
    p.window = BLE_SCAN_WINDOW;
    p.filter_policy = BLE_HCI_SCAN_FILT_NO_WL;
    int rc = ble_gap_disc(ownAddrType, BLE_HS_FOREVER, &p, gapEvent, nullptr);
    scanning = rc == 0 || rc == BLE_HS_EALREADY;
    return scanning;
}

void StopScan() {
    if (!scanning) return;
    ble_gap_disc_cancel();
    scanning = false;
}

bool IsScanning() { return scanning; }

// --- client --------------------------------------------------------------------------------------

bool Client::connect(const ble_addr_t& peer, int timeoutMs) {
    StopScan();
    struct ble_gap_conn_params p = {};
    p.scan_itvl = 0x10;
    p.scan_window = 0x10;
    p.itvl_min = 12;
    p.itvl_max = 12;
    p.latency = 0;
    p.supervision_timeout = 48;
    p.min_ce_len = 0;
    p.max_ce_len = 0;
    pending.client = this;
    pending.status = 0;
    xSemaphoreTake(pending.sem, 0);
    int rc = ble_gap_connect(ownAddrType, &peer, timeoutMs, &p, gapEvent, this);
    if (rc != 0) {
        pending.client = nullptr;
        return false;
    }
    if (!waitPending(timeoutMs + 1000)) {
        ble_gap_conn_cancel();
        waitPending(1000);
    }
    bool ok = isConnected();
    if (!ok) pending.client = nullptr;
    return ok;
}

bool Client::findChr(const ble_uuid_any_t& svc, const ble_uuid_any_t& chr, uint16_t& valHandle) {
    if (!isConnected()) return false;
    pending.startHandle = pending.endHandle = pending.valHandle = 0;
    xSemaphoreTake(pending.sem, 0);
    if (ble_gattc_disc_svc_by_uuid(conn, &svc.u, onSvcDisc, nullptr) != 0) return false;
    if (!waitPending(BLE_GATTC_UNRESPONSIVE_TIMEOUT_MS) || pending.startHandle == 0) return false;
    xSemaphoreTake(pending.sem, 0);
    if (ble_gattc_disc_chrs_by_uuid(conn, pending.startHandle, pending.endHandle, &chr.u, onChrDisc, nullptr) != 0) return false;
    if (!waitPending(BLE_GATTC_UNRESPONSIVE_TIMEOUT_MS) || pending.valHandle == 0) return false;
    valHandle = pending.valHandle;
    return true;
}

std::string Client::read(const ble_uuid_any_t& svc, const ble_uuid_any_t& chr) {
    uint16_t h;
    if (!findChr(svc, chr, h)) return "";
    pending.value.clear();
    xSemaphoreTake(pending.sem, 0);
    if (ble_gattc_read(conn, h, onRead, nullptr) != 0) return "";
    if (!waitPending(BLE_GATTC_UNRESPONSIVE_TIMEOUT_MS) || pending.status != 0) return "";
    return pending.value;
}

bool Client::write(const ble_uuid_any_t& svc, const ble_uuid_any_t& chr, const uint8_t* data, size_t len) {
    uint16_t h;
    if (!findChr(svc, chr, h)) return false;
    xSemaphoreTake(pending.sem, 0);
    if (ble_gattc_write_flat(conn, h, data, len, onWrite, nullptr) != 0) return false;
    return waitPending(BLE_GATTC_UNRESPONSIVE_TIMEOUT_MS) && pending.status == 0;
}

void Client::disconnect() {
    if (!isConnected()) {
        pending.client = nullptr;
        return;
    }
    xSemaphoreTake(pending.sem, 0);
    if (ble_gap_terminate(conn, BLE_ERR_REM_USER_CONN_TERM) == 0) waitPending(2000);
    conn = BLE_HS_CONN_HANDLE_NONE;
    pending.client = nullptr;
}

// --- server --------------------------------------------------------------------------------------

namespace Server {

void SetCallbacks(const Callbacks& cb) { serverCb = cb; }
void SetModelNumber(const std::string& model) { modelNumber = model; }
int ConnectedCount() { return peripheralConns; }

bool NotifyHeartRate(uint8_t flags, uint8_t bpm) {
    uint8_t buf[2] = {flags, bpm};
    bool any = false;
    // ponytail: notify every connected central; there are at most CONFIG_BT_NIMBLE_MAX_CONNECTIONS.
    for (uint16_t h = 0; h < CONFIG_BT_NIMBLE_MAX_CONNECTIONS + 1; h++) {
        struct ble_gap_conn_desc desc;
        if (ble_gap_conn_find(h, &desc) != 0 || desc.role != BLE_GAP_ROLE_SLAVE) continue;
        struct os_mbuf* om = ble_hs_mbuf_from_flat(buf, sizeof(buf));
        if (om && ble_gatts_notify_custom(h, hrValHandle, om) == 0) any = true;
    }
    return any;
}

void Disconnect(uint16_t connHandle) { ble_gap_terminate(connHandle, BLE_ERR_REM_USER_CONN_TERM); }

bool ReadPeerIrk(uint16_t connHandle, std::string& irkHex) {
    struct ble_gap_conn_desc desc;
    if (ble_gap_conn_find(connHandle, &desc) != 0) return false;
    struct ble_store_key_sec key = {};
    key.peer_addr = desc.peer_id_addr;
    struct ble_store_value_sec bond;
    if (ble_store_read_peer_sec(&key, &bond) != 0 || !bond.irk_present) return false;
    static const char hex[] = "0123456789abcdef";
    irkHex.clear();
    for (int i = 0; i < 16; i++) {
        auto c = bond.irk[15 - i];
        irkHex.push_back(hex[c >> 4]);
        irkHex.push_back(hex[c & 15]);
    }
    return true;
}

bool AdvertiseRaw(const uint8_t* adv, size_t len) {
    StopAdvertising();
    if (ble_gap_adv_set_data(adv, len) != 0) return false;
    struct ble_gap_adv_params p = {};
    p.conn_mode = BLE_GAP_CONN_MODE_NON;
    p.disc_mode = BLE_GAP_DISC_MODE_NON;
    advertising = ble_gap_adv_start(ownAddrType, nullptr, BLE_HS_FOREVER, &p, gapEvent, nullptr) == 0;
    return advertising;
}

bool AdvertiseConnectable() {
    StopAdvertising();
    struct ble_hs_adv_fields f = {};
    f.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;
    ble_uuid16_t hrm = BLE_UUID16_INIT(0x180D);
    f.uuids16 = &hrm;
    f.num_uuids16 = 1;
    f.uuids16_is_complete = 1;
    f.slave_itvl_range = (uint8_t*)"\x06\x00\x12\x00";
    if (ble_gap_adv_set_fields(&f) != 0) return false;
    struct ble_hs_adv_fields rsp = {};
    const char* name = ble_svc_gap_device_name();
    rsp.name = (uint8_t*)name;
    rsp.name_len = strlen(name);
    rsp.name_is_complete = 1;
    ble_gap_adv_rsp_set_fields(&rsp);
    struct ble_gap_adv_params p = {};
    p.conn_mode = BLE_GAP_CONN_MODE_UND;
    p.disc_mode = BLE_GAP_DISC_MODE_GEN;
    advertising = ble_gap_adv_start(ownAddrType, nullptr, BLE_HS_FOREVER, &p, gapEvent, nullptr) == 0;
    return advertising;
}

void StopAdvertising() {
    ble_gap_adv_stop();
    advertising = false;
}

}  // namespace Server
}  // namespace Ble
