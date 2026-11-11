"""Regression checks for measurement validity and post-boot identity guards."""
import contextlib
import io
import json
import queue
import threading
import unittest
from unittest.mock import Mock, patch

import bench
import external_feed


class TimedSerial:
    def __init__(self, samples, interval=1.0):
        self.samples = iter(samples)
        self.interval = interval
        self.now = 0.0

    def readline(self):
        self.now += self.interval
        return ("CSIREF " + json.dumps(next(self.samples)) + "\n").encode()


class BenchValidityTests(unittest.TestCase):
    def test_external_phase_rejects_live_ping_output(self):
        ser = Mock()
        ser.readline.return_value = b'CSIREF {"source":"ping","ble_scan":true}\n'
        with self.assertRaisesRegex(RuntimeError, "Live traffic source/scanning condition"):
            bench.measure(ser, 60, 1, ble_scan=True, traffic_source="external")

    def test_external_feed_rejects_other_nodes_and_unbounded_duration(self):
        for ip, seconds in [("192.168.129.169", 120), ("192.168.129.191", 120),
                            (external_feed.BENCH_IP, 301), (external_feed.BENCH_IP, 0)]:
            with self.subTest(ip=ip, seconds=seconds), self.assertRaises(ValueError):
                external_feed.ExternalFeed(ip, seconds)

    def test_external_feed_skips_missed_deadlines_without_catchup_bursts(self):
        deadline, skipped = external_feed.following_deadline(1.0, 1.055)
        self.assertEqual(skipped, 5)
        self.assertAlmostEqual(deadline, 1.06)
        deadline, skipped = external_feed.following_deadline(1.0, 1.001)
        self.assertEqual(skipped, 0)
        self.assertAlmostEqual(deadline, 1.01)

    def test_external_sender_expires_without_consumer_cleanup(self):
        channel = Mock()
        context = Mock()
        context.__enter__ = Mock(return_value=channel)
        context.__exit__ = Mock(return_value=False)
        with patch.object(external_feed.socket, "socket", return_value=context):
            with external_feed.ExternalFeed(external_feed.BENCH_IP, 0.035) as sender:
                sender.thread.join(timeout=1)  # No stop request: its own deadline must end it.
                self.assertFalse(sender.thread.is_alive())
                self.assertGreaterEqual(sender.sent, 1)
                self.assertLessEqual(sender.sent, 4)
        for call in channel.sendto.call_args_list:
            self.assertEqual(call.args, (bytes.fromhex("f09f91bb"), (external_feed.BENCH_IP, 5555)))
        channel.setblocking.assert_called_once_with(False)

    def test_hardware_transport_failure_retries_but_bad_schema_does_not(self):
        first, second = Mock(), Mock()
        first.getresponse.side_effect = TimeoutError("server temporarily busy")
        second.getresponse.return_value = Mock(status=200)
        second.getresponse.return_value.read.return_value = b'{"values":{"csi_reference_ble_scan":false}}'
        with patch.object(bench.http.client, "HTTPConnection", side_effect=[first, second]), \
                patch.object(bench.time, "sleep"):
            self.assertEqual(bench.hardware("192.0.2.1"), {"csi_reference_ble_scan": False})
        second.getresponse.return_value.read.return_value = b'{"values":null}'
        with patch.object(bench.http.client, "HTTPConnection", return_value=second) as factory:
            with self.assertRaisesRegex(RuntimeError, "Unexpected hardware metadata schema"):
                bench.hardware("192.0.2.1")
            factory.assert_called_once()

    def test_partial_continuation_runs_only_remaining_control_or_safety(self):
        self.assertEqual(bench.requested_phases(True, 5, True), [(2, False)])
        self.assertEqual(bench.requested_phases(True, 6, True), [])
        self.assertEqual(bench.requested_phases(False, 0, False), [(0, True), (1, True), (2, True), (0, True)])
        for start, safety in [(-1, True), (7, True), (6, False)]:
            with self.assertRaises(ValueError):
                bench.requested_phases(True, start, safety)

    def test_lost_restart_response_requires_boot_and_identity_without_a_second_restart(self):
        with patch.object(bench, "post", side_effect=TimeoutError) as post, \
                patch.object(bench, "wait_boot", return_value="192.0.2.1") as boot, \
                patch.object(bench, "verify_build") as verify, \
                contextlib.redirect_stdout(io.StringIO()):
            self.assertEqual(bench.restart_and_verify("serial", "192.0.2.1", "expected"), "192.0.2.1")
        post.assert_called_once_with("192.0.2.1", "/restart", {})
        boot.assert_called_once_with("serial")
        verify.assert_called_once_with("192.0.2.1", "expected")

    def test_lost_restart_response_without_a_new_boot_fails(self):
        with patch.object(bench, "post", side_effect=TimeoutError), \
                patch.object(bench, "wait_boot", side_effect=RuntimeError("No post-reset IP")), \
                contextlib.redirect_stdout(io.StringIO()):
            with self.assertRaisesRegex(RuntimeError, "No post-reset IP"):
                bench.restart_and_verify("serial", "192.0.2.1", "expected")

    def test_cleanup_rejects_a_saved_scanning_disable(self):
        with patch.object(bench, "post"), \
                patch.object(bench, "restart_and_verify", return_value="192.0.2.1"), \
                patch.object(bench, "hardware", return_value={"csi_reference_mode": "0",
                    "csi_reference_arm": False, "csi_reference_until": "0", "csi_reference_ble_scan": False}):
            with self.assertRaisesRegex(RuntimeError, "cleanup settings were not restored"):
                bench.cleanup("serial", "192.0.2.1", "expected")

    def test_capture_drains_serial_while_the_consumer_is_busy_with_http(self):
        records = [b'CSIREF {"t_us":1}\n', b'CSIREF {"t_us":2}\n']
        drained = threading.Event()
        pending = queue.Queue()
        for record in records:
            pending.put(record)

        def read():
            if pending.empty():
                drained.set()  # The preceding line has reached the capture queue.
            try:
                return pending.get(timeout=0.01)
            except queue.Empty:
                return b""

        port = Mock()
        port.readline.side_effect = read
        with bench.SerialCapture(port) as capture:
            # No consumer reads until the entire producer stream was drained.
            self.assertTrue(drained.wait(timeout=2))
            self.assertEqual([capture.readline(), capture.readline()], records)

    def test_capture_overflow_invalidates_instead_of_silently_dropping_records(self):
        port = Mock()
        port.readline.return_value = b'CSIREF {"t_us":1}\n'
        with bench.SerialCapture(port, max_lines=1) as capture:
            self.assertTrue(capture.failed.wait(timeout=2))
            with self.assertRaisesRegex(RuntimeError, "queue overflow"):
                capture.readline()

    def test_newline_terminated_malformed_record_still_fails(self):
        ser = Mock()
        ser.readline.return_value = b'CSIREF {"t_us":1,"detector":"high_accuracy}\n'
        with self.assertRaises(json.JSONDecodeError):
            bench.measure(ser, 60, 2)

    def test_timeout_fragment_is_not_parsed_as_a_complete_json_record(self):
        record = b'CSIREF {"t_us":23456789,"utc":1794426907,"detector":"high_accuracy"}\n'
        port = Mock()
        port.readline.side_effect = [record[:38], b"", record[38:]]
        reader = bench.SerialLines(port)
        self.assertEqual(reader.readline(), b"")
        self.assertEqual(reader.readline(), b"")
        complete = reader.readline()
        self.assertEqual(complete, record)
        self.assertEqual(json.loads(complete[7:])["detector"], "high_accuracy")

    def test_serial_line_buffer_remains_bounded_without_a_newline(self):
        port = Mock()
        port.readline.return_value = b"x" * 8193
        with self.assertRaisesRegex(RuntimeError, "bounded 8192-byte buffer"):
            bench.SerialLines(port).readline()

    def response(self, version="expected", firmware="esp32c3-csiref"):
        response = Mock(status=200)
        response.read.return_value = json.dumps({"ver": version, "firm": firmware}).encode()
        conn = Mock()
        conn.getresponse.return_value = response
        return conn

    def test_transient_boot_transport_failure_is_retried(self):
        first, second = Mock(), self.response()
        first.getresponse.side_effect = TimeoutError("server still starting")
        with patch.object(bench.http.client, "HTTPConnection", side_effect=[first, second]) as factory, \
                patch.object(bench.time, "sleep"), contextlib.redirect_stdout(io.StringIO()):
            bench.verify_build("192.0.2.1", "expected")
        self.assertEqual(factory.call_count, 2)
        first.close.assert_called_once()
        second.close.assert_called_once()

    def test_wrong_firmware_identity_is_never_retried_as_a_boot_delay(self):
        for version, firmware in [("wrong", "esp32c3-csiref"), ("expected", "esp32c3")]:
            with self.subTest(version=version, firmware=firmware), \
                    patch.object(bench.http.client, "HTTPConnection",
                                 return_value=self.response(version, firmware)) as factory:
                with self.assertRaisesRegex(RuntimeError, "Unexpected bench firmware"):
                    bench.verify_build("192.0.2.1", "expected")
                factory.assert_called_once()

    def test_a_reboot_in_the_middle_invalidates_even_recovered_final_counters(self):
        samples = []
        for i in range(60):
            epoch_second = i + 10 if i < 25 else i - 25 + 4
            adverts = (i + 1) * 100 if i < 25 else (i - 25) * 100
            samples.append({"t_us": epoch_second * 1_000_000, "adverts": adverts})
        # Endpoints alone look healthy, which is why every adjacent pair matters.
        self.assertGreater(samples[-1]["t_us"], samples[0]["t_us"])
        self.assertGreater(samples[-1]["adverts"], samples[0]["adverts"])
        ser = TimedSerial(samples)
        with patch.object(bench.time, "monotonic", side_effect=lambda: ser.now):
            with self.assertRaisesRegex(RuntimeError, "Counter epoch changed within"):
                bench.measure(ser, 60, 0)

    def test_nominal_one_hz_off_telemetry_jitter_is_not_all_missing_time(self):
        samples = [{"t_us": 10_000_000 + i * 1_010_000, "adverts": i * 50,
                    "ready": False} for i in range(65)]
        ser = TimedSerial(samples, 1.01)
        with patch.object(bench.time, "monotonic", side_effect=lambda: ser.now), \
                contextlib.redirect_stdout(io.StringIO()):
            result = bench.measure(ser, 65, 0)
        self.assertEqual(result["unobserved_seconds"], 0)
        self.assertTrue(result["valid_input"])

    def test_no_scan_control_with_received_adverts_is_rejected(self):
        samples = [{"t_us": 10_000_000 + i * 1_000_000, "adverts": i * 50,
                    "ready": True, "slots": 100, "window_slots": 100} for i in range(60)]
        ser = TimedSerial(samples)
        with patch.object(bench.time, "monotonic", side_effect=lambda: ser.now):
            with self.assertRaisesRegex(RuntimeError, "No-scan control received BLE callbacks"):
                bench.measure(ser, 60, 1, ble_scan=False)


if __name__ == "__main__":
    unittest.main()
