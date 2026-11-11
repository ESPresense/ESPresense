"""Bounded, unicast-only feed for the pinned SDK's external-host bench control."""
import socket
import threading
import time

BENCH_IP = "192.168.129.243"
PORT = 5555
MARKER = bytes.fromhex("f09f91bb")  # Pinned runtime_sensing_schema.h marker.
INTERVAL = 0.01


def following_deadline(previous, now):
    deadline = previous + INTERVAL
    skipped = max(0, int((now - deadline) / INTERVAL) + 1) if now > deadline else 0
    return deadline + skipped * INTERVAL, skipped


class ExternalFeed:
    def __init__(self, ip, seconds):
        if ip != BENCH_IP:
            raise ValueError("External diagnostic feed is restricted to the C3 bench IP")
        if not 0 < seconds <= 300:
            raise ValueError("External feed must expire within 300 seconds")
        self.seconds = seconds
        self.stopping = threading.Event()
        self.thread = threading.Thread(target=self._run, daemon=True)
        self.sent = self.errors = self.skipped = 0
        self.max_lateness = 0.0
        self.started = self.finished = None
        self.failure = None

    def __enter__(self):
        self.thread.start()
        return self

    def _run(self):
        try:
            with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as channel:
                channel.setblocking(False)
                self.started = time.monotonic()
                end = self.started + self.seconds
                deadline = self.started
                while not self.stopping.is_set():
                    now = time.monotonic()
                    if now >= end:
                        break
                    if self.stopping.wait(max(0, min(deadline, end) - now)):
                        break
                    now = time.monotonic()
                    if now >= end:
                        break
                    self.max_lateness = max(self.max_lateness, now - deadline)
                    try:
                        channel.sendto(MARKER, (BENCH_IP, PORT))
                        self.sent += 1
                    except OSError:
                        self.errors += 1
                    deadline, skipped = following_deadline(deadline, time.monotonic())
                    self.skipped += skipped
        except Exception as error:
            self.failure = error
        finally:
            self.finished = time.monotonic()

    def stats(self):
        elapsed = ((self.finished or time.monotonic()) - self.started
                   if self.started is not None else 0)
        return {"target": BENCH_IP, "port": PORT, "requested_pps": 100,
                "max_seconds": self.seconds, "seconds": elapsed,
                "successful_sends": self.sent, "send_errors": self.errors,
                "skipped_deadlines": self.skipped, "max_lateness_seconds": self.max_lateness,
                "successful_sends_per_second": self.sent / elapsed if elapsed else None}

    def __exit__(self, *args):
        self.stopping.set()
        self.thread.join(timeout=2)
        if self.thread.is_alive():
            raise RuntimeError("External sender did not stop within its bounded wait")
        if self.failure is not None:
            raise RuntimeError("External sender failed") from self.failure
