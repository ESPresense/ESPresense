#!/usr/bin/env python3
"""Prove the SDK Wi-Fi lifecycle object calls its native coexistence API."""
import argparse
import json
from pathlib import Path
import re
import subprocess

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--build', type=Path, required=True)
parser.add_argument('--label', required=True)
parser.add_argument('--require', action='store_true')
args = parser.parse_args()
objects = list(args.build.rglob('wifi_lifecycle.cpp.obj'))
assert len(objects) == 1, f'Expected one SDK Wi-Fi lifecycle object, found{len(objects)}'
symbols = subprocess.run(['riscv32-esp-elf-nm', '--undefined-only', str(objects[0])],
                         check=True, capture_output=True, text=True).stdout
config = (args.build / 'sdkconfig').read_text()
commands = json.loads((args.build / 'compile_commands.json').read_text())
compile_entry = next(row for row in commands if row['file'].endswith('/wifi_lifecycle.cpp'))
command = compile_entry.get('command', ' '.join(compile_entry.get('arguments', [])))
result = {'label': args.label,
    'sdk_wifi_lifecycle_calls_coex_preference': bool(re.search(r'\bU\s+esp_coex_preference_set\b', symbols)),
    'sdk_compile_has_coex_include': 'esp_coex/include' in command,
    'software_coexistence_enabled': 'CONFIG_ESP_COEX_SW_COEXIST_ENABLE=y\n' in config,
    'sdk_source_file': compile_entry['file']}
print('CSI_REFERENCE_BUILD_AUDIT '+json.dumps(result, sort_keys=True), flush=True)
if args.label == 'before_dependency' and result['sdk_wifi_lifecycle_calls_coex_preference']:
    raise SystemExit('Baseline already contains the SDK coexistence call; dependency-gap hypothesis not reproduced. Stop before a duplicate hardware trial.')
if args.require and not all(result[key] for key in [
    'sdk_wifi_lifecycle_calls_coex_preference', 'sdk_compile_has_coex_include', 'software_coexistence_enabled']):
    raise SystemExit('SDK native coexistence call was not proven; do not flash this build')
