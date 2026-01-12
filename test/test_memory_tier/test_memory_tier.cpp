/**
 * @file test_memory_tier.cpp
 * @brief Unit tests for ESPresense tiered memory management
 *
 * Tests for the tiered memory system including ColdRecord, ColdTier,
 * QuickClassifier, MemoryWatchdog, and EvictionScorer components.
 *
 * Run with: pio test -e native-memory
 */

#include <unity.h>
#include <cstring>
#include <cstdint>

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

#include "BleMemoryTier.h"

// ============================================================================
// ColdRecord Tests
// ============================================================================

void test_ColdRecord_size_is_24_bytes(void) {
    // ColdRecord must be exactly 24 bytes for predictable memory usage
    // 256 records * 24 bytes = 6144 bytes (6KB)
    TEST_ASSERT_EQUAL(24, sizeof(ColdRecord));
}

void test_ColdRecord_fields_are_correct_size(void) {
    ColdRecord record = {};

    TEST_ASSERT_EQUAL(6, sizeof(record.mac));
    TEST_ASSERT_EQUAL(1, sizeof(record.rssi));
    TEST_ASSERT_EQUAL(1, sizeof(record.rssiMax));
    TEST_ASSERT_EQUAL(1, sizeof(record.addrType));
    TEST_ASSERT_EQUAL(1, sizeof(record.idType));
    TEST_ASSERT_EQUAL(1, sizeof(record.flags));
    TEST_ASSERT_EQUAL(1, sizeof(record.seenCount));
    TEST_ASSERT_EQUAL(4, sizeof(record.lastSeenMs));
    TEST_ASSERT_EQUAL(4, sizeof(record.firstSeenMs));
    TEST_ASSERT_EQUAL(1, sizeof(record.rssiAvg));
    TEST_ASSERT_EQUAL(1, sizeof(record.rssiSamples));
    TEST_ASSERT_EQUAL(2, sizeof(record.reserved));
}

void test_ColdRecord_tombstone_flags(void) {
    ColdRecord record = {};
    record.clear();

    // Fresh record should be empty (not valid, not tombstone)
    TEST_ASSERT_TRUE(record.isEmpty());
    TEST_ASSERT_FALSE(record.isValid());
    TEST_ASSERT_FALSE(record.isTombstone());
    TEST_ASSERT_TRUE(record.canStopProbing());
    TEST_ASSERT_TRUE(record.canInsertHere());

    // Set valid
    record.setValid();
    TEST_ASSERT_FALSE(record.isEmpty());
    TEST_ASSERT_TRUE(record.isValid());
    TEST_ASSERT_FALSE(record.isTombstone());
    TEST_ASSERT_FALSE(record.canStopProbing());
    TEST_ASSERT_FALSE(record.canInsertHere());

    // Mark as tombstone (critical for hash table correctness)
    record.markTombstone();
    TEST_ASSERT_FALSE(record.isEmpty());
    TEST_ASSERT_FALSE(record.isValid());
    TEST_ASSERT_TRUE(record.isTombstone());
    TEST_ASSERT_FALSE(record.canStopProbing());
    TEST_ASSERT_TRUE(record.canInsertHere());
}

void test_ColdRecord_rssi_average_no_overflow(void) {
    ColdRecord record = {};
    record.clear();
    record.rssiAvg = -40;
    record.rssiSamples = 1;

    // Update with readings that would overflow int8_t if summed
    for (int i = 0; i < 100; i++) {
        record.updateRssiAverage(-90);
    }

    // Average should converge toward -90, not overflow
    TEST_ASSERT_TRUE(record.rssiAvg < -80);
    TEST_ASSERT_TRUE(record.rssiAvg > -100);
    TEST_ASSERT_EQUAL(101, record.rssiSamples);
}

void test_cold_record_can_be_marked_promoted(void) {
    ColdRecord record;
    record.clear();
    record.setValid();

    TEST_ASSERT_FALSE(record.isPromoted());

    record.setPromoted();
    TEST_ASSERT_TRUE(record.isPromoted());
    TEST_ASSERT_TRUE(record.isValid());
}

// ============================================================================
// Hash Function Tests
// ============================================================================

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
// Classifier Tests
// ============================================================================

