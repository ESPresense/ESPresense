#!/usr/bin/env python3
"""Controlled-load C3 pilot on the already-provisioned HIL board, never house nodes."""
import argparse
import contextlib
import hashlib
import http.client
import json
import queue
import re
import sys
import threading
import time
import urllib.parse
from pathlib import Path
import serial
from external_feed import ExternalFeed

ANSI = re.compile(r"\x1b\[[0-9;]*m")
IP = re.compile(r"IP address:\s*(\d+\.\d+\.\d+\.\d+)")


class SerialLines:
    """A serial read timeout ends a read, not necessarily a protocol line."""

    def __init__(self, port):
        self.port = port
        self.pending = bytearray()

    def readline(self):
        if b"\n" not in self.pending:
            self.pending.extend(self.port.readline())
        if len(self.pending) > 8192:
            raise RuntimeError("Serial line exceeded bounded 8192-byte buffer")
        newline = self.pending.find(b"\n")
        if newline < 0:
            return b""  # Keep the fragment through this timeout and phase changes.
        result = bytes(self.pending[:newline + 1])
        del self.pending[:newline + 1]
        return result


class SerialCapture:
    """Drain the UART during HTTP requests, rather than overflowing its RX buffer."""

    def __init__(self, port, max_lines=8192):
        self.reader = SerialLines(port)
        self.lines = queue.Queue(maxsize=max_lines)
        self.stopping = threading.Event()
        self.failed = threading.Event()
        self.error = None
        self.thread = threading.Thread(target=self._capture, daemon=True)

    def __enter__(self):
        self.thread.start()
        return self

    def _capture(self):
        try:
            while not self.stopping.is_set():
                line = self.reader.readline()
                if line:
                    try:
                        self.lines.put_nowait(line)
                    except queue.Full as error:
                        raise RuntimeError("Serial capture queue overflow; trial invalid") from error
        except Exception as error:
            self.error = error
            self.failed.set()

    def readline(self):
        if self.failed.is_set():
            raise RuntimeError(f"Serial capture failed: {self.error}") from self.error
        try:
            return self.lines.get(timeout=1)
        except queue.Empty:
            return b""

    def __exit__(self, *args):
        self.stopping.set()
        self.thread.join(timeout=2)
        if self.thread.is_alive():
            raise RuntimeError("Serial capture did not stop within its bounded read timeout")


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


