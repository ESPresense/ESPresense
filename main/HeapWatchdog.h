#pragma once

#include <cstdint>

// The low-heap reboot decision, kept apart from main.cpp so it can be tested without
// hardware. See loop() for the caller.
//
// #2309 showed why free heap alone is the wrong number to guard. A node ran 6.8h with
// telemetry failing from 3.8h onward and never rebooted: it sat at 16-20KB free, above
// MQTT_MIN_FREE_MEMORY, while the largest free block had already fallen under the 2312 byte
// allocation that 93% of its failed allocations asked for. Fragmented-but-not-empty looks
// healthy to a free-heap check and is just as dead in practice, so it gets its own floor.
enum class HeapTrip : uint8_t {
    None,
    FreeHeap,  // total free heap exhausted
    MaxAlloc,  // heap fragmented: largest contiguous block too small to serve a real request
};

// One slow-loop tick. Returns the condition that has held for `threshold` consecutive
// ticks (12 x 5s = 60s of debounce), or None.
//
// `passes` is caller-owned state and is reset by any healthy tick, so a node that dips and
// recovers does not accumulate toward a reboot. Both floors share the counter: a heap that
// alternates between the two failure modes is not healthy in between, and restarting the
// count on each alternation would let it limp indefinitely — the exact behaviour this
// exists to stop.
//
// Reporting *which* floor tripped is the point of the enum rather than a bool. The
// complaint in #2309 was a serial log that said "Low memory" and never said why the node
// then died, so the caller has to be able to name the condition in the reboot line.
inline HeapTrip heapWatchdogTick(uint32_t freeHeap, uint32_t maxAlloc,
                                 uint32_t freeFloor, uint32_t maxAllocFloor,
                                 uint8_t &passes, uint8_t threshold = 12) {
    bool const freeLow = freeHeap < freeFloor;
    bool const allocLow = maxAlloc < maxAllocFloor;

    if (!freeLow && !allocLow) {
        passes = 0;
        return HeapTrip::None;
    }

    if (passes < UINT8_MAX) passes++;
    if (passes < threshold) return HeapTrip::None;

    // Free heap first when both are low: it is the stronger statement about the node, and
    // an empty heap is necessarily also a fragmented one.
    return freeLow ? HeapTrip::FreeHeap : HeapTrip::MaxAlloc;
}
