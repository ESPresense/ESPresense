#!/usr/bin/env python3
"""Bounded, resumable TCP/HTTP recorder; contains no firmware mutation commands."""
import argparse
import asyncio
from datetime import datetime, timezone
import fcntl
import gzip
import json
import os
from pathlib import Path
import re
import shutil
import time
import urllib.request

ANSI = re.compile(r"\x1b\[[0-9;]*m")
FINGERPRINT = re.compile(r"^\d+ (New|Del) ")


class Recorder:
    def __init__(self, args):
        self.args = args
        self.out = Path(args.out)
        self.out.mkdir(parents=True, exist_ok=True, mode=0o700)
        self.start = time.time()
        self.written = 0
        self.writers = {}
        self.last_packet = {}
        self.last_status = 0
        self.stop_reason = "deadline"
        self.lock = (self.out / "recorder.lock").open("a")
        fcntl.flock(self.lock, fcntl.LOCK_EX | fcntl.LOCK_NB)

    def active(self):
        return time.time() < self.args.until and self.stop_reason == "deadline"

    def record(self, node, source, data):
        now = time.time()
        bucket = int(now // 600)
        existing = self.writers.get(node)
        if existing is None or existing[0] != bucket:
            if existing:
                existing[1].close()
                existing[2].rename(existing[2].with_suffix(""))
            path = self.out / f"{node}-{bucket}-{os.getpid()}.jsonl.gz.partial"
            writer = gzip.open(path, "at", encoding="utf-8")
            self.writers[node] = (bucket, writer, path)
        writer = self.writers[node][1]
        event = {"observed_at": datetime.fromtimestamp(now, timezone.utc).isoformat(),
                 "node": node, "source": source, "data": data}
        writer.write(json.dumps(event, separators=(",", ":")) + "\n")
        self.written += 1
        if source == "reference":
            self.last_packet[node] = now

    def status(self, done=False):
        for _, writer, _ in self.writers.values():
            writer.flush()
        size = sum(p.stat().st_size for p in self.out.glob("*.gz*"))
        free = shutil.disk_usage(self.out).free
        if size >= self.args.max_bytes or free < 256 * 1024 * 1024:
            self.stop_reason = "disk_limit"
        state = {"pid": os.getpid(), "started_at": self.start, "checked_at": time.time(),
                 "until": self.args.until, "records": self.written, "bytes_on_disk": size,
                 "last_reference_at": self.last_packet, "done": done,
                 "stop_reason": self.stop_reason if done or not self.active() else None}
        temporary = self.out / "status.json.tmp"
        temporary.write_text(json.dumps(state, indent=2) + "\n")
        temporary.replace(self.out / "status.json")

    async def tcp(self, node, ip):
        while self.active():
            writer = None
            try:
                reader, writer = await asyncio.wait_for(asyncio.open_connection(ip, 6053), 10)
                self.record(node, "connection", {"state": "connected"})
                counts = {"New": 0, "Del": 0}
                last_proxy = time.monotonic()
                last_line = time.monotonic()
                while self.active():
                    try:
                        raw = await asyncio.wait_for(reader.readline(), 10)
                    except asyncio.TimeoutError:
                        self.record(node, "gap", {"reason": "no TCP line for 10s"})
                        # A reset can leave a receive-only socket half open:
                        # neither peer sends anything that would elicit a RST.
                        # Renew a silent mirror so post-reboot data is captured.
                        if time.monotonic() - last_line >= 30:
                            raise ConnectionError("TCP mirror silent for 30s; reconnecting")
                        continue
                    if not raw:
                        raise ConnectionError("TCP stream closed")
                    last_line = time.monotonic()
                    line = ANSI.sub("", raw.decode("utf-8", "replace").strip())
                    match = FINGERPRINT.match(line)
                    if match:
                        counts[match.group(1)] += 1
                    elif line.startswith("CSIREF {"):
                        try:
                            self.record(node, "reference", json.loads(line[7:]))
                        except json.JSONDecodeError:
                            self.record(node, "invalid_reference", {"line": line})
                    elif line:
                        self.record(node, "log", {"line": line})
                    if time.monotonic() - last_proxy >= 5:
                        self.record(node, "fingerprint_proxy", {**counts,
                                    "seconds": time.monotonic() - last_proxy})
                        counts = {"New": 0, "Del": 0}
                        last_proxy = time.monotonic()
            except (OSError, asyncio.TimeoutError, ValueError) as error:
                self.record(node, "connection", {"state": "disconnected", "reason": str(error)})
                await asyncio.sleep(min(5, max(0, self.args.until - time.time())))
            finally:
                if writer:
                    writer.close()
                    try:
                        await writer.wait_closed()
                    except OSError:
                        pass

    @staticmethod
    def fetch(ip, path):
        with urllib.request.urlopen(f"http://{ip}{path}", timeout=4) as response:
            return json.load(response)

    async def telemetry(self, node, ip):
        cycle = 0
        while self.active():
            for path in (["/json/tele", "/json"] if cycle % 12 == 0 else ["/json/tele"]):
                try:
                    data = await asyncio.to_thread(self.fetch, ip, path)
                    self.record(node, path, data)
                except (OSError, ValueError) as error:
                    self.record(node, "http_gap", {"path": path, "reason": str(error)})
            cycle += 1
            await asyncio.sleep(min(5, max(0, self.args.until - time.time())))

    async def run(self, nodes):
        tasks = [asyncio.create_task(fn(name, ip)) for name, ip in nodes
                 for fn in (self.tcp, self.telemetry)]
        try:
            while self.active():
                self.status()
                for task in tasks:
                    if task.done():
                        task.result()  # A dead recorder must fail visibly, not claim coverage.
                        raise RuntimeError("A recording task ended unexpectedly")
                await asyncio.sleep(min(5, max(0, self.args.until - time.time())))
        except BaseException:
            self.stop_reason = "failed"
            raise
        finally:
            for task in tasks:
                task.cancel()
            await asyncio.gather(*tasks, return_exceptions=True)
            self.status(done=True)
            for _, writer, path in self.writers.values():
                writer.close()
                path.rename(path.with_suffix(""))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--node", action="append", required=True, help="Explicit name=IP; no discovery or scans")
    parser.add_argument("--until", type=float, required=True, help="Absolute UTC Unix stop time")
    parser.add_argument("--out", required=True)
    parser.add_argument("--max-bytes", type=int, default=2 * 1024**3)
    args = parser.parse_args()
    nodes = []
    for item in args.node:
        name, ip = item.split("=", 1)
        if not re.fullmatch(r"[a-zA-Z0-9_-]+", name):
            parser.error("Node names must be safe filename components")
        import ipaddress
        ipaddress.ip_address(ip)
        nodes.append((name, ip))
    if len(dict(nodes)) != len(nodes):
        parser.error("Duplicate node names")
    if not time.time() < args.until <= time.time() + 48 * 3600:
        parser.error("Deadline must be in the next 48 hours")
    os.umask(0o077)
    asyncio.run(Recorder(args).run(nodes))


if __name__ == "__main__":
    main()