def measure(ser, seconds, mode, flood_ready=None, ble_scan=True, traffic_source=None, scan_profile=None):
    samples = []
    faults = []
    tainted_after = set()
    malformed = 0
    stop = time.monotonic() + seconds
    while time.monotonic() < stop:
        if flood_ready and not Path(flood_ready).exists():
            raise RuntimeError("Controlled BLE flood stopped during measurement")
        line = ANSI.sub("", ser.readline().decode("utf-8", "replace").strip())
        if line.startswith("CSIREF {"):
            try:
                sample = json.loads(line[7:])
            except json.JSONDecodeError as error:
                malformed += 1
                if samples:
                    tainted_after.add(samples[-1]["t_us"])
                # Preserve a diagnostic without exporting names/MACs if another
                # log writer interleaved text. Every such phase remains invalid.
                text = line[7:][max(0, error.pos - 35):error.pos + 45]
                shape = "".join("0" if c.isdigit() else "a" if c.isalnum()
                                else c if c in '{}[]":,._- ' else "?" for c in text)
                fault = {"kind": "malformed_reference", "length": len(line),
                         "position": error.pos, "sha256": hashlib.sha256(line.encode()).hexdigest(),
                         "redacted_shape": shape}
                if malformed <= 15:
                    faults.append(fault)
                    print("CSI_REFERENCE_TRANSPORT_FAULT " + json.dumps(fault, sort_keys=True), flush=True)
                continue
            if traffic_source is not None:
                expected = "external_host" if traffic_source == "external" else "ping"
                if sample.get("source") != expected or sample.get("ble_scan") is not ble_scan:
                    raise RuntimeError("Live traffic source/scanning condition differs from requested phase")
            if scan_profile is not None:
                verify_scan_condition(sample, ble_scan, scan_profile, bool(mode or scan_profile))
            samples.append(sample)
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
    def observed(sample, dt):
        return 0 < dt <= gap_limit and sample["t_us"] not in tainted_after
    ready_seconds = sum(dt for sample, dt in intervals if observed(sample, dt) and sample.get("ready"))
    unknown_seconds = sum(dt for sample, dt in intervals if not observed(sample, dt))
    generated = [s.get("generated") for s in measured]
    generator_epoch_valid = (all(value is not None for value in generated)
                             and all(b >= a for a, b in zip(generated, generated[1:])))
    result = {"mode": mode, "ble_scan": ble_scan, "scan_profile": scan_profile,
              "scan_window_units": first.get("scan_window_units"),
              "scan_interval_units": first.get("scan_interval_units"),
              "condition": "BLE coexistence" if ble_scan else "No-scan diagnostic control",
              "seconds": elapsed,
              "adverts_per_minute": (last["adverts"] - first["adverts"]) * 60 / elapsed,
              "generator_successful_sends_per_second": (
                  (generated[-1] - generated[0]) / elapsed if generator_epoch_valid else None),
              "ready_fraction": ready_seconds / elapsed,
              "unobserved_seconds": unknown_seconds,
              "max_sample_gap_seconds": max(dt for _, dt in intervals),
              "slot_coverage_mean": (sum(dt * s.get("slots", 0) / max(1, s.get("window_slots", 0))
                                         for s, dt in intervals if observed(s, dt)) / elapsed),
              "coverage_method": "time weighted; telemetry gaps remain unknown",
              "heap_min": min((s["heap"] for s in measured if "heap" in s), default=None),
              "first": first, "last": last, "faults": faults,
              "malformed_reference_records": malformed}
    if faults or (mode and (result["ready_fraction"] < 0.5 or result["slot_coverage_mean"] < 0.7)):
        result["valid_input"] = False
    else:
        result["valid_input"] = True
    if not ble_scan and last["adverts"] != first["adverts"]:
        raise RuntimeError("No-scan control received BLE callbacks; condition is invalid")
    print("CSI_REFERENCE_PILOT " + json.dumps(result, sort_keys=True), flush=True)
    return result


def restart_phase(ser, ip, expected, mode, until, ble_scan, external_host=False, scan_profile=0):
    post(ip, "/wifi/hardware", {"csi_reference_mode": str(mode),
         "csi_reference_arm": "1" if mode or scan_profile else "0", "csi_reference_until": str(until),
         "csi_reference_ble_scan": "1" if ble_scan else "0",
         "csi_reference_external_host": "1" if external_host else "0",
         "csi_reference_scan_profile": str(scan_profile)})
    return restart_and_verify(ser, ip, expected)


def restart_and_verify(ser, ip, expected):
    try:
        post(ip, "/restart", {})
    except (OSError, http.client.HTTPException):
        # Do not repeat a possibly delivered reboot: the one-shot arm could
        # already be consumed. Only a fresh serial boot and exact build pass.
        print("BENCH restart response interrupted; requiring fresh boot proof", flush=True)
    ip = wait_boot(ser)
    verify_build(ip, expected)
    return ip


def hardware(ip):
    stop = time.monotonic() + 45
    last_error = None
    while time.monotonic() < stop:
        conn = http.client.HTTPConnection(ip, timeout=3)
        try:
            conn.request("GET", "/wifi/hardware")
            response = conn.getresponse()
            data = json.loads(response.read())
            if response.status != 200:
                raise OSError(f"Hardware metadata HTTP {response.status}")
            if not isinstance(data.get("values"), dict):
                raise RuntimeError("Unexpected hardware metadata schema")
            return data["values"]
        except (OSError, http.client.HTTPException, ValueError) as error:
            last_error = error
            time.sleep(0.5)
        finally:
            conn.close()
    raise RuntimeError(f"Hardware metadata unavailable after bounded retries: {last_error}")


def requested_phases(no_scan_control, phase_start, safety):
    phases = [(mode, True) for mode in (0, 1, 2, 0)]
    if no_scan_control:
        phases.extend([(1, False), (2, False)])
    if phase_start < 0 or phase_start > len(phases) or (phase_start == len(phases) and not safety):
        raise ValueError("Phase start must select an existing phase or safety-only continuation")
    return phases[phase_start:]