void test_classifier_drops_weak_rssi(void) {
    uint8_t mac[6] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC};

    ClassifyResult result = quickClassify(mac, nullptr, 0, -95);
    TEST_ASSERT_EQUAL(ClassifyResult::DROP, result);
}

void test_classifier_cold_for_moderate_rssi(void) {
    uint8_t mac[6] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC};

    ClassifyResult result = quickClassify(mac, nullptr, 0, -87);
    TEST_ASSERT_EQUAL(ClassifyResult::COLD, result);
}

void test_classifier_hot_for_known_mac(void) {
    uint8_t knownMac[6] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};

    QuickClassifier classifier;
    classifier.addKnownMac(knownMac);

    ClassifyResult result = classifier.classify(knownMac, nullptr, 0, -80);
    TEST_ASSERT_EQUAL(ClassifyResult::HOT, result);
}

void test_classifier_hot_for_ibeacon(void) {
    uint8_t mac[6] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC};

    uint8_t ibeaconData[] = {
        0x02, 0x15,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x01,
        0x00, 0x02,
        0xC5
    };

    QuickClassifier classifier;
    ClassifyResult result = classifier.classify(mac, ibeaconData, sizeof(ibeaconData), -70);
    TEST_ASSERT_EQUAL(ClassifyResult::HOT, result);
}

void test_classifier_default_is_cold(void) {
    uint8_t mac[6] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC};

    QuickClassifier classifier;
    ClassifyResult result = classifier.classify(mac, nullptr, 0, -70);
    TEST_ASSERT_EQUAL(ClassifyResult::COLD, result);
}

void test_classifier_under_memory_pressure(void) {
    TieredMemoryConfig config;
    config.criticalHeap = 30000;
    TieredMemoryManager manager(config);

    uint8_t mac[6] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC};

    // Normal memory - moderate RSSI goes to COLD
    set_mock_heap(100000, 80000);
    ClassifyResult result = manager.classify(mac, nullptr, 0, -80);
    TEST_ASSERT_EQUAL(ClassifyResult::COLD, result);

    // Critical memory - system may adapt behavior
    set_mock_heap(20000, 15000);
    result = manager.classify(mac, nullptr, 0, -80);
    TEST_ASSERT_TRUE(result == ClassifyResult::DROP || result == ClassifyResult::COLD);
}

// ============================================================================
// Cold Tier Tests
// ============================================================================

void test_cold_tier_insert_and_lookup(void) {
    ColdTier tier;
    uint8_t mac[6] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC};

    bool inserted = tier.insert(mac, -70, 0);
    TEST_ASSERT_TRUE(inserted);

    ColdRecord* found = tier.lookup(mac);
    TEST_ASSERT_NOT_NULL(found);
    TEST_ASSERT_EQUAL(-70, found->rssi);
}

void test_cold_tier_lookup_returns_null_for_missing(void) {
    ColdTier tier;
    uint8_t mac[6] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC};

    ColdRecord* found = tier.lookup(mac);
    TEST_ASSERT_NULL(found);
}

void test_cold_tier_update_existing(void) {
    ColdTier tier;
    uint8_t mac[6] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC};

    tier.insert(mac, -70, 0);
    tier.update(mac, -65);

    ColdRecord* found = tier.lookup(mac);
    TEST_ASSERT_NOT_NULL(found);
    TEST_ASSERT_EQUAL(-65, found->rssi);
    TEST_ASSERT_EQUAL(-65, found->rssiMax);
}

void test_cold_tier_evicts_oldest_when_full(void) {
    ColdTier tier(8);

    for (int i = 0; i < 6; i++) {
        uint8_t mac[6] = {0, 0, 0, 0, 0, (uint8_t)i};
        tier.insert(mac, -70 + i, i * 1000);
    }

    uint8_t newMac[6] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
    bool inserted = tier.insert(newMac, -60, 60000);
    TEST_ASSERT_TRUE(inserted);

    uint8_t oldestMac[6] = {0, 0, 0, 0, 0, 0};
    ColdRecord* found = tier.lookup(oldestMac);
    TEST_ASSERT_NULL(found);

    found = tier.lookup(newMac);
    TEST_ASSERT_NOT_NULL(found);
}

