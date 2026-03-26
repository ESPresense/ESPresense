#!/usr/bin/env python3
"""
Generate build_timestamp_value.h at the start of each build.
Uses a PlatformIO hook to ensure it runs before compilation.
"""
import os
from datetime import datetime

def generate_timestamp_header(source, target, env):
    """Generate the build timestamp header file."""
    header_path = os.path.join(env["PROJECT_DIR"], "src", "build_timestamp_value.h")
    timestamp = datetime.now().strftime("%Y-%m-%dT%H:%M:%S")
    with open(header_path, "w") as fp:
        fp.write(f'#define BUILD_TIMESTAMP "{timestamp}"')

Import("env")
# Run before compiling any source files
env.AddPreAction("check_prog_size", generate_timestamp_header)
