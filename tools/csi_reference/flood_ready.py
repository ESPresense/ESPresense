#!/usr/bin/env python3
"""Keep the flood's exit code and publish readiness only after it claims hci0."""
from pathlib import Path
import subprocess
import sys

with subprocess.Popen([sys.executable, "-u", "/app/ble_flood.py", *sys.argv[1:]],
                      stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True) as child:
    for line in child.stdout:
        print(line, end="", flush=True)
        if "[flood] hci0 claimed, rotating at 40.0/s" in line:
            Path("/flood/ready").write_text(line)
    raise SystemExit(child.wait())
