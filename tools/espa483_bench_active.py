#!/usr/bin/env python3
"""ESPA-483 active-sensing trial: bench BLE-advert cost at a few csi_active_pps rates,
on an esp32c3-csicap node already flashed and joined to WiFi (see hil_run in
.woodpecker/hil.yml for the flash + improv_wifi steps this assumes already ran).

For each rate in --rates: POST csi_enable=1 + csi_active_pps=<rate> to /wifi/hardware,
restart the node (the setting is read once at boot, see CsiMotion::ConnectToWifi), wait
for it to rejoin, then count BLE New/Del advert lines on the serial log for --duration
seconds -- the same line shape tools/hil_coex_count.py filters in CI. A concurrent
ble-flood step gives a fixed reference BLE load, so counts are comparable across rates.
"""
import argparse
import http.client
import json
import re
import sys
import time
import urllib.parse

import serial

IP_PATTERN = re.compile(r"IP address:\s*(\d+\.\d+\.\d+\.\d+)")
BLE_PATTERN = re.compile(r"^\d+ (New|Del) ")
BOOT_TIMEOUT_SECS = 90


def wait_for_ip(ser, timeout=BOOT_TIMEOUT_SECS):
    deadline = time.monotonic() + timeout
    while time.monotonic() < deadline:
        line = ser.readline().decode("utf-8", "replace").strip()
        if not line:
            continue
        print(f"  serial: {line}", file=sys.stderr)
        m = IP_PATTERN.search(line)
        if m:
            return m.group(1)
    raise RuntimeError("no IP address seen within timeout")


def http_post(ip, path, fields, retries=5):
    body = urllib.parse.urlencode(fields)
    last_err = None
    for _ in range(retries):
        try:
            conn = http.client.HTTPConnection(ip, 80, timeout=5)
            conn.request("POST", path, body, {"Content-Type": "application/x-www-form-urlencoded"})
            resp = conn.getresponse()
            data = resp.read()
            conn.close()
            return resp.status, data
        except (ConnectionRefusedError, OSError, http.client.HTTPException) as e:
            last_err = e
            time.sleep(2)
    raise RuntimeError(f"POST {path} failed after {retries} tries: {last_err}")


def apply_rate(ip, rate):
    status, body = http_post(ip, "/wifi/hardware", {"csi_enable": "1", "csi_active_pps": str(rate)})
    print(f"  POST /wifi/hardware csi_active_pps={rate} -> {status} {body[:200]}", file=sys.stderr)
    if status != 200:
        raise RuntimeError(f"settings POST rejected: {status} {body}")
    http_post(ip, "/restart", {})


def drain_until_reboot(ser, timeout=BOOT_TIMEOUT_SECS):
    """After /restart the node reboots on the same serial link; wait for the fresh IP line."""
    return wait_for_ip(ser, timeout)


def count_adverts(ser, duration):
    new_count = del_count = 0
    other_lines = 0
    deadline = time.monotonic() + duration
    while time.monotonic() < deadline:
        line = ser.readline().decode("utf-8", "replace").strip()
        if not line:
            continue
        m = BLE_PATTERN.match(line)
        if m:
            if m.group(1) == "New":
                new_count += 1
            else:
                del_count += 1
        else:
            other_lines += 1
    elapsed = duration
    return {
        "new": new_count, "del": del_count, "total": new_count + del_count,
        "per_min": (new_count + del_count) * 60.0 / elapsed, "other_lines": other_lines,
    }


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--port", required=True)
    ap.add_argument("--baud", type=int, default=115200)
    ap.add_argument("--rates", type=int, nargs="+", default=[0, 20, 50])
    ap.add_argument("--duration", type=int, default=300, help="seconds counted per rate")
    ap.add_argument("--out", required=True)
    args = ap.parse_args()

    ser = serial.Serial(args.port, args.baud, timeout=1)
    results = []
    try:
        print("Waiting for initial boot / IP...", file=sys.stderr)
        ip = wait_for_ip(ser)
        print(f"Node at {ip}", file=sys.stderr)
        for rate in args.rates:
            print(f"=== rate={rate}pps ===", file=sys.stderr)
            apply_rate(ip, rate)
            ip = drain_until_reboot(ser)
            print(f"  back up at {ip}, settling 15s then counting {args.duration}s", file=sys.stderr)
            time.sleep(15)  # let CSI settle (kSettleMs) and the flood reach steady state
            counts = count_adverts(ser, args.duration)
            counts["rate_pps"] = rate
            print(f"  {counts}", file=sys.stderr)
            results.append(counts)
    finally:
        ser.close()

    with open(args.out, "w") as f:
        json.dump(results, f, indent=2)

    print("\n| rate (pps) | New | Del | total | adverts/min |")
    print("|---|---|---|---|---|")
    for r in results:
        print(f"| {r['rate_pps']} | {r['new']} | {r['del']} | {r['total']} | {r['per_min']:.1f} |")


if __name__ == "__main__":
    main()
