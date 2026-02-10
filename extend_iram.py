Import("env")
import re

# Extend IRAM0 segment by 4KB (0x1000) for ESP32 to accommodate NimBLE 2.x
# This borrows from SRAM1 which would otherwise be heap
def extend_iram(env):
    ldscript_dir = env.subst("$PROJECT_PACKAGES_DIR/framework-arduinoespressif32-libs/esp32/ld")
    memory_ld = ldscript_dir + "/memory.ld"

    try:
        with open(memory_ld, "r") as f:
            content = f.read()

        old = "iram0_0_seg (RX) : org = 0x40080000, len = 0x20000 + 0x0"
        new = "iram0_0_seg (RX) : org = 0x40080000, len = 0x20000 + 0x1000"

        if old in content:
            content = content.replace(old, new)
            with open(memory_ld, "w") as f:
                f.write(content)
            print("IRAM extended by 4KB (0x1000) for NimBLE 2.x compatibility")
        elif new in content:
            print("IRAM already extended")
        else:
            print("WARNING: Could not find IRAM segment to extend")
    except Exception as e:
        print(f"WARNING: Could not extend IRAM: {e}")

extend_iram(env)