def scan_window_phases(no_scan_control, phase_start, traffic_source):
    if no_scan_control or phase_start or traffic_source != "ping":
        raise ValueError("Scan-window sweep requires a complete PING run without scan-off controls")
    return [(0, True, 0)] + [(mode, True, profile) for profile in (1, 2)
                            for mode in (0, 1, 2, 0)] + [(0, True, 0)]


def verify_scan_condition(sample, ble_scan, profile, trial_active):
    window = (128, 32, 16)[profile] if trial_active else 128
    if (sample.get("ble_scan") is not ble_scan or sample.get("scan_profile") != profile
            or sample.get("scan_interval_units") != 128
            or sample.get("scan_requested_units") != window
            or sample.get("trial_active") is not trial_active
            or (ble_scan and sample.get("scan_window_units") != window)):
        raise RuntimeError("Live scan-window condition differs from the armed phase")


def wait_scan_condition(ser, detector, ble_scan, profile, trial_active, flood_ready, timeout=45):
    # Settings and initial boot output precede NTP, durable arm consumption, and
    # asynchronous controller scan cancellation/restart. Do not measure them.
    stop = time.monotonic() + timeout
    while time.monotonic() < stop:
        sample = next_reference(ser, max(1, stop - time.monotonic()), flood_ready)
        if sample.get("detector") != detector:
            continue
        try:
            verify_scan_condition(sample, ble_scan, profile, trial_active)
        except RuntimeError:
            continue
        return sample
    raise RuntimeError("Controller did not apply the requested scan-window condition")


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
    profile = 2 if args.scan_window_sweep else 0
    scanning = args.scan_window_sweep
    deadline = int(time.time() + 180)
    ip = restart_phase(ser, ip, args.expected_version, 2, deadline, scanning,
                       args.traffic_source == "external", profile)
    active = wait_scan_condition(ser, "high_accuracy", scanning, profile, True, args.flood_ready)
    ip = restart_and_verify(ser, ip, args.expected_version)
    off = wait_scan_condition(ser, "off", scanning, profile, False, args.flood_ready)
    values = hardware(ip)
    if (values.get("csi_reference_arm") is not False
            or str(values.get("csi_reference_mode")) != "2"
            or str(values.get("csi_reference_until")) != str(deadline)
            or str(values.get("csi_reference_scan_profile")) != str(profile)
            or time.time() >= deadline):
        raise RuntimeError("One-shot consumption not proven before the saved deadline")
    result = {"check": "one_shot_reboot", "passed": True,
              "saved_deadline": deadline, "active": active, "after_reboot": off}
    print("CSI_REFERENCE_SAFETY " + json.dumps(result, sort_keys=True), flush=True)
    save(result)

    deadline = int(time.time() + 90)
    ip = restart_phase(ser, ip, args.expected_version, 2, deadline, scanning,
                       args.traffic_source == "external", profile)
    active = wait_scan_condition(ser, "high_accuracy", scanning, profile, True, args.flood_ready)
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
        off = wait_scan_condition(ser, "off", scanning, profile, False, args.flood_ready)
        break
    else:
        raise RuntimeError("Persistent expiry did not reboot into detector-off state")
    verify_build(ip, args.expected_version)
    values = hardware(ip)
    if (values.get("csi_reference_arm") is not False
            or str(values.get("csi_reference_until")) != str(deadline)
            or str(values.get("csi_reference_scan_profile")) != str(profile)):
        raise RuntimeError("Expiry did not preserve consumed arming and saved deadline")
    # Repeated expiry reboots must not masquerade as a successful disarm.
    previous = off
    stop = time.monotonic() + 15
    while time.monotonic() < stop:
        sample = next_reference(ser, max(1, stop - time.monotonic()), args.flood_ready)
        if sample.get("detector") != "off" or sample["t_us"] <= previous["t_us"]:
            raise RuntimeError("Sensing resumed or the expired node rebooted repeatedly")
        verify_scan_condition(sample, scanning, profile, False)
        previous = sample
    result = {"check": "persistent_expiry", "passed": True, "saved_deadline": deadline,
              "active": active, "last_active": last_active,
              "after_expiry": off, "stable_off": previous}
    print("CSI_REFERENCE_SAFETY " + json.dumps(result, sort_keys=True), flush=True)
    save(result)
    if args.scan_window_sweep:
        # Critical extra case: no SDK is running, but reduced BLE scanning must
        # still expire without the host, reboot once, and return to full scan.
        deadline = int(time.time() + 60)
        ip = restart_phase(ser, ip, args.expected_version, 0, deadline, True, False, profile)
        active = wait_scan_condition(ser, "off", True, profile, True, args.flood_ready)
        last_active = active
        stop = time.monotonic() + max(0, deadline - time.time()) + 45
        while time.monotonic() < stop:
            sample = next_reference(ser, max(1, stop - time.monotonic()), args.flood_ready)
            if sample.get("trial_active"):
                verify_scan_condition(sample, True, profile, True)
                last_active = sample
                continue
            if (time.time() < deadline - 2 or not sample.get("software_reset")
                    or sample["t_us"] >= last_active["t_us"]):
                raise RuntimeError("BLE-only scan profile did not expire with a fresh software boot")
            # Controller may not have started yet at the first postboot report.
            off = wait_scan_condition(ser, "off", True, profile, False, args.flood_ready)
            break
        else:
            raise RuntimeError("BLE-only reduced scan did not expire")
        verify_build(ip, args.expected_version)
        values = hardware(ip)
        if (values.get("csi_reference_arm") is not False
                or str(values.get("csi_reference_mode")) != "0"
                or str(values.get("csi_reference_until")) != str(deadline)
                or str(values.get("csi_reference_scan_profile")) != str(profile)):
            raise RuntimeError("BLE-only expiry did not retain consumed arm and saved settings")
        previous = off
        stop = time.monotonic() + 15
        while time.monotonic() < stop:
            sample = next_reference(ser, max(1, stop - time.monotonic()), args.flood_ready)
            verify_scan_condition(sample, True, profile, False)
            if sample.get("detector") != "off" or sample["t_us"] <= previous["t_us"]:
                raise RuntimeError("BLE-only expiry rebooted repeatedly or resumed sensing")
            previous = sample
        result = {"check": "ble_only_profile_expiry", "passed": True, "saved_deadline": deadline,
                  "active": active, "last_active": last_active,
                  "after_expiry": off, "stable_off": previous}
        print("CSI_REFERENCE_SAFETY " + json.dumps(result, sort_keys=True), flush=True)
        save(result)
    return ip


