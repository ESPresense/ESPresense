/**
 * @file test_memory_tier.cpp
 * @brief TDD tests for ESPresense tiered memory management
 *
 * Test-Driven Development: These tests are written FIRST to define
 * the expected behavior before implementation.
 *
 * Run with: pio test -e native -f test_memory_tier
 */

#include <unity.h>
#include <cstring>
#include <cstdint>

// Forward declarations for types we'll implement
// These will fail to compile until BleMemoryTier.h exists

#ifdef UNIT_TEST
// Mock ESP32 heap functions for native testing
static size_t mock_free_heap = 100000;
static size_t mock_largest_block = 80000;

extern "C" size_t heap_caps_get_free_size(uint32_t caps) {
    (void)caps;
    return mock_free_heap;
}

extern "C" size_t heap_caps_get_largest_free_block(uint32_t caps) {
    (void)caps;
    return mock_largest_block;
}

void set_mock_heap(size_t free_heap, size_t largest_block) {
    mock_free_heap = free_heap;
    mock_largest_block = largest_block;
}
#endif

// Include the header we're testing (will fail until implemented)
#include "BleMemoryTier.h"

// ============================================================================
// PHASE 1: Core Data Structure Tests
// ============================================================================

void test_ColdRecord_size_is_24_bytes(void) {
    // ColdRecord must be exactly 24 bytes for predictable memory usage
    // 256 records * 24 bytes = 6144 bytes (6KB)
    TEST_ASSERT_EQUAL(24, sizeof(ColdRecord));
}

void test_ColdRecord_fields_are_correct_size(void) {
    ColdRecord record = {};

    // Verify field sizes add up to 24 bytes
    TEST_ASSERT_EQUAL(6, sizeof(record.mac));        // MAC address
    TEST_ASSERT_EQUAL(1, sizeof(record.rssi));       // Last RSSI
    TEST_ASSERT_EQUAL(1, sizeof(record.rssiMax));    // Max RSSI seen
    TEST_ASSERT_EQUAL(1, sizeof(record.addrType));   // BLE address type
    TEST_ASSERT_EQUAL(1, sizeof(record.idType));     // Classification hint
    TEST_ASSERT_EQUAL(1, sizeof(record.flags));      // State bitfield
    TEST_ASSERT_EQUAL(1, sizeof(record.seenCount));  // Capped at 255
    TEST_ASSERT_EQUAL(4, sizeof(record.lastSeenMs)); // Last seen millis
    TEST_ASSERT_EQUAL(4, sizeof(record.firstSeenMs));// First seen millis
    TEST_ASSERT_EQUAL(1, sizeof(record.rssiAvg));    // Running average (not sum!)
    TEST_ASSERT_EQUAL(1, sizeof(record.rssiSamples));// Sample count for average
    TEST_ASSERT_EQUAL(2, sizeof(record.reserved));   // Future use
}

void test_ColdRecord_tombstone_flags(void) {
    ColdRecord record = {};
    record.clear();

    // Fresh record should be empty (not valid, not tombstone)
    TEST_ASSERT_TRUE(record.isEmpty());
    TEST_ASSERT_FALSE(record.isValid());
    TEST_ASSERT_FALSE(record.isTombstone());
    TEST_ASSERT_TRUE(record.canStopProbing());  // Empty = end of chain
    TEST_ASSERT_TRUE(record.canInsertHere());   // Can insert in empty

    // Set valid
    record.setValid();
    TEST_ASSERT_FALSE(record.isEmpty());
    TEST_ASSERT_TRUE(record.isValid());
    TEST_ASSERT_FALSE(record.isTombstone());
    TEST_ASSERT_FALSE(record.canStopProbing()); // Valid = continue probing
    TEST_ASSERT_FALSE(record.canInsertHere());  // Can't insert in valid

    // Mark as tombstone (critical for hash table correctness)
    record.markTombstone();
    TEST_ASSERT_FALSE(record.isEmpty());
    TEST_ASSERT_FALSE(record.isValid());
    TEST_ASSERT_TRUE(record.isTombstone());
    TEST_ASSERT_FALSE(record.canStopProbing()); // Tombstone = continue probing!
    TEST_ASSERT_TRUE(record.canInsertHere());   // Can reuse tombstone
}

