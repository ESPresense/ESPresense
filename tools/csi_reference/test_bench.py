"""Regression checks for measurement validity and post-boot identity guards."""
import contextlib
import io
import json
import unittest
from unittest.mock import Mock, patch

import bench


class TimedSerial:
    def __init__(self, samples, interval=1.0):
        self.samples = iter(samples)
        self.interval = interval
        self.now = 0.0

    def readline(self):
        self.now += self.interval
        return ("CSIREF " + json.dumps(next(self.samples)) + "\n").encode()


class BenchValidityTests(unittest.TestCase):
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
