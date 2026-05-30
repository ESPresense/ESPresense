#!/usr/bin/env python3
"""Probe the public ESP Web Tools manifest endpoints for ESPresense releases.

Reproduces issue #2225 ("Failed to download manifest"), which surfaces when
``https://espresense.com/releases/{version}.json?flavor={flavor}`` returns 500
for a given version/flavor combination. The web installer surfaces that
upstream failure as a generic "Failed to download manifest" toast with no
diagnostic in the UI.

This script fetches each (version, flavor) pair listed in ``targets.txt``,
asserts the response is HTTP 200 with parseable JSON that conforms to the
ESP Web Tools manifest schema, and HEADs each ``builds[].parts[].path`` to
confirm the binaries are reachable. A small retry budget filters single
transient 500s; persistent failures still fail the run.

The script is stdlib-only on purpose so it can run unmodified in CI without a
PlatformIO/Python virtualenv setup step.
"""
from __future__ import annotations

import argparse
import json
import sys
import time
import urllib.error
import urllib.parse
import urllib.request
from dataclasses import dataclass, field
from pathlib import Path

DEFAULT_BASE = "https://espresense.com"
DEFAULT_TARGETS = Path(__file__).with_name("targets.txt")
USER_AGENT = "ESPresense-manifest-check/1 (+https://github.com/ESPresense/ESPresense)"

REQUIRED_TOP_KEYS = ("name", "version", "builds")
REQUIRED_PART_KEYS = ("path", "offset")


@dataclass
class Target:
    version: str
    flavor: str

    def manifest_url(self, base: str) -> str:
        query = urllib.parse.urlencode({"flavor": self.flavor})
        return f"{base}/releases/{self.version}.json?{query}"


@dataclass
class Failure:
    target: Target
    stage: str
    detail: str


@dataclass
class Report:
    failures: list[Failure] = field(default_factory=list)
    checked: int = 0

    def fail(self, target: Target, stage: str, detail: str) -> None:
        self.failures.append(Failure(target, stage, detail))


def parse_targets(path: Path) -> list[Target]:
    targets: list[Target] = []
    for raw in path.read_text().splitlines():
        line = raw.split("#", 1)[0].strip()
        if not line:
            continue
        if "," not in line:
            raise SystemExit(f"{path}: malformed line (expected 'version,flavor'): {raw!r}")
        version, flavor = (part.strip() for part in line.split(",", 1))
        if not version or not flavor:
            raise SystemExit(f"{path}: empty version or flavor: {raw!r}")
        targets.append(Target(version=version, flavor=flavor))
    if not targets:
        raise SystemExit(f"{path}: no targets")
    return targets


def http_request(url: str, *, method: str, timeout: float, retries: int, backoff: float) -> tuple[int, bytes, str]:
    last_exc: Exception | None = None
    for attempt in range(retries + 1):
        req = urllib.request.Request(url, method=method, headers={"User-Agent": USER_AGENT})
        try:
            with urllib.request.urlopen(req, timeout=timeout) as resp:
                body = resp.read() if method == "GET" else b""
                return resp.status, body, resp.headers.get("Content-Type", "")
        except urllib.error.HTTPError as exc:
            if exc.code < 500 or attempt == retries:
                body = exc.read() if method == "GET" else b""
                return exc.code, body, exc.headers.get("Content-Type", "") if exc.headers else ""
            last_exc = exc
        except (urllib.error.URLError, TimeoutError) as exc:
            last_exc = exc
            if attempt == retries:
                raise
        time.sleep(backoff * (2 ** attempt))
    if last_exc:
        raise last_exc
    raise RuntimeError("unreachable")


def validate_schema(payload: dict) -> str | None:
    for key in REQUIRED_TOP_KEYS:
        if key not in payload:
            return f"missing top-level key: {key}"
    builds = payload.get("builds")
    if not isinstance(builds, list) or not builds:
        return "builds[] must be a non-empty list"
    for idx, build in enumerate(builds):
        if not isinstance(build, dict):
            return f"builds[{idx}] is not an object"
        if "chipFamily" not in build:
            return f"builds[{idx}] missing chipFamily"
        parts = build.get("parts")
        if not isinstance(parts, list) or not parts:
            return f"builds[{idx}].parts must be a non-empty list"
        for pidx, part in enumerate(parts):
            if not isinstance(part, dict):
                return f"builds[{idx}].parts[{pidx}] is not an object"
            for key in REQUIRED_PART_KEYS:
                if key not in part:
                    return f"builds[{idx}].parts[{pidx}] missing {key}"
    return None


