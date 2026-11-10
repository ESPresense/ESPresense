#!/usr/bin/env python3
"""Controlled-load C3 pilot on the already-provisioned HIL board, never house nodes."""
import argparse
import http.client
import json
import re
import time
import urllib.parse
from pathlib import Path
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


def verify_build(ip, expected):
    conn = http.client.HTTPConnection(ip, timeout=10)
    try:
        conn.request("GET", "/json")
        response = conn.getresponse()
        data = json.loads(response.read())
        if response.status != 200 or data.get("ver") != expected or data.get("firm") != "esp32c3-csiref":
            raise RuntimeError(f"Unexpected bench firmware: {data}")
        print("BENCH metadata " + json.dumps(data, sort_keys=True), flush=True)
    finally:
        conn.close()


def measure(ser, seconds, mode, flood_ready=None):
    samples = []
    faults = []
    stop = time.monotonic() + seconds
    while time.monotonic() < stop:
        if flood_ready and not Path(flood_ready).exists():
            raise RuntimeError("Controlled BLE flood stopped during measurement")
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
    intervals = [(a, (b["t_us"] - a["t_us"]) / 1e6)
                 for a, b in zip(measured, measured[1:])]
    ready_seconds = sum(dt for sample, dt in intervals if 0 < dt <= 1 and sample.get("ready"))
    unknown_seconds = sum(dt for _, dt in intervals if dt > 1)
    result = {"mode": mode, "seconds": elapsed,
              "adverts_per_minute": (last["adverts"] - first["adverts"]) * 60 / elapsed,
              "ready_fraction": ready_seconds / elapsed,
              "unobserved_seconds": unknown_seconds,
              "max_sample_gap_seconds": max(dt for _, dt in intervals),
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
    parser.add_argument("--expected-version", required=True)
    parser.add_argument("--flood-ready", help="Required live marker throughout each controlled measurement")
    args = parser.parse_args()
    results = []
    with serial.Serial(args.port, 115200, timeout=1) as ser:
        ser.dtr = False
        ser.rts = True
        time.sleep(0.1)
        ser.rts = False
        ip = wait_boot(ser)
        verify_build(ip, args.expected_version)
        post(ip, "/wifi", {"auto_update": "0"})
        try:
            for mode in (0, 1, 2, 0):
                post(ip, "/wifi/hardware", {"csi_reference_mode": str(mode),
                     "csi_reference_arm": "1" if mode else "0",
                     "csi_reference_until": str(int(time.time() + 900))})
                post(ip, "/restart", {})
                ip = wait_boot(ser)
                verify_build(ip, args.expected_version)
                results.append(measure(ser, args.seconds, mode, args.flood_ready))
        finally:
            post(ip, "/wifi/hardware", {"csi_reference_mode": "0", "csi_reference_arm": "0", "csi_reference_until": "0"})
            post(ip, "/restart", {})
    with open(args.out, "w") as output:
        json.dump(results, output, indent=2)
    if not all(result["valid_input"] for result in results):
        raise SystemExit("Reference acquisition/readiness failed; do not score this as detector accuracy")


if __name__ == "__main__":
    main()
