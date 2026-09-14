#pragma once
// Thin layer over the ESP-IDF NimBLE host (host/ble_*.h): continuous passive scanning with a
// zero-copy advertisement parser, a synchronous GATT client for the query path, and the
// enrollment peripheral (HRM + Device Information services, iBeacon / connectable advertising).
#include <cstddef>
#include <cstdint>
#include <string>

#include "host/ble_gap.h"
#include "host/ble_hs.h"
#include "host/ble_uuid.h"

namespace Ble {

// One advertisement report. Fields are parsed on demand from the raw payload (<= 31 bytes),
// so a report costs nothing until the fingerprint code asks for something.
class Advert {
   public:
    Advert(const ble_gap_disc_desc& d) : addr(d.addr), rssi(d.rssi), eventType(d.event_type), data(d.data), len(d.length_data) {}

    const ble_addr_t& getAddress() const { return addr; }
    uint8_t getAddressType() const { return addr.type; }
    int8_t getRSSI() const { return rssi; }
    uint8_t getAdvType() const { return eventType; }

    bool haveName() const;
    std::string getName() const;
    bool haveTXPower() const;
    int8_t getTXPower() const;
    size_t getServiceUUIDCount() const;
    ble_uuid_any_t getServiceUUID(size_t i) const;
    size_t getServiceDataCount() const;
    ble_uuid_any_t getServiceDataUUID(size_t i) const;
    std::string getServiceData(size_t i) const;
    bool haveManufacturerData() const;
    std::string getManufacturerData() const;

   private:
    // Walk the AD structures; fn(type, data, len) returns true to stop.
    template <typename F>
    void each(F fn) const;
    ble_addr_t addr;
    int8_t rssi;
    uint8_t eventType;
    const uint8_t* data;
    uint8_t len;
};

using AdvertCallback = void (*)(const Advert&);

void Init(const char* deviceName, AdvertCallback cb);  // start host, wait for sync
void DeleteAllBonds();
bool StartScan();
void StopScan();
bool IsScanning();

std::string uuidStr(const ble_uuid_any_t& u);  // ble_uuid_to_str format (same as NimBLE-Arduino)
std::string addrStr(const ble_addr_t& a);
inline bool uuidEq(const ble_uuid_any_t& a, const ble_uuid_any_t& b) { return ble_uuid_cmp(&a.u, &b.u) == 0; }
inline bool addrEq(const ble_addr_t& a, const ble_addr_t& b) { return memcmp(a.val, b.val, 6) == 0; }
ble_uuid_any_t uuid16(uint16_t v);
ble_uuid_any_t uuid128(uint32_t first, uint16_t second, uint16_t third, uint64_t fourth);  // NimBLEUUID(a,b,c,d) layout

// Synchronous GATT client for the query path. Scanning is stopped while connected; the
// scan task restarts it afterwards.
class Client {
   public:
    ~Client() { disconnect(); }
    bool connect(const ble_addr_t& peer, int timeoutMs = 5000);
    bool isConnected() const { return conn != BLE_HS_CONN_HANDLE_NONE; }
    // Read a characteristic by service/characteristic UUID. Empty on any failure.
    std::string read(const ble_uuid_any_t& svc, const ble_uuid_any_t& chr);
    bool write(const ble_uuid_any_t& svc, const ble_uuid_any_t& chr, const uint8_t* data, size_t len);
    void disconnect();

    uint16_t conn = BLE_HS_CONN_HANDLE_NONE;  // set from the GAP event callback

   private:
    bool findChr(const ble_uuid_any_t& svc, const ble_uuid_any_t& chr, uint16_t& valHandle);
};

// Peripheral (enrollment). Services are registered before the host starts.
namespace Server {
struct Callbacks {
    void (*onConnect)(uint16_t connHandle, const ble_addr_t& peer);
    void (*onDisconnect)(uint16_t connHandle, int reason);
};
void SetCallbacks(const Callbacks& cb);
void SetModelNumber(const std::string& model);  // Device Information 0x2A24
int ConnectedCount();
bool NotifyHeartRate(uint8_t flags, uint8_t bpm);
void Disconnect(uint16_t connHandle);
bool ReadPeerIrk(uint16_t connHandle, std::string& irkHex);  // from the bond, reversed hex as before

// Non-connectable raw advertisement (iBeacon), or connectable HRM advertising with scan response.
bool AdvertiseRaw(const uint8_t* adv, size_t len);
bool AdvertiseConnectable();
void StopAdvertising();
}  // namespace Server

}  // namespace Ble
