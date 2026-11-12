#!/usr/bin/env python3
"""Prove the SDK Wi-Fi lifecycle object calls its native coexistence API."""
import argparse
import hashlib
import json
from pathlib import Path
import re
import subprocess
import struct

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--build', type=Path, required=True)
parser.add_argument('--label', required=True)
parser.add_argument('--require', action='store_true')
parser.add_argument('--elf-audit', action='store_true', help='Inspect final C3 ELF binding without executing firmware')
parser.add_argument('--output', type=Path, help='Save machine-readable object/link evidence')
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
if args.elf_audit:
    images = list(args.build.glob('*.elf'))
    assert len(images) == 1, 'Expected one final application ELF'
    image = images[0]
    raw = image.read_bytes()
    assert len(raw) >= 52 and raw[:6] == b'\x7fELF\x01\x01', 'Expected ELF32 little endian'
    assert struct.unpack_from('<H', raw, 18)[0] == 243, 'Expected RISC-V C3 application'
    section_offset = struct.unpack_from('<I', raw, 32)[0]
    section_size, section_count = struct.unpack_from('<HH', raw, 46)
    assert section_size == 40 and section_offset + section_size*section_count <= len(raw)
    defined = subprocess.run(['riscv32-esp-elf-nm', '-S', '--defined-only', str(image)],
                             check=True, capture_output=True, text=True).stdout
    match = re.search(r'^([0-9a-fA-F]+)\s+([0-9a-fA-F]+)\s+[Tt]\s+coex_preference_set$', defined, re.M)
    assert match, 'Final ELF does not define the vendor preference function as application code'
    address, size = (int(x, 16) for x in match.groups())
    sections = [struct.unpack_from('<10I', raw, section_offset+i*section_size)
                for i in range(section_count)]
    owners = [s for s in sections if s[2] & 4 and s[3] <= address and address+size <= s[3]+s[5]]
    assert size > 0 and len(owners) == 1, 'Cannot identify final executable function bytes'
    owner = owners[0]
    offset = owner[4]+address-owner[3]
    assert offset+size <= len(raw)
    body = raw[offset:offset+size]
    def disassemble(symbol):
        return subprocess.run(['riscv32-esp-elf-objdump', '-d', '--disassemble='+symbol, str(image)],
                              check=True, capture_output=True, text=True).stdout
    wrapper = disassemble('esp_coex_preference_set')
    vendor = disassemble('coex_preference_set')
    result.update(elf=str(image), elf_sha256=hashlib.sha256(raw).hexdigest(),
        elf_preference_address=hex(address), elf_preference_size=size,
        elf_preference_hex=body.hex(), elf_preference_is_return_ok_stub=body == bytes.fromhex('01458280'),
        elf_wrapper_targets_vendor_function='<coex_preference_set>' in wrapper,
        elf_wrapper_disassembly=wrapper, elf_vendor_disassembly=vendor,
        link_evidence_scope='Final C3 link binding only; not actual RF allocation, readiness, BLE cost or occupancy accuracy')
if args.output:
    args.output.write_text(json.dumps(result, indent=2)+'\n')

print('CSI_REFERENCE_BUILD_AUDIT '+json.dumps(result, sort_keys=True), flush=True)
if args.label == 'before_dependency' and result['sdk_wifi_lifecycle_calls_coex_preference']:
    raise SystemExit('Baseline already contains the SDK coexistence call; dependency-gap hypothesis not reproduced. Stop before a duplicate hardware trial.')
if args.require and not all(result[key] for key in [
    'sdk_wifi_lifecycle_calls_coex_preference', 'sdk_compile_has_coex_include', 'software_coexistence_enabled']):
    raise SystemExit('SDK native coexistence call was not proven; do not flash this build')
