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
    # The IP line precedes completion of setup. SDK startup can delay the first
    # HTTP request; retry transport failures, never an unexpected identity.
    stop = time.monotonic() + 45
    last_error = None
    while time.monotonic() < stop:
        conn = http.client.HTTPConnection(ip, timeout=3)
        try:
            conn.request("GET", "/json")
            response = conn.getresponse()
            data = json.loads(response.read())
            if response.status != 200:
                raise OSError(f"Metadata HTTP {response.status}")
            if data.get("ver") != expected or data.get("firm") != "esp32c3-csiref":
                raise RuntimeError(f"Unexpected bench firmware: {data}")
            print("BENCH metadata " + json.dumps(data, sort_keys=True), flush=True)
            return
        except (OSError, http.client.HTTPException, ValueError) as error:
            last_error = error
            time.sleep(0.5)
        finally:
            conn.close()
    raise RuntimeError(f"Bench metadata unavailable after boot: {last_error}")


def measure(ser, seconds, mode, flood_ready=None, ble_scan=True):
    samples = []
    faults = []
    stop = time.monotonic() + seconds
    while time.monotonic() < stop:
        if flood_ready and not Path(flood_ready).exists():
            raise RuntimeError("Controlled BLE flood stopped during measurement")
        line = ANSI.sub("", ser.readline().decode("utf-8", "replace").strip())
        if line.startswith("CSIREF {"):
            samples.append(json.loads(line[7:]))
        elif IP.search(line) or line.startswith("ESP-ROM:"):
            raise RuntimeError("Device rebooted during the measurement")
        elif line.startswith("CSIREF fault") or "panic" in line.lower():
            faults.append(line)
            print(line, flush=True)
    if len(samples) < 10:
        raise RuntimeError(f"mode {mode}: insufficient reference records ({len(samples)})")
    if any(b["t_us"] <= a["t_us"] or b["adverts"] < a["adverts"]
           for a, b in zip(samples, samples[1:])):
        raise RuntimeError("Counter epoch changed within the measurement")
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
    # Off reports are nominally 1 Hz, active reports 4 Hz. Allow cadence jitter
    # before calling an interval unobserved; a 1.01s off interval is not a gap.
    gap_limit = 1.0 if mode else 2.0
    ready_seconds = sum(dt for sample, dt in intervals if 0 < dt <= gap_limit and sample.get("ready"))
    unknown_seconds = sum(dt for _, dt in intervals if dt > gap_limit)
    result = {"mode": mode, "ble_scan": ble_scan,
              "condition": "BLE coexistence" if ble_scan else "No-scan diagnostic control",
              "seconds": elapsed,
              "adverts_per_minute": (last["adverts"] - first["adverts"]) * 60 / elapsed,
              "ready_fraction": ready_seconds / elapsed,
              "unobserved_seconds": unknown_seconds,
              "max_sample_gap_seconds": max(dt for _, dt in intervals),
              "slot_coverage_mean": (sum(dt * s.get("slots", 0) / max(1, s.get("window_slots", 0))
                                         for s, dt in intervals if 0 < dt <= gap_limit) / elapsed),
              "coverage_method": "time weighted; telemetry gaps remain unknown",
              "heap_min": min((s["heap"] for s in measured if "heap" in s), default=None),
              "first": first, "last": last, "faults": faults}
    if mode and (faults or result["ready_fraction"] < 0.5 or result["slot_coverage_mean"] < 0.7):
        result["valid_input"] = False
    else:
        result["valid_input"] = True
    if not ble_scan and last["adverts"] != first["adverts"]:
        raise RuntimeError("No-scan control received BLE callbacks; condition is invalid")
    print("CSI_REFERENCE_PILOT " + json.dumps(result, sort_keys=True), flush=True)
    return result


def restart_phase(ser, ip, expected, mode, until, ble_scan):
    post(ip, "/wifi/hardware", {"csi_reference_mode": str(mode),
         "csi_reference_arm": "1" if mode else "0", "csi_reference_until": str(until),
         "csi_reference_ble_scan": "1" if ble_scan else "0"})
    post(ip, "/restart", {})
    ip = wait_boot(ser)
    verify_build(ip, expected)
    return ip


def hardware(ip):
    conn = http.client.HTTPConnection(ip, timeout=5)
    try:
        conn.request("GET", "/wifi/hardware")
        response = conn.getresponse()
        data = json.loads(response.read())
        if response.status != 200:
            raise RuntimeError(f"Hardware metadata HTTP {response.status}")
        return data["values"]
    finally:
        conn.close()


def next_reference(ser, timeout, flood_ready):
    stop = time.monotonic() + timeout
    while time.monotonic() < stop:
        if flood_ready and not Path(flood_ready).exists():
            raise RuntimeError("Controlled flood stopped during safety probe")
        line = ANSI.sub("", ser.readline().decode("utf-8", "replace").strip())
        if "panic" in line.lower() or line.startswith("CSIREF fault"):
            raise RuntimeError(line)
        if line.startswith("CSIREF {"):
            return json.loads(line[7:])
    raise RuntimeError("No reference telemetry during safety probe")


def wait_detector(ser, detector, flood_ready, timeout=45):
    stop = time.monotonic() + timeout
    while time.monotonic() < stop:
        sample = next_reference(ser, max(1, stop - time.monotonic()), flood_ready)
        if sample.get("detector") == detector:
            return sample
    raise RuntimeError(f"Safety probe never observed detector {detector}")


