"""Unit tests for the manifest-check schema validator.

Network-touching paths are exercised in CI by running the script against the
real manifest endpoint; this file only covers the offline parser/schema logic
so it can run as a fast PR-time check without external dependencies.
"""
from __future__ import annotations

import unittest
from pathlib import Path
import sys

sys.path.insert(0, str(Path(__file__).parent))
from check_manifest import parse_targets, validate_schema  # noqa: E402


class ValidateSchemaTests(unittest.TestCase):
    def _good(self) -> dict:
        return {
            "name": "ESPresense vX (esp32)",
            "version": "vX",
            "new_install_prompt_erase": True,
            "builds": [
                {
                    "chipFamily": "ESP32",
                    "parts": [
                        {"path": "/static/esp32/bootloader.bin", "offset": 4096},
                        {"path": "download/vX/esp32.bin", "offset": 65536},
                    ],
                }
            ],
        }

    def test_valid(self) -> None:
        self.assertIsNone(validate_schema(self._good()))

    def test_missing_top_key(self) -> None:
        for key in ("name", "version", "builds"):
            payload = self._good()
            del payload[key]
            with self.subTest(key=key):
                err = validate_schema(payload)
                self.assertIsNotNone(err)
                self.assertIn(key, err)

    def test_empty_builds(self) -> None:
        payload = self._good()
        payload["builds"] = []
        self.assertIn("non-empty", validate_schema(payload) or "")

    def test_part_missing_offset(self) -> None:
        payload = self._good()
        del payload["builds"][0]["parts"][0]["offset"]
        err = validate_schema(payload)
        self.assertIsNotNone(err)
        self.assertIn("offset", err)

    def test_build_missing_parts(self) -> None:
        payload = self._good()
        del payload["builds"][0]["parts"]
        err = validate_schema(payload)
        self.assertIsNotNone(err)
        self.assertIn("parts", err)


class TargetsTests(unittest.TestCase):
    def test_parse(self, tmpfile=None) -> None:
        from tempfile import NamedTemporaryFile
        with NamedTemporaryFile("w", suffix=".txt", delete=False) as fh:
            fh.write("# header\nlatest,esp32\n  v4.0.6, m5atom  # legacy\n\n")
            path = Path(fh.name)
        try:
            targets = parse_targets(path)
            self.assertEqual(len(targets), 2)
            self.assertEqual((targets[0].version, targets[0].flavor), ("latest", "esp32"))
            self.assertEqual((targets[1].version, targets[1].flavor), ("v4.0.6", "m5atom"))
        finally:
            path.unlink()


if __name__ == "__main__":
    unittest.main()