void test_ColdRecord_rssi_average_no_overflow(void) {
    ColdRecord record = {};
    record.clear();
    record.rssiAvg = -40;
    record.rssiSamples = 1;

    // Update with readings that would overflow int8_t if summed
    // -40 + (-90 * 100) = -9040 would overflow, but EMA won't
    for (int i = 0; i < 100; i++) {
        record.updateRssiAverage(-90);
    }

    // Average should converge toward -90, not overflow
    // With EMA alpha=0.125, after 100 iterations it should be close to -90
    TEST_ASSERT_TRUE(record.rssiAvg < -80);  // Should have moved significantly toward -90
    TEST_ASSERT_TRUE(record.rssiAvg > -100); // But not below valid range
    TEST_ASSERT_EQUAL(101, record.rssiSamples);
}

void test_fnv1a_hash_is_deterministic(void) {
    uint8_t mac[6] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC};

    uint32_t hash1 = fnv1a_hash(mac, 6);
    uint32_t hash2 = fnv1a_hash(mac, 6);

    TEST_ASSERT_EQUAL_UINT32(hash1, hash2);
}

void test_fnv1a_hash_different_for_different_macs(void) {
    uint8_t mac1[6] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC};
    uint8_t mac2[6] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};

    uint32_t hash1 = fnv1a_hash(mac1, 6);
    uint32_t hash2 = fnv1a_hash(mac2, 6);

    TEST_ASSERT_NOT_EQUAL(hash1, hash2);
}

// ============================================================================
// PHASE 2: Quick Classifier Tests (Stack-only, no heap allocation)
// ============================================================================

void test_classifier_drops_weak_rssi(void) {
    uint8_t mac[6] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC};

    // RSSI below MIN_RSSI (-90) should be dropped
    ClassifyResult result = quickClassify(mac, nullptr, 0, -95);
    TEST_ASSERT_EQUAL(ClassifyResult::DROP, result);
}

void test_classifier_cold_for_moderate_rssi(void) {
    uint8_t mac[6] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC};

    // RSSI between MIN_RSSI_COLD (-85) and MIN_RSSI (-90) should be COLD
    ClassifyResult result = quickClassify(mac, nullptr, 0, -87);
    TEST_ASSERT_EQUAL(ClassifyResult::COLD, result);
}

void test_classifier_hot_for_known_mac(void) {
    uint8_t knownMac[6] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};

    // Add MAC to known list
    QuickClassifier classifier;
    classifier.addKnownMac(knownMac);

    // Known MAC should always be HOT (regardless of RSSI above minimum)
    ClassifyResult result = classifier.classify(knownMac, nullptr, 0, -80);
    TEST_ASSERT_EQUAL(ClassifyResult::HOT, result);
}

void test_classifier_hot_for_ibeacon(void) {
    uint8_t mac[6] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC};

    // iBeacon manufacturer data: 0x004C (Apple) + 0x0215 (iBeacon type)
    uint8_t ibeaconData[] = {
        0x02, 0x15,  // iBeacon type + length
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // UUID part 1
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // UUID part 2
        0x00, 0x01,  // Major
        0x00, 0x02,  // Minor
        0xC5         // TX Power
    };

    QuickClassifier classifier;
    ClassifyResult result = classifier.classify(mac, ibeaconData, sizeof(ibeaconData), -70);
    TEST_ASSERT_EQUAL(ClassifyResult::HOT, result);
}

void test_classifier_default_is_cold(void) {
    uint8_t mac[6] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC};

    // Unknown device with decent RSSI should go to COLD tier
    QuickClassifier classifier;
    ClassifyResult result = classifier.classify(mac, nullptr, 0, -70);
    TEST_ASSERT_EQUAL(ClassifyResult::COLD, result);
}

// ============================================================================
// PHASE 3: Cold Tier Tests
// ============================================================================

void test_cold_tier_insert_and_lookup(void) {
    ColdTier tier;
    uint8_t mac[6] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC};

    // Insert a record
    bool inserted = tier.insert(mac, -70, 0);
    TEST_ASSERT_TRUE(inserted);

    // Look it up
    ColdRecord* found = tier.lookup(mac);
    TEST_ASSERT_NOT_NULL(found);
    TEST_ASSERT_EQUAL(-70, found->rssi);
}

