"""Build-time firmware size guard (#2252).

After each firmware image is built, compare its size against the first app
slot (app0/ota_0) of the partition table this environment actually builds
with (board_build.partitions). An image larger than its slot silently fails
to boot (see #2244), so fail the build here with an actionable error.
Print a utilization warning at >=90% but only fail on a real overflow.
"""

Import("env")

import os

WARN_PERCENT = 90


def _parse_size(text):
    text = text.strip().lower()
    multipliers = {"k": 1024, "m": 1024 * 1024}
    if text[-1:] in multipliers:
        return int(text[:-1], 0) * multipliers[text[-1]]
    return int(text, 0)


def _resolve_partition_csv(env, name):
    candidates = []
    if os.path.isabs(name):
        candidates.append(name)
    else:
        candidates.append(os.path.join(env.subst("$PROJECT_DIR"), name))
        # Framework built-in tables (e.g. "default.csv", "min_spiffs.csv")
        try:
            framework_dir = env.PioPlatform().get_package_dir(
                "framework-arduinoespressif32"
            )
        except Exception:
            framework_dir = None
        if framework_dir:
            candidates.append(
                os.path.join(framework_dir, "tools", "partitions", os.path.basename(name))
            )
    for candidate in candidates:
        if os.path.isfile(candidate):
            return candidate
    return None


def _first_app_slot(csv_path):
    """Return (name, size) of the first `app` partition in the CSV."""
    with open(csv_path) as fp:
        for line in fp:
            line = line.split("#")[0].strip()
            if not line:
                continue
            fields = [field.strip() for field in line.split(",")]
            if len(fields) >= 5 and fields[1] == "app":
                return fields[0], _parse_size(fields[4])
    return None, None


def check_fw_size(target, source, env):
    partitions = env.GetProjectOption("board_build.partitions")
    if not partitions:
        print("check_fw_size: no board_build.partitions for this env; skipping")
        return
    csv_path = _resolve_partition_csv(env, partitions)
    if not csv_path:
        print("check_fw_size: partition table '%s' not found; skipping" % partitions)
        return
    slot_name, slot_size = _first_app_slot(csv_path)
    if not slot_size:
        print("check_fw_size: no app slot found in %s; skipping" % csv_path)
        return

    image_path = str(target[0])
    image_size = os.path.getsize(image_path)
    percent = image_size * 100.0 / slot_size
    print(
        "check_fw_size: %s image %d bytes / app slot '%s' %d bytes from %s (%.1f%%)"
        % (env.subst("$PIOENV"), image_size, slot_name, slot_size, csv_path, percent)
    )

    if percent >= WARN_PERCENT and image_size <= slot_size:
        print(
            "check_fw_size: WARNING: app image at %.1f%% of its %d-byte app slot; "
            "approaching the partition limit" % (percent, slot_size)
        )

    if image_size > slot_size:
        raise Exception(
            "FIRMWARE TOO BIG for env '%s': image is %d bytes but app slot '%s' "
            "in %s holds only %d bytes (over by %d bytes). The device would "
            "silently boot-loop (#2244). Trim features/code or enlarge the app "
            "partition in the partition table."
            % (
                env.subst("$PIOENV"),
                image_size,
                slot_name,
                csv_path,
                slot_size,
                image_size - slot_size,
            )
        )


env.AddPostAction("$BUILD_DIR/${PROGNAME}.bin", check_fw_size)