void test_cold_tier_remove_promoted_record(void) {
    ColdTier tier;
    uint8_t mac[6] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC};

    tier.insert(mac, -70, 0);
    ColdRecord* record = tier.lookup(mac);
    TEST_ASSERT_NOT_NULL(record);

    record->setPromoted();
    bool removed = tier.remove(mac);
    TEST_ASSERT_TRUE(removed);

    record = tier.lookup(mac);
    TEST_ASSERT_NULL(record);
}

void test_cold_tier_remove_nonexistent_returns_false(void) {
    ColdTier tier;
    uint8_t mac[6] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC};

    bool removed = tier.remove(mac);
    TEST_ASSERT_FALSE(removed);
}

void test_cold_tier_update_increments_seen_count(void) {
    ColdTier tier;
    uint8_t mac[6] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC};

    tier.insert(mac, -70, 0);
    ColdRecord* record = tier.lookup(mac);
    TEST_ASSERT_EQUAL(1, record->seenCount);

    for (int i = 0; i < 10; i++) {
        tier.update(mac, -70, (i + 1) * 1000);
    }

    record = tier.lookup(mac);
    TEST_ASSERT_EQUAL(11, record->seenCount);
}

void test_cold_tier_seen_count_caps_at_255(void) {
    ColdTier tier;
    uint8_t mac[6] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC};

    tier.insert(mac, -70, 0);

    for (int i = 0; i < 300; i++) {
        tier.update(mac, -70, i * 100);
    }

    ColdRecord* record = tier.lookup(mac);
    TEST_ASSERT_EQUAL(255, record->seenCount);
}

void test_rssi_max_tracks_strongest_signal(void) {
    ColdTier tier;
    uint8_t mac[6] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC};

    tier.insert(mac, -80, 0);
    ColdRecord* record = tier.lookup(mac);
    TEST_ASSERT_EQUAL(-80, record->rssiMax);

    tier.update(mac, -90, 1000);
    record = tier.lookup(mac);
    TEST_ASSERT_EQUAL(-80, record->rssiMax);

    tier.update(mac, -60, 2000);
    record = tier.lookup(mac);
    TEST_ASSERT_EQUAL(-60, record->rssiMax);

    tier.update(mac, -70, 3000);
    record = tier.lookup(mac);
    TEST_ASSERT_EQUAL(-60, record->rssiMax);
}

void test_promotion_threshold_behavior(void) {
    ColdTier tier;
    uint8_t mac[6] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC};

    tier.insert(mac, -80, 0);

    for (int i = 1; i <= 4; i++) {
        tier.update(mac, -80, i * 1000);
    }
    ColdRecord* record = tier.lookup(mac);
    TEST_ASSERT_NOT_NULL(record);
    TEST_ASSERT_EQUAL(5, record->seenCount);

    tier.update(mac, -70, 5000);
    record = tier.lookup(mac);
    TEST_ASSERT_EQUAL(6, record->seenCount);
    TEST_ASSERT_EQUAL(-70, record->rssi);
}

// ============================================================================
// Memory Watchdog Tests
// ============================================================================

void test_watchdog_detects_low_memory(void) {
    MemoryWatchdog watchdog;

    set_mock_heap(25000, 20000);

    TEST_ASSERT_TRUE(watchdog.isMemoryPressure());
}

void test_watchdog_ok_with_sufficient_memory(void) {
    MemoryWatchdog watchdog;

    set_mock_heap(100000, 80000);

    TEST_ASSERT_FALSE(watchdog.isMemoryPressure());
}

void test_watchdog_detects_fragmentation(void) {
    MemoryWatchdog watchdog;

    set_mock_heap(100000, 20000);

    TEST_ASSERT_TRUE(watchdog.isFragmented());
}

void test_watchdog_restart_rate_limited(void) {
    MemoryWatchdog watchdog;

    TEST_ASSERT_TRUE(watchdog.canRestart());
    watchdog.recordRestart();

    TEST_ASSERT_TRUE(watchdog.canRestart());
    watchdog.recordRestart();

    TEST_ASSERT_TRUE(watchdog.canRestart());
    watchdog.recordRestart();

    TEST_ASSERT_FALSE(watchdog.canRestart());
}

