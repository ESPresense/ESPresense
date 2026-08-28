// Low-heap watchdog trip decision: the debounce, and which floor gets reported.
//
// #2309's node never rebooted because the watchdog only ever looked at free heap, so the
// case that matters most here is a heap with plenty free and no contiguous block left.
#include <unity.h>

#include "../../main/HeapWatchdog.h"

namespace {

constexpr uint32_t FREE_FLOOR = 12192;  // MQTT_MIN_FREE_MEMORY
constexpr uint32_t ALLOC_FLOOR = 4096;  // MIN_MAX_ALLOC_HEAP

HeapTrip tick(uint32_t freeHeap, uint32_t maxAlloc, uint8_t &passes) {
    return heapWatchdogTick(freeHeap, maxAlloc, FREE_FLOOR, ALLOC_FLOOR, passes);
}

// Run `count` identical ticks and return the last verdict.
HeapTrip run(uint32_t freeHeap, uint32_t maxAlloc, uint8_t &passes, int count) {
    HeapTrip last = HeapTrip::None;
    for (int i = 0; i < count; i++) last = tick(freeHeap, maxAlloc, passes);
    return last;
}

void test_healthy_never_trips(void) {
    uint8_t passes = 0;
    // A whole day of healthy slow-loop ticks.
    TEST_ASSERT_TRUE(HeapTrip::None == run(120000, 90000, passes, 17280));
    TEST_ASSERT_EQUAL_UINT8(0, passes);
}

void test_free_heap_trips_at_twelve_not_eleven(void) {
    uint8_t passes = 0;
    TEST_ASSERT_TRUE(HeapTrip::None == run(8000, 90000, passes, 11));
    TEST_ASSERT_TRUE(HeapTrip::FreeHeap == tick(8000, 90000, passes));
}

// The #2309 case: 20KB free is above the free floor and looks fine, but nothing contiguous
// is left. The old watchdog sat here until the node Guru'd.
void test_fragmentation_trips_while_free_heap_looks_healthy(void) {
    uint8_t passes = 0;
    TEST_ASSERT_TRUE(HeapTrip::None == run(20000, 2000, passes, 11));
    TEST_ASSERT_TRUE(HeapTrip::MaxAlloc == tick(20000, 2000, passes));
}

void test_free_heap_wins_when_both_are_low(void) {
    uint8_t passes = 0;
    // An empty heap is necessarily a fragmented one; report the stronger statement.
    TEST_ASSERT_TRUE(HeapTrip::FreeHeap == run(1000, 500, passes, 12));
}

void test_one_healthy_tick_resets_the_count(void) {
    uint8_t passes = 0;
    run(8000, 90000, passes, 11);
    TEST_ASSERT_TRUE(HeapTrip::None == tick(120000, 90000, passes));
    TEST_ASSERT_EQUAL_UINT8(0, passes);
    // Back to starving: a full fresh debounce is required, not one more tick.
    TEST_ASSERT_TRUE(HeapTrip::None == run(8000, 90000, passes, 11));
    TEST_ASSERT_TRUE(HeapTrip::FreeHeap == tick(8000, 90000, passes));
}

// Alternating between the two failure modes is not recovery, and must not defer the reboot.
void test_alternating_conditions_still_trip(void) {
    uint8_t passes = 0;
    HeapTrip last = HeapTrip::None;
    for (int i = 0; i < 12; i++)
        last = i % 2 ? tick(20000, 2000, passes)   // fragmented
                     : tick(8000, 90000, passes);  // empty
    TEST_ASSERT_TRUE(HeapTrip::None != last);
}

// At the floor exactly is healthy; one byte under is not.
void test_boundaries_are_exclusive(void) {
    uint8_t passes = 0;
    TEST_ASSERT_TRUE(HeapTrip::None == run(FREE_FLOOR, ALLOC_FLOOR, passes, 100));
    TEST_ASSERT_EQUAL_UINT8(0, passes);
    TEST_ASSERT_TRUE(HeapTrip::MaxAlloc == run(FREE_FLOOR, ALLOC_FLOOR - 1, passes, 12));
}

// A node left tripping without rebooting must not wrap its counter back under the
// threshold and go quiet.
void test_counter_saturates_rather_than_wrapping(void) {
    uint8_t passes = 0;
    TEST_ASSERT_TRUE(HeapTrip::MaxAlloc == run(20000, 2000, passes, 1000));
    TEST_ASSERT_EQUAL_UINT8(UINT8_MAX, passes);
}

}  // namespace

int main(int, char **) {
    UNITY_BEGIN();
    RUN_TEST(test_healthy_never_trips);
    RUN_TEST(test_free_heap_trips_at_twelve_not_eleven);
    RUN_TEST(test_fragmentation_trips_while_free_heap_looks_healthy);
    RUN_TEST(test_free_heap_wins_when_both_are_low);
    RUN_TEST(test_one_healthy_tick_resets_the_count);
    RUN_TEST(test_alternating_conditions_still_trip);
    RUN_TEST(test_boundaries_are_exclusive);
    RUN_TEST(test_counter_saturates_rather_than_wrapping);
    return UNITY_END();
}