void test_cold_tier_lookup_returns_null_for_missing(void) {
    ColdTier tier;
    uint8_t mac[6] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC};

    // Should return null for non-existent MAC
    ColdRecord* found = tier.lookup(mac);
    TEST_ASSERT_NULL(found);
}

void test_cold_tier_update_existing(void) {
    ColdTier tier;
    uint8_t mac[6] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC};

    // Insert then update
    tier.insert(mac, -70, 0);
    tier.update(mac, -65);  // Stronger signal

    ColdRecord* found = tier.lookup(mac);
    TEST_ASSERT_NOT_NULL(found);
    TEST_ASSERT_EQUAL(-65, found->rssi);
    TEST_ASSERT_EQUAL(-65, found->rssiMax);  // Max should update
}

void test_cold_tier_evicts_oldest_when_full(void) {
    ColdTier tier(8);  // Small tier for testing

    // Fill the tier to 75% capacity (6 records, triggers eviction at next insert)
    for (int i = 0; i < 6; i++) {
        uint8_t mac[6] = {0, 0, 0, 0, 0, (uint8_t)i};
        // Give different timestamps so oldest is clearly defined
        tier.insert(mac, -70 + i, i * 1000);
    }

    // Insert one more - should evict oldest (MAC with lastSeenMs=0)
    uint8_t newMac[6] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
    bool inserted = tier.insert(newMac, -60, 60000);  // 60 seconds later
    TEST_ASSERT_TRUE(inserted);

    // First MAC should be evicted (it has oldest timestamp)
    uint8_t oldestMac[6] = {0, 0, 0, 0, 0, 0};
    ColdRecord* found = tier.lookup(oldestMac);
    TEST_ASSERT_NULL(found);

    // New MAC should exist
    found = tier.lookup(newMac);
    TEST_ASSERT_NOT_NULL(found);
}

// ============================================================================
// PHASE 4: Memory Watchdog Tests
// ============================================================================

void test_watchdog_detects_low_memory(void) {
    MemoryWatchdog watchdog;

    // Set mock heap to critical level (below 30KB threshold)
    set_mock_heap(25000, 20000);

    TEST_ASSERT_TRUE(watchdog.isMemoryPressure());
}

void test_watchdog_ok_with_sufficient_memory(void) {
    MemoryWatchdog watchdog;

    // Set mock heap to healthy level
    set_mock_heap(100000, 80000);

    TEST_ASSERT_FALSE(watchdog.isMemoryPressure());
}

void test_watchdog_detects_fragmentation(void) {
    MemoryWatchdog watchdog;

    // Free heap is high but largest block is small = fragmentation
    set_mock_heap(100000, 20000);  // Only 20% contiguous

    TEST_ASSERT_TRUE(watchdog.isFragmented());
}

void test_watchdog_restart_rate_limited(void) {
    MemoryWatchdog watchdog;

    // Should allow first restart
    TEST_ASSERT_TRUE(watchdog.canRestart());
    watchdog.recordRestart();

    // Should allow second restart
    TEST_ASSERT_TRUE(watchdog.canRestart());
    watchdog.recordRestart();

    // Should allow third restart
    TEST_ASSERT_TRUE(watchdog.canRestart());
    watchdog.recordRestart();

    // Should block fourth restart (max 3 per hour)
    TEST_ASSERT_FALSE(watchdog.canRestart());
}

// ============================================================================
// PHASE 5: Eviction Scorer Tests
// ============================================================================

void test_eviction_score_higher_for_included_device(void) {
    EvictionScorer scorer;

    // Included device should have infinite score (non-evictable)
    float includedScore = scorer.calculate(ID_TYPE_KNOWN_MAC, true, false, 1000, 5.0f, 10);
    float normalScore = scorer.calculate(ID_TYPE_MISC, false, false, 1000, 5.0f, 10);

    TEST_ASSERT_TRUE(includedScore > normalScore);
    TEST_ASSERT_TRUE(isinf(includedScore));
}

void test_eviction_score_higher_for_ibeacon(void) {
    EvictionScorer scorer;

    float ibeaconScore = scorer.calculate(ID_TYPE_IBEACON, false, false, 1000, 5.0f, 10);
    float miscScore = scorer.calculate(ID_TYPE_MISC, false, false, 1000, 5.0f, 10);

    TEST_ASSERT_TRUE(ibeaconScore > miscScore);
}