void test_watchdog_restart_window_reset(void) {
    MemoryWatchdog watchdog;

    watchdog.recordRestart();
    watchdog.recordRestart();
    watchdog.recordRestart();
    TEST_ASSERT_FALSE(watchdog.canRestart());

    watchdog.resetRestartCount();
    TEST_ASSERT_TRUE(watchdog.canRestart());
}

void test_memory_pressure_transitions(void) {
    MemoryWatchdog watchdog;

    set_mock_heap(100000, 80000);
    TEST_ASSERT_FALSE(watchdog.isMemoryPressure());
    TEST_ASSERT_FALSE(watchdog.isFragmented());

    set_mock_heap(50000, 45000);
    TEST_ASSERT_FALSE(watchdog.isMemoryPressure());

    set_mock_heap(35000, 30000);
    TEST_ASSERT_FALSE(watchdog.isMemoryPressure());

    set_mock_heap(25000, 20000);
    TEST_ASSERT_TRUE(watchdog.isMemoryPressure());

    set_mock_heap(60000, 50000);
    TEST_ASSERT_FALSE(watchdog.isMemoryPressure());
}

void test_fragmentation_patterns(void) {
    MemoryWatchdog watchdog;

    set_mock_heap(100000, 80000);
    TEST_ASSERT_FALSE(watchdog.isFragmented());

    set_mock_heap(100000, 51000);
    TEST_ASSERT_FALSE(watchdog.isFragmented());

    set_mock_heap(100000, 49000);
    TEST_ASSERT_TRUE(watchdog.isFragmented());

    set_mock_heap(100000, 20000);
    TEST_ASSERT_TRUE(watchdog.isFragmented());

    set_mock_heap(29000, 27000);
    TEST_ASSERT_FALSE(watchdog.isFragmented());
    TEST_ASSERT_TRUE(watchdog.isMemoryPressure());
}

void test_simultaneous_pressure_and_fragmentation(void) {
    MemoryWatchdog watchdog;

    set_mock_heap(28000, 8000);

    TEST_ASSERT_TRUE(watchdog.isMemoryPressure());
    TEST_ASSERT_TRUE(watchdog.isFragmented());

    TEST_ASSERT_TRUE(watchdog.getFreeHeap() < 30000);
    TEST_ASSERT_TRUE(watchdog.getLargestBlock() < watchdog.getFreeHeap() / 2);
}

// ============================================================================
// Eviction Scorer Tests
// ============================================================================

void test_eviction_score_higher_for_included_device(void) {
    EvictionScorer scorer;

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

    float recentScore = scorer.calculate(ID_TYPE_MISC, false, false, 1000, 5.0f, 10);
    float oldScore = scorer.calculate(ID_TYPE_MISC, false, false, 300000, 5.0f, 10);

    TEST_ASSERT_TRUE(recentScore > oldScore);
}

void test_eviction_score_higher_for_closer_device(void) {
    EvictionScorer scorer;

    float closeScore = scorer.calculate(ID_TYPE_MISC, false, false, 1000, 1.0f, 10);
    float farScore = scorer.calculate(ID_TYPE_MISC, false, false, 1000, 10.0f, 10);

    TEST_ASSERT_TRUE(closeScore > farScore);
}

void test_eviction_prefers_weak_old_devices(void) {
    EvictionScorer scorer;

    float keepScore = scorer.calculate(ID_TYPE_IBEACON, false, false, 1000, 1.0f, 10);
    float evictScore = scorer.calculate(ID_TYPE_MISC, false, false, 600000, 20.0f, 2);

    TEST_ASSERT_TRUE(keepScore > evictScore);
    TEST_ASSERT_TRUE(keepScore > evictScore * 5);
}

// ============================================================================
// Configuration and Integration Tests
// ============================================================================

void test_tiered_memory_config_defaults(void) {
    TieredMemoryConfig config;

    TEST_ASSERT_TRUE(config.enabled);
    TEST_ASSERT_EQUAL(64, config.maxHot);
    TEST_ASSERT_EQUAL(128, config.maxCold);
    TEST_ASSERT_EQUAL(-90, config.minRssi);
    TEST_ASSERT_EQUAL(-85, config.minRssiCold);
    TEST_ASSERT_EQUAL(30000, config.criticalHeap);
    TEST_ASSERT_EQUAL(3, config.maxRestartsPerHour);
}

