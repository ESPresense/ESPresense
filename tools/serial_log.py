#!/usr/bin/env python
"""Print serial output for N seconds: serial_log.py <port> [seconds=20]. Needs pyserial (in the IDF venv)."""
import sys, time
import serial

port, secs = sys.argv[1], float(sys.argv[2]) if len(sys.argv) > 2 else 20
# ponytail: waits out the USB re-enumeration after reset; native-USB S3/C6 drop the port briefly.
end = time.time() + secs
while True:
    try:
        s = serial.Serial(port, 115200, timeout=0.5)
        break
    except serial.SerialException:
        if time.time() > end: sys.exit(f"could not open {port}")
        time.sleep(0.2)
# ponytail: reopen after 3s of silence; after a reset macOS can leave us on a dead handle.
last = time.time()
while time.time() < end:
    try:
        data = s.read(4096)
        if not data and time.time() - last > 3:
            raise serial.SerialException("silent")
    except serial.SerialException:  # port vanished or went quiet (reset), reopen
        s.close(); time.sleep(0.2); last = time.time()
        try: s = serial.Serial(port, 115200, timeout=0.5)
        except serial.SerialException: pass
        continue
    if data:
        last = time.time()
        sys.stdout.write(data.decode(errors="replace")); sys.stdout.flush()