def check_target(
    target: Target, base: str, *, timeout: float, retries: int, backoff: float, check_parts: bool, report: Report
) -> None:
    report.checked += 1
    url = target.manifest_url(base)
    try:
        status, body, content_type = http_request(
            url, method="GET", timeout=timeout, retries=retries, backoff=backoff
        )
    except Exception as exc:  # noqa: BLE001 -- want the message verbatim
        report.fail(target, "fetch", f"{url}: {exc}")
        return

    if status != 200:
        snippet = body[:200].decode("utf-8", errors="replace").strip()
        report.fail(target, "status", f"{url}: HTTP {status} (body: {snippet!r})")
        return

    if "json" not in content_type.lower():
        report.fail(target, "content-type", f"{url}: got {content_type!r}, want application/json")
        return

    try:
        payload = json.loads(body)
    except json.JSONDecodeError as exc:
        snippet = body[:200].decode("utf-8", errors="replace")
        report.fail(target, "parse", f"{url}: {exc.msg} at offset {exc.pos} (body starts: {snippet!r})")
        return

    schema_error = validate_schema(payload)
    if schema_error:
        report.fail(target, "schema", f"{url}: {schema_error}")
        return

    if not check_parts:
        return

    seen_paths: set[str] = set()
    for build in payload["builds"]:
        for part in build["parts"]:
            path = part["path"]
            if path in seen_paths:
                continue
            seen_paths.add(path)
            if path.startswith(("http://", "https://")):
                part_url = path
            else:
                part_url = urllib.parse.urljoin(base + "/", path.lstrip("/"))
            try:
                pstatus, _, _ = http_request(
                    part_url, method="HEAD", timeout=timeout, retries=retries, backoff=backoff
                )
            except Exception as exc:  # noqa: BLE001
                report.fail(target, "part-fetch", f"{part_url}: {exc}")
                continue
            if pstatus not in (200, 301, 302, 307, 308):
                report.fail(target, "part-status", f"{part_url}: HTTP {pstatus}")


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=(__doc__ or '').split("\n", 1)[0])
    parser.add_argument(
        "--base", default=DEFAULT_BASE, help="Manifest host (default: %(default)s)"
    )
    parser.add_argument(
        "--targets",
        type=Path,
        default=DEFAULT_TARGETS,
        help="Path to a 'version,flavor' targets file (default: %(default)s)",
    )
    parser.add_argument(
        "--timeout", type=float, default=15.0, help="Per-request timeout seconds (default: %(default)s)"
    )
    parser.add_argument(
        "--retries", type=int, default=3, help="Retry count for 5xx and connection errors (default: %(default)s)"
    )
    parser.add_argument(
        "--backoff", type=float, default=2.0, help="Initial backoff seconds, exponential (default: %(default)s)"
    )
    parser.add_argument(
        "--no-check-parts", action="store_true", help="Skip the per-binary HEAD validation"
    )
    args = parser.parse_args(argv)

    targets = parse_targets(args.targets)
    report = Report()
    for target in targets:
        check_target(
            target,
            args.base.rstrip("/"),
            timeout=args.timeout,
            retries=args.retries,
            backoff=args.backoff,
            check_parts=not args.no_check_parts,
            report=report,
        )

    print(f"Checked {report.checked} target(s).")
    if not report.failures:
        print("All manifests OK.")
        return 0

    print(f"FAILED: {len(report.failures)} problem(s):", file=sys.stderr)
    for f in report.failures:
        print(f"  [{f.stage}] {f.target.version}/{f.target.flavor}: {f.detail}", file=sys.stderr)
    return 1


if __name__ == "__main__":
    sys.exit(main())
