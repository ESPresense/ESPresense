#!/usr/bin/env python3
"""Drop-in replacement for the `grep -vE "^[0-9]+ (New|Del) "` filter in .woodpecker/hil.yml.

The raw per-event BLE New/Del lines are too high-volume to leave in the CI log (see the
comment in hil.yml: an 80 lines/s flood stalled the Woodpecker agent on pipeline 1280), but
discarding them outright throws away the one number a CSI/BLE coexistence check needs: how
many adverts the node actually saw. This keeps the log quiet and periodically reports a rate
instead of either the raw spam or nothing. Every other serial line passes through unchanged.
"""
import re
import sys
import time

PATTERN = re.compile(r"^\d+ (New|Del) ")
INTERVAL_SECS = 30


def main():
    new_count = 0
    del_count = 0
    window_start = time.monotonic()

    def flush(now):
        nonlocal new_count, del_count, window_start
        elapsed = now - window_start
        if elapsed <= 0:
            return
        rate = (new_count + del_count) / elapsed
        print(f"[coex] New={new_count} Del={del_count} over {elapsed:.0f}s (rate={rate:.2f}/s)", flush=True)
        new_count = del_count = 0
        window_start = now

    for line in sys.stdin:
        line = line.rstrip("\n")
        match = PATTERN.match(line)
        if match:
            if match.group(1) == "New":
                new_count += 1
            else:
                del_count += 1
        else:
            print(line, flush=True)
        now = time.monotonic()
        if now - window_start >= INTERVAL_SECS:
            flush(now)

    flush(time.monotonic())


if __name__ == "__main__":
    main()
