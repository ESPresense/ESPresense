#!/usr/bin/env python
"""Print serial output for N seconds: serial_log.py <port> [seconds=20]. Needs pyserial (in the IDF venv)."""
import sys, time
import serial


def open_port(port):
    # DTR/RTS low before opening: on USB-UART boards (CP210x/CH340) they drive EN/IO0, so a
    # plain open (or the reopen below) would reset the chip every time.
    s = serial.Serial()
    s.port, s.baudrate, s.timeout = port, 115200, 0.5
    s.dtr = s.rts = False
    s.open()
    return s

port, secs = sys.argv[1], float(sys.argv[2]) if len(sys.argv) > 2 else 20
# ponytail: waits out the USB re-enumeration after reset; native-USB S3/C6 drop the port briefly.
end = time.time() + secs
while True:
    try:
        s = open_port(port)
        break
    except serial.SerialException:
        if time.time() > end: sys.exit(f"could not open {port}")
        time.sleep(0.2)
# Reopen only when the port goes away (native USB re-enumerates on reset). Never reopen on
# silence: on both native USB and USB-UART, opening the port resets the chip.
last = time.time()
while time.time() < end:
    try:
        data = s.read(4096)
    except serial.SerialException:  # port vanished or went quiet (reset), reopen
        s.close(); time.sleep(0.2); last = time.time()
        try: s = open_port(port)
        except serial.SerialException: pass
        continue
    if data:
        last = time.time()
        sys.stdout.write(data.decode(errors="replace")); sys.stdout.flush()
