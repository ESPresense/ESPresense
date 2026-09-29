#pragma once
#ifdef COEXIST_TEST
// ESPA-196 (FW-1): measure the real CSI+BLE coexistence knee on HIL bench hardware.
// Compiled only into the `*-coexist` firmware variants (see envs.cmake) - the 4
// production envs never define COEXIST_TEST, so every call site touching this
// component is a no-op there and production behavior is unchanged.
//
// Design: ESPA-172 execution-package comment (39175b73dd6f...), field-format
// finalized 2026-06-09 (comment e8a1539f-8269...). Sweeps a self-ping-driven CSI
// capture rate through {-1,0,10,20,30,50,100} Hz while the existing always-on BLE
// passive scan runs completely unmodified, and emits two serial record types a
// companion parser can grep out of the HIL log. rate=-1 (added ESPA-218) is a true
// CSI-fully-disabled control bucket - comparing it against rate=0 (CSI on, no
// self-ping) isolates whether CSI extraction itself costs BLE duty, independent of
// the self-ping traffic ESPA-196's sweep varies from rate=10 up:
//
//   COEXIST_A board=<id> rate=<hz> dur_s=<n> ble_adverts_seen=<n> ping_sent=<n>
//             ping_recv=<n> csi_captured=<n> csi_dropped=<n> free_heap=<n>
//             min_free_heap=<n> uptime_ms=<n>
//     - one line per (rate, dwell) bucket. NOTE: this is a deliberately smaller
//       field set than the original §A BUCKET_METRICS_FIELDS sketch (no
//       ble_windows_attempted/completed - see .cpp for why); duty-loss % is
//       computed downstream by comparing a bucket's ble_adverts_seen against
//       the R=0 control bucket, exactly as the original runbook specifies.
//
//   COEXIST_B <base64>
//     - one line per captured CSI record: the ratified §B v1 binary frame
//       (magic 0xC5, ver=1), byte-exact, base64-encoded for safe serial
//       transport. This is the part QA's ecf1_adapter.py actually parses.
//
// The reference BLE advertiser is deliberately NOT part of this firmware - it's
// ble-loadgen (ESPresense/ble-loadgen) driven from the HIL pipeline at a fixed
// --rate. This firmware only counts adverts it sees.
#include "Ble.h"

namespace CoexistTest {

// Installs a counting shim in front of the production advert callback so
// `ble_adverts_seen` reflects the exact same scan the fingerprint code sees,
// with zero change to BLE scan behavior itself. Call in place of the plain
// callback at the one Ble::Init() call site.
Ble::AdvertCallback WrapAdvertCallback(Ble::AdvertCallback real);

// Starts the CSI capture + ping-driver + rate-sweep task (and its serial
// emitter task). Safe to call once from setup(); the sweep task itself waits
// for Network::isOnline() before doing anything.
void Begin();

}  // namespace CoexistTest
#endif  // COEXIST_TEST