def cleanup(ser, ip, expected):
    post(ip, "/wifi/hardware", {"csi_reference_mode": "0", "csi_reference_arm": "0",
         "csi_reference_until": "0", "csi_reference_ble_scan": "1", "csi_reference_external_host": "0",
         "csi_reference_scan_profile": "0"})
    ip = restart_and_verify(ser, ip, expected)
    values = hardware(ip)
    if (str(values.get("csi_reference_mode")) != "0"
            or values.get("csi_reference_arm") is not False
            or str(values.get("csi_reference_until")) != "0"
            or values.get("csi_reference_ble_scan") is not True
            or values.get("csi_reference_external_host") is not False
            or str(values.get("csi_reference_scan_profile")) != "0"):
        raise RuntimeError("Bench cleanup settings were not restored")
    wait_scan_condition(ser, "off", True, 0, False, None)
    print("CSI_REFERENCE_CLEANUP verified off, arm=false, until=0, scanning=true, external=false, full_window=128", flush=True)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--port", required=True)
    parser.add_argument("--seconds", type=int, default=120)
    parser.add_argument("--out", required=True)
    parser.add_argument("--expected-version", required=True)
    parser.add_argument("--flood-ready", help="Required live marker throughout each controlled measurement")
    parser.add_argument("--no-scan-control", action="store_true",
                        help="Also measure both detectors with BLE scanning explicitly disabled")
    parser.add_argument("--scan-window-sweep", action="store_true",
                        help="Full/quarter/eighth BLE window tradeoff, matched baselines, no SDK changes")
    parser.add_argument("--safety-checks", action="store_true",
                        help="Verify one-shot reboot and automatic persistent expiry under the same lock")
    parser.add_argument("--phase-start", type=int, default=0,
                        help="Skip already preserved phases; a partial run cannot pass the house gate")
    parser.add_argument("--traffic-source", choices=("ping", "external"), default="ping",
                        help="Explicit external-host acquisition diagnostic; target and SDK cadence stay100pps")
    args = parser.parse_args()
    if args.traffic_source == "external" and not 0 < args.seconds <= 290:
        parser.error("External-host phases must fit the sender's300-second hard expiry")
    try:
        phases = (scan_window_phases(args.no_scan_control, args.phase_start, args.traffic_source)
                  if args.scan_window_sweep else [(m, b, 0) for m, b in
                      requested_phases(args.no_scan_control, args.phase_start, args.safety_checks)])
    except ValueError as error:
        parser.error(str(error))
    results = []
    safety = []
    traffic_feeds = []

    def save():
        path = Path(args.out)
        temporary = path.with_suffix(path.suffix + ".tmp")
        temporary.write_text(json.dumps({"phases": results, "safety": safety,
            "source_version": args.expected_version, "phase_start": args.phase_start,
            "traffic_source": args.traffic_source, "scan_window_sweep": args.scan_window_sweep,
            "external_feeds": traffic_feeds}, indent=2))
        temporary.replace(path)

    def save_safety(result):
        safety.append(result)
        save()

    save()
    with serial.Serial(args.port, 115200, timeout=1) as port:
        port.dtr = False
        port.rts = True
        time.sleep(0.1)
        port.rts = False
        with SerialCapture(port) as ser:
            ip = wait_boot(ser)
            verify_build(ip, args.expected_version)
            post(ip, "/wifi", {"auto_update": "0"})
            try:
                print(f"BENCH phase_start={args.phase_start} phases={phases}", flush=True)
                for mode, ble_scan, scan_profile in phases:
                    ip = restart_phase(ser, ip, args.expected_version, mode,
                                       int(time.time() + 900), ble_scan, args.traffic_source == "external", scan_profile)
                    values = hardware(ip)
                    if values.get("csi_reference_ble_scan") is not ble_scan:
                        raise RuntimeError("Saved BLE scanning condition does not match requested phase")
                    if values.get("csi_reference_external_host") is not (args.traffic_source == "external"):
                        raise RuntimeError("Saved traffic source does not match requested phase")
                    if str(values.get("csi_reference_scan_profile")) != str(scan_profile):
                        raise RuntimeError("Saved scan-window profile differs from requested phase")
                    wait_scan_condition(ser, ("off", "lightweight", "high_accuracy")[mode],
                                        ble_scan, scan_profile, bool(mode or scan_profile), args.flood_ready)
                    feed = (ExternalFeed(ip, args.seconds + 10) if mode and args.traffic_source == "external"
                            else contextlib.nullcontext(None))
                    with feed as sender:
                        result = measure(ser, args.seconds, mode, args.flood_ready, ble_scan, args.traffic_source, scan_profile)
                    if sender is not None:
                        result["external_feed"] = sender.stats()
                        traffic_feeds.append({"mode": mode, "ble_scan": ble_scan, **sender.stats()})
                        print("CSI_REFERENCE_EXTERNAL_FEED " + json.dumps(traffic_feeds[-1], sort_keys=True), flush=True)
                    results.append(result)
                    save()  # Preserve completed phases even if a later phase fails.
                if args.safety_checks:
                    feed = (ExternalFeed(ip, 300) if args.traffic_source == "external"
                            else contextlib.nullcontext(None))
                    with feed as sender:
                        ip = safety_checks(ser, ip, args, save_safety)
                    if sender is not None:
                        traffic_feeds.append({"phase": "safety", **sender.stats()})
                        print("CSI_REFERENCE_EXTERNAL_FEED " + json.dumps(traffic_feeds[-1], sort_keys=True), flush=True)
                        save()
            finally:
                original_error = sys.exc_info()[0] is not None
                try:
                    cleanup(ser, ip, args.expected_version)
                except Exception as error:
                    print(f"BENCH cleanup unverified: {type(error).__name__}", flush=True)
                    if not original_error:
                        raise
    if not all(result["valid_input"] for result in results):
        raise SystemExit("Reference acquisition/readiness failed; do not score this as detector accuracy")
    if args.phase_start:
        print("CSI_REFERENCE_GATE not evaluated: partial diagnostic continuation", flush=True)


if __name__ == "__main__":
    main()