void test_backward_compatibility_when_disabled(void) {
    TieredMemoryConfig config;
    config.enabled = false;

    TieredMemoryManager manager(config);
    uint8_t mac[6] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC};

    ClassifyResult result = manager.classify(mac, nullptr, 0, -95);
    TEST_ASSERT_EQUAL(ClassifyResult::HOT, result);
}

// ============================================================================
// Stress Tests
// ============================================================================

void test_cold_tier_fill_and_evict_cycle(void) {
    ColdTier tier(32);

    for (int i = 0; i < 24; i++) {
        uint8_t mac[6] = {0xAA, 0xBB, (uint8_t)(i >> 8), (uint8_t)(i & 0xFF), 0x00, 0x00};
        bool inserted = tier.insert(mac, -70, i * 100);
        TEST_ASSERT_TRUE(inserted);
    }
    TEST_ASSERT_EQUAL(24, tier.count());

    for (int i = 0; i < 10; i++) {
        uint8_t mac[6] = {0xCC, 0xDD, (uint8_t)(i >> 8), (uint8_t)(i & 0xFF), 0x00, 0x00};
        tier.insert(mac, -60, 10000 + i * 100);
    }

    TEST_ASSERT_TRUE(tier.count() <= 24);
}

void test_cold_tier_tombstone_reuse(void) {
    ColdTier tier(16);

    uint8_t mac1[6] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};
    uint8_t mac2[6] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};

    tier.insert(mac1, -70, 0);
    tier.insert(mac2, -75, 0);
    TEST_ASSERT_EQUAL(2, tier.count());

    tier.remove(mac1);
    TEST_ASSERT_EQUAL(1, tier.count());

    ColdRecord* found = tier.lookup(mac2);
    TEST_ASSERT_NOT_NULL(found);

    uint8_t mac3[6] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x77};
    tier.insert(mac3, -65, 0);
    TEST_ASSERT_EQUAL(2, tier.count());

    TEST_ASSERT_NULL(tier.lookup(mac1));
    TEST_ASSERT_NOT_NULL(tier.lookup(mac2));
    TEST_ASSERT_NOT_NULL(tier.lookup(mac3));
}

void test_cold_tier_high_collision_scenario(void) {
    ColdTier tier(32);

    for (int i = 0; i < 20; i++) {
        uint8_t mac[6] = {0x00, 0x00, 0x00, 0x00, (uint8_t)(i >> 8), (uint8_t)(i & 0xFF)};
        bool inserted = tier.insert(mac, -70 + (i % 20), i * 1000);
        TEST_ASSERT_TRUE(inserted);
    }

    for (int i = 0; i < 20; i++) {
        uint8_t mac[6] = {0x00, 0x00, 0x00, 0x00, (uint8_t)(i >> 8), (uint8_t)(i & 0xFF)};
        ColdRecord* found = tier.lookup(mac);
        TEST_ASSERT_NOT_NULL(found);
    }
}

void test_rapid_insert_remove_cycles(void) {
    ColdTier tier(64);

    for (int cycle = 0; cycle < 100; cycle++) {
        uint8_t mac[6] = {
            (uint8_t)((cycle >> 8) & 0xFF),
            (uint8_t)(cycle & 0xFF),
            0x00, 0x00, 0x00, 0x00
        };

        bool inserted = tier.insert(mac, -70, cycle * 10);
        TEST_ASSERT_TRUE(inserted);

        ColdRecord* found = tier.lookup(mac);
        TEST_ASSERT_NOT_NULL(found);

        bool removed = tier.remove(mac);
        TEST_ASSERT_TRUE(removed);

        found = tier.lookup(mac);
        TEST_ASSERT_NULL(found);
    }

    TEST_ASSERT_EQUAL(0, tier.count());
}

void test_double_remove_no_underflow(void) {
    // Safety: double-remove should not cause count underflow
    ColdTier tier(16);
    uint8_t mac[6] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};

    // Insert one record
    bool inserted = tier.insert(mac, -70, 1000);
    TEST_ASSERT_TRUE(inserted);
    TEST_ASSERT_EQUAL(1, tier.count());

    // First remove succeeds
    bool removed1 = tier.remove(mac);
    TEST_ASSERT_TRUE(removed1);
    TEST_ASSERT_EQUAL(0, tier.count());

    // Second remove fails (already tombstoned)
    bool removed2 = tier.remove(mac);
    TEST_ASSERT_FALSE(removed2);
    TEST_ASSERT_EQUAL(0, tier.count());  // Count should not underflow
}