void test_eviction_score_decays_with_age(void) {
    EvictionScorer scorer;

    float recentScore = scorer.calculate(ID_TYPE_MISC, false, false, 1000, 5.0f, 10);    // 1 second old
    float oldScore = scorer.calculate(ID_TYPE_MISC, false, false, 300000, 5.0f, 10);     // 5 minutes old

    TEST_ASSERT_TRUE(recentScore > oldScore);
}

void test_eviction_score_higher_for_closer_device(void) {
    EvictionScorer scorer;

    float closeScore = scorer.calculate(ID_TYPE_MISC, false, false, 1000, 1.0f, 10);   // 1 meter
    float farScore = scorer.calculate(ID_TYPE_MISC, false, false, 1000, 10.0f, 10);    // 10 meters

    TEST_ASSERT_TRUE(closeScore > farScore);
}

// ============================================================================
// PHASE 6: Integration Tests
// ============================================================================

void test_tiered_memory_config_defaults(void) {
    TieredMemoryConfig config;

    // More conservative defaults based on user feedback
    TEST_ASSERT_TRUE(config.enabled);
    TEST_ASSERT_EQUAL(64, config.maxHot);
    TEST_ASSERT_EQUAL(128, config.maxCold);  // Reduced from 256 for more headroom
    TEST_ASSERT_EQUAL(-90, config.minRssi);
    TEST_ASSERT_EQUAL(-85, config.minRssiCold);
    TEST_ASSERT_EQUAL(30000, config.criticalHeap);  // Increased from 20KB
    TEST_ASSERT_EQUAL(3, config.maxRestartsPerHour);
}

void test_backward_compatibility_when_disabled(void) {
    TieredMemoryConfig config;
    config.enabled = false;

    // When disabled, everything should go to HOT tier (original behavior)
    TieredMemoryManager manager(config);
    uint8_t mac[6] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC};

    ClassifyResult result = manager.classify(mac, nullptr, 0, -95);
    TEST_ASSERT_EQUAL(ClassifyResult::HOT, result);  // Even weak signals go to HOT when disabled
}

// ============================================================================
// Test Runner
// ============================================================================

void setUp(void) {
    // Reset mock heap to defaults before each test
    set_mock_heap(100000, 80000);
}

void tearDown(void) {
    // Clean up after each test
}

int main(int argc, char **argv) {
    UNITY_BEGIN();

    // Phase 1: Core Data Structures
    RUN_TEST(test_ColdRecord_size_is_24_bytes);
    RUN_TEST(test_ColdRecord_fields_are_correct_size);
    RUN_TEST(test_ColdRecord_tombstone_flags);
    RUN_TEST(test_ColdRecord_rssi_average_no_overflow);
    RUN_TEST(test_fnv1a_hash_is_deterministic);
    RUN_TEST(test_fnv1a_hash_different_for_different_macs);

    // Phase 2: Quick Classifier
    RUN_TEST(test_classifier_drops_weak_rssi);
    RUN_TEST(test_classifier_cold_for_moderate_rssi);
    RUN_TEST(test_classifier_hot_for_known_mac);
    RUN_TEST(test_classifier_hot_for_ibeacon);
    RUN_TEST(test_classifier_default_is_cold);

    // Phase 3: Cold Tier
    RUN_TEST(test_cold_tier_insert_and_lookup);
    RUN_TEST(test_cold_tier_lookup_returns_null_for_missing);
    RUN_TEST(test_cold_tier_update_existing);
    RUN_TEST(test_cold_tier_evicts_oldest_when_full);

    // Phase 4: Memory Watchdog
    RUN_TEST(test_watchdog_detects_low_memory);
    RUN_TEST(test_watchdog_ok_with_sufficient_memory);
    RUN_TEST(test_watchdog_detects_fragmentation);
    RUN_TEST(test_watchdog_restart_rate_limited);

    // Phase 5: Eviction Scorer
    RUN_TEST(test_eviction_score_higher_for_included_device);
    RUN_TEST(test_eviction_score_higher_for_ibeacon);
    RUN_TEST(test_eviction_score_decays_with_age);
    RUN_TEST(test_eviction_score_higher_for_closer_device);

    // Phase 6: Integration
    RUN_TEST(test_tiered_memory_config_defaults);
    RUN_TEST(test_backward_compatibility_when_disabled);

    return UNITY_END();
}
