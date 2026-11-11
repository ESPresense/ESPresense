#pragma once

// A saved profile never reduces scanning before the one-shot trial is consumed.
inline constexpr unsigned csiReferenceScanWindowUnits(long profile, bool armedBoot, unsigned full) {
    return !armedBoot ? full : profile == 1 ? full / 4 : profile == 2 ? full / 8 : full;
}