void test_remove_nonexistent_mac(void) {
    // Safety: remove on non-existent MAC should not affect count
    ColdTier tier(16);
    uint8_t existing[6] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};
    uint8_t nonexistent[6] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};

    // Insert one record
    tier.insert(existing, -70, 1000);
    TEST_ASSERT_EQUAL(1, tier.count());

    // Try to remove non-existent MAC
    bool removed = tier.remove(nonexistent);
    TEST_ASSERT_FALSE(removed);
    TEST_ASSERT_EQUAL(1, tier.count());  // Count unchanged
}

// ============================================================================
// Test Runner
// ============================================================================

void setUp(void) {
    set_mock_heap(100000, 80000);
}

void tearDown(void) {
}

int main(int argc, char **argv) {
    UNITY_BEGIN();

    // ColdRecord
    RUN_TEST(test_ColdRecord_size_is_24_bytes);
    RUN_TEST(test_ColdRecord_fields_are_correct_size);
    RUN_TEST(test_ColdRecord_tombstone_flags);
    RUN_TEST(test_ColdRecord_rssi_average_no_overflow);
    RUN_TEST(test_cold_record_can_be_marked_promoted);

    // Hash Function
    RUN_TEST(test_fnv1a_hash_is_deterministic);
    RUN_TEST(test_fnv1a_hash_different_for_different_macs);

    // Classifier
    RUN_TEST(test_classifier_drops_weak_rssi);
    RUN_TEST(test_classifier_cold_for_moderate_rssi);
    RUN_TEST(test_classifier_hot_for_known_mac);
    RUN_TEST(test_classifier_hot_for_ibeacon);
    RUN_TEST(test_classifier_default_is_cold);
    RUN_TEST(test_classifier_under_memory_pressure);

    // Cold Tier
    RUN_TEST(test_cold_tier_insert_and_lookup);
    RUN_TEST(test_cold_tier_lookup_returns_null_for_missing);
    RUN_TEST(test_cold_tier_update_existing);
    RUN_TEST(test_cold_tier_evicts_oldest_when_full);
    RUN_TEST(test_cold_tier_remove_promoted_record);
    RUN_TEST(test_cold_tier_remove_nonexistent_returns_false);
    RUN_TEST(test_cold_tier_update_increments_seen_count);
    RUN_TEST(test_cold_tier_seen_count_caps_at_255);
    RUN_TEST(test_rssi_max_tracks_strongest_signal);
    RUN_TEST(test_promotion_threshold_behavior);

    // Memory Watchdog
    RUN_TEST(test_watchdog_detects_low_memory);
    RUN_TEST(test_watchdog_ok_with_sufficient_memory);
    RUN_TEST(test_watchdog_detects_fragmentation);
    RUN_TEST(test_watchdog_restart_rate_limited);
    RUN_TEST(test_watchdog_restart_window_reset);
    RUN_TEST(test_memory_pressure_transitions);
    RUN_TEST(test_fragmentation_patterns);
    RUN_TEST(test_simultaneous_pressure_and_fragmentation);

    // Eviction Scorer
    RUN_TEST(test_eviction_score_higher_for_included_device);
    RUN_TEST(test_eviction_score_higher_for_ibeacon);
    RUN_TEST(test_eviction_score_decays_with_age);
    RUN_TEST(test_eviction_score_higher_for_closer_device);
    RUN_TEST(test_eviction_prefers_weak_old_devices);

    // Configuration and Integration
    RUN_TEST(test_tiered_memory_config_defaults);
    RUN_TEST(test_backward_compatibility_when_disabled);

    // Stress Tests
    RUN_TEST(test_cold_tier_fill_and_evict_cycle);
    RUN_TEST(test_cold_tier_tombstone_reuse);
    RUN_TEST(test_cold_tier_high_collision_scenario);
    RUN_TEST(test_rapid_insert_remove_cycles);

    // Safety Tests (guard against underflow/invalid state)
    RUN_TEST(test_double_remove_no_underflow);
    RUN_TEST(test_remove_nonexistent_mac);

    return UNITY_END();
}
