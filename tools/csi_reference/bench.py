#!/usr/bin/env python3
"""Controlled-load C3 pilot on the already-provisioned HIL board, never house nodes."""
import argparse
import http.client
import json
import re
import time
import urllib.parse
import serial

ANSI = re.compile(r"\x1b\[[0-9;]*m")
IP = re.compile(r"IP address:\s*(\d+\.\d+\.\d+\.\d+)")


def post(ip, path, values):
    conn = http.client.HTTPConnection(ip, timeout=10)
    try:
        conn.request("POST", path, urllib.parse.urlencode(values),
                     {"Content-Type": "application/x-www-form-urlencoded"})
        response = conn.getresponse()
        body = response.read()
        if response.status != 200:
            raise RuntimeError(f"{path}: HTTP {response.status}: {body[:150]!r}")
    finally:
        conn.close()


def wait_boot(ser):
    stop = time.monotonic() + 120
    while time.monotonic() < stop:
        line = ANSI.sub("", ser.readline().decode("utf-8", "replace").strip())
        match = IP.search(line)
        if match:
            print(f"BENCH boot confirmed: {line}", flush=True)
            return match.group(1)
        if "CSIREF" in line or "restart" in line.lower() or "panic" in line.lower():
            print(f"BENCH boot: {line}", flush=True)
    raise RuntimeError("No post-reset IP in serial log")


def measure(ser, seconds, mode):
    samples = []
    faults = []
    stop = time.monotonic() + seconds
    while time.monotonic() < stop:
        line = ANSI.sub("", ser.readline().decode("utf-8", "replace").strip())
        if line.startswith("CSIREF {"):
            samples.append(json.loads(line[7:]))
        elif line.startswith("CSIREF fault") or "panic" in line.lower():
            faults.append(line)
            print(line, flush=True)
    if len(samples) < 10:
        raise RuntimeError(f"mode {mode}: insufficient reference records ({len(samples)})")
    # Exclude restart and initial calibration time uniformly. Count application
    # BLE advertisement callbacks, not fingerprint New/Del lines.
    start = samples[0]["t_us"] + 20_000_000
    measured = [s for s in samples if s["t_us"] >= start]
    if len(measured) < 2:
        raise RuntimeError("Not enough post-warmup data")
    first, last = measured[0], measured[-1]
    elapsed = (last["t_us"] - first["t_us"]) / 1e6
    if elapsed <= 0 or last["adverts"] < first["adverts"]:
        raise RuntimeError("Counter epoch changed during measurement")
    ready = [s for s in measured if s.get("ready")]
    result = {"mode": mode, "seconds": elapsed,
              "adverts_per_minute": (last["adverts"] - first["adverts"]) * 60 / elapsed,
              "ready_fraction": len(ready) / len(measured),
              "slot_coverage_mean": (sum(s.get("slots", 0) / max(1, s.get("window_slots", 0))
                                         for s in measured) / len(measured)),
              "heap_min": min((s["heap"] for s in measured if "heap" in s), default=None),
              "first": first, "last": last, "faults": faults}
    if mode and (faults or result["ready_fraction"] < 0.5):
        result["valid_input"] = False
    else:
        result["valid_input"] = True
    print("CSI_REFERENCE_PILOT " + json.dumps(result, sort_keys=True), flush=True)
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--port", required=True)
    parser.add_argument("--seconds", type=int, default=120)
    parser.add_argument("--out", required=True)
    args = parser.parse_args()
    results = []
    with serial.Serial(args.port, 115200, timeout=1) as ser:
        ser.dtr = False
        ser.rts = True
        time.sleep(0.1)
        ser.rts = False
        ip = wait_boot(ser)
        post(ip, "/wifi", {"auto_update": "0"})
        try:
            for mode in (0, 1, 2, 0):
                post(ip, "/wifi/hardware", {"csi_reference_mode": str(mode),
                     "csi_reference_until": str(int(time.time() + 900))})
                post(ip, "/restart", {})
                ip = wait_boot(ser)
                results.append(measure(ser, args.seconds, mode))
        finally:
            post(ip, "/wifi/hardware", {"csi_reference_mode": "0", "csi_reference_until": "0"})
            post(ip, "/restart", {})
    with open(args.out, "w") as output:
        json.dump(results, output, indent=2)
    if not all(result["valid_input"] for result in results):
        raise SystemExit("Reference acquisition/readiness failed; do not score this as detector accuracy")


if __name__ == "__main__":
    main()
