"""
Post-build script to check firmware size against partition size for ESP32-C3 boards.
Prevents silent boot failures when firmware exceeds partition size.

This addresses the issue where ESP32-C3 firmware (esp32-c3-devkitm-1) can exceed
the app0 partition size after Arduino v3 / IDF 5.4 migration, causing boot loops.

Usage: Add to platformio.ini:
  extra_scripts = check_partitionsize.py

The script reads partition sizes from partitions_singleapp.csv and fails the build
if firmware exceeds the partition size (with a 4KB safety margin).
"""
import os
import sys
from pathlib import Path


def get_partition_size(env, partition_name="app0"):
    """Read partition size from the configured CSV file."""
    partition_file = os.path.join(env.GetLaunchDir(), "partitions_singleapp.csv")
    if not os.path.exists(partition_file):
        sys.stderr.write(f"[check_partitionsize] Warning: partition file not found: {partition_file}\n")
        return None
    
    with open(partition_file, "r") as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            parts = [p.strip() for p in line.split(",")]
            if len(parts) >= 5 and parts[0].strip() == partition_name:
                size_str = parts[4].strip()
                return int(size_str, 0)
    return None


def check_firmware_size(source, target, env):
    """Check compiled firmware size against partition limit.
    
    Runs after the firmware ELF is built and size is checked.
    If firmware exceeds partition size, the build fails before flashing.
    """
    board = env.get("BOARD", "")
    if "esp32c3" not in board.lower():
        return 0
    
    firmware_name = env.get("PROGNAME", "espresense")
    build_dir = env.get("BUILD_DIR", ".")
    firmware_path = Path(build_dir) / (firmware_name + ".bin")
    
    if not firmware_path.exists():
        sys.stderr.write(f"[check_partitionsize] Warning: firmware not found at {firmware_path}, skipping size check\n")
        return 0
    
    firmware_size = firmware_path.stat().st_size
    partition_size = get_partition_size(env, "app0")
    
    if partition_size is None:
        sys.stderr.write("[check_partitionsize] Warning: could not determine partition size, skipping check\n")
        return 0
    
    # Leave 4KB margin for safety
    safe_limit = partition_size - 0x1000
    
    size_mb = firmware_size / (1024 * 1024)
    partition_mb = partition_size / (1024 * 1024)
    
    if firmware_size > safe_limit:
        msg = (f"\n*** Firmware size ({firmware_size} bytes, {size_mb:.2f} MB) "
               f"exceeds app0 partition ({partition_size} bytes, {partition_mb:.2f} MB)! ***\n"
               f"*** Boot failure expected! Increase partition size or reduce firmware. ***\n")
        sys.stderr.write(msg)
        env.Exit(1)
        return 1
    
    sys.stderr.write(f"[check_partitionsize] OK: {size_mb:.2f} MB / {partition_mb:.2f} MB\n")
    return 0


Import("env")
# Hook into check_prog_size which runs after firmware is built
env.AddPostAction("check_prog_size", check_firmware_size)