def safety_checks(ser, ip, args, save):
    # These checks stay inside the pipeline's device flock. They establish
    # arming/expiry behavior, independently of the detector's input readiness.
    deadline = int(time.time() + 180)
    ip = restart_phase(ser, ip, args.expected_version, 2, deadline, False)
    active = wait_detector(ser, "high_accuracy", args.flood_ready)
    post(ip, "/restart", {})
    ip = wait_boot(ser)
    verify_build(ip, args.expected_version)
    off = wait_detector(ser, "off", args.flood_ready)
    values = hardware(ip)
    if (values.get("csi_reference_arm") is not False
            or str(values.get("csi_reference_mode")) != "2"
            or str(values.get("csi_reference_until")) != str(deadline)
            or time.time() >= deadline):
        raise RuntimeError("One-shot consumption not proven before the saved deadline")
    result = {"check": "one_shot_reboot", "passed": True,
              "saved_deadline": deadline, "active": active, "after_reboot": off}
    print("CSI_REFERENCE_SAFETY " + json.dumps(result, sort_keys=True), flush=True)
    save(result)

    deadline = int(time.time() + 90)
    ip = restart_phase(ser, ip, args.expected_version, 2, deadline, False)
    active = wait_detector(ser, "high_accuracy", args.flood_ready)
    last_active = active
    stop = time.monotonic() + max(0, deadline - time.time()) + 40
    while time.monotonic() < stop:
        sample = next_reference(ser, max(1, stop - time.monotonic()), args.flood_ready)
        if sample.get("detector") != "off":
            last_active = sample
            continue
        if (time.time() < deadline - 2 or not sample.get("software_reset")
                or sample["t_us"] >= last_active["t_us"]):
            raise RuntimeError("Unexpected disarm/reboot before the expiry proof")
        off = sample
        break
    else:
        raise RuntimeError("Persistent expiry did not reboot into detector-off state")
    verify_build(ip, args.expected_version)
    values = hardware(ip)
    if (values.get("csi_reference_arm") is not False
            or str(values.get("csi_reference_until")) != str(deadline)):
        raise RuntimeError("Expiry did not preserve consumed arming and saved deadline")
    # Repeated expiry reboots must not masquerade as a successful disarm.
    previous = off
    stop = time.monotonic() + 15
    while time.monotonic() < stop:
        sample = next_reference(ser, max(1, stop - time.monotonic()), args.flood_ready)
        if sample.get("detector") != "off" or sample["t_us"] <= previous["t_us"]:
            raise RuntimeError("Sensing resumed or the expired node rebooted repeatedly")
        previous = sample
    result = {"check": "persistent_expiry", "passed": True, "saved_deadline": deadline,
              "active": active, "last_active": last_active,
              "after_expiry": off, "stable_off": previous}
    print("CSI_REFERENCE_SAFETY " + json.dumps(result, sort_keys=True), flush=True)
    save(result)
    return ip


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--port", required=True)
    parser.add_argument("--seconds", type=int, default=120)
    parser.add_argument("--out", required=True)
    parser.add_argument("--expected-version", required=True)
    parser.add_argument("--flood-ready", help="Required live marker throughout each controlled measurement")
    parser.add_argument("--no-scan-control", action="store_true",
                        help="Also measure both detectors with BLE scanning explicitly disabled")
    parser.add_argument("--safety-checks", action="store_true",
                        help="Verify one-shot reboot and automatic persistent expiry under the same lock")
    args = parser.parse_args()
    results = []
    safety = []

    def save():
        path = Path(args.out)
        temporary = path.with_suffix(path.suffix + ".tmp")
        temporary.write_text(json.dumps({"phases": results, "safety": safety}, indent=2))
        temporary.replace(path)

    def save_safety(result):
        safety.append(result)
        save()

    save()
    with serial.Serial(args.port, 115200, timeout=1) as ser:
        ser.dtr = False
        ser.rts = True
        time.sleep(0.1)
        ser.rts = False
        ip = wait_boot(ser)
        verify_build(ip, args.expected_version)
        post(ip, "/wifi", {"auto_update": "0"})
        try:
            phases = [(mode, True) for mode in (0, 1, 2, 0)]
            if args.no_scan_control:
                phases.extend([(1, False), (2, False)])
            for mode, ble_scan in phases:
                ip = restart_phase(ser, ip, args.expected_version, mode,
                                   int(time.time() + 900), ble_scan)
                values = hardware(ip)
                if values.get("csi_reference_ble_scan") is not ble_scan:
                    raise RuntimeError("Saved BLE scanning condition does not match requested phase")
                results.append(measure(ser, args.seconds, mode, args.flood_ready, ble_scan))
                save()  # Preserve completed phases even if a later phase fails.
            if args.safety_checks:
                ip = safety_checks(ser, ip, args, save_safety)
        finally:
            post(ip, "/wifi/hardware", {"csi_reference_mode": "0", "csi_reference_arm": "0",
                 "csi_reference_until": "0", "csi_reference_ble_scan": "1"})
            post(ip, "/restart", {})
    coexistence = [result for result in results if result["ble_scan"]]
    if not all(result["valid_input"] for result in coexistence):
        raise SystemExit("Reference acquisition/readiness failed; do not score this as detector accuracy")


if __name__ == "__main__":
    main()
