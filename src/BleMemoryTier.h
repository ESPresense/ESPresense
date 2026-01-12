/**
 * @file BleMemoryTier.h
 * @brief Tiered memory management for ESPresense BLE tracking
 *
 * This module implements a three-tier memory architecture to prevent OOM:
 * - HOT tier: Full BleFingerprint objects (~400 bytes) for valuable devices
 * - COLD tier: Lightweight ColdRecord structs (24 bytes) for less valuable devices
 * - DROP tier: Rejected early, no memory allocated
 *
 * Key design principles:
 * - Quick classification happens BEFORE heap allocation
 * - Stack-only operations in the hot path
 * - Single allocation at startup (no runtime malloc on MMU-less ESP32)
 * - Value-based eviction using piecewise linear scoring (no exp())
 * - Memory watchdog with safe restart via deep sleep
 * - PSRAM support for WROVER devices with automatic fallback
 */

#pragma once
#ifndef BLE_MEMORY_TIER_H
#define BLE_MEMORY_TIER_H

#include <cstdint>
#include <cstring>
#include <cmath>
#include <cstdio>
#include <new>

// Logging support - define LOG_MEMORY_TIER to enable debug output
#ifdef LOG_MEMORY_TIER
#include <Arduino.h>
#define MEM_LOG(fmt, ...) Serial.printf("[MemTier] " fmt "\n", ##__VA_ARGS__)
#else
#define MEM_LOG(fmt, ...) ((void)0)
#endif

#ifdef ESP32
#include <esp_heap_caps.h>
#include <esp_sleep.h>
#include <esp_system.h>
#else
// Native testing - these are mocked in test file
#ifndef UNIT_TEST
// Only declare if not already defined in test file
extern "C" {
    size_t heap_caps_get_free_size(uint32_t caps);
    size_t heap_caps_get_largest_free_block(uint32_t caps);
}
#endif
#define MALLOC_CAP_INTERNAL 0
#define MALLOC_CAP_SPIRAM 1
#endif

// ============================================================================
// ID Type Constants (matching BleFingerprint.h values)
// ============================================================================

#ifndef ID_TYPE_IBEACON
#define ID_TYPE_IBEACON 180
#endif

#ifndef ID_TYPE_KNOWN_MAC
#define ID_TYPE_KNOWN_MAC 210
#endif

#ifndef ID_TYPE_MISC
#define ID_TYPE_MISC 0
#endif

// ============================================================================
// Classification Result
// ============================================================================

enum class ClassifyResult : uint8_t {
    DROP = 0,   // Reject immediately, no memory allocated
    COLD = 1,   // Store in lightweight cold tier
    HOT = 2     // Promote to full BleFingerprint
};

// ============================================================================
// ColdRecord - 24-byte lightweight device record
// ============================================================================

/**
 * Compact device record for the cold tier.
 * MUST be exactly 24 bytes for predictable memory usage.
 * 128 records * 24 bytes = 3072 bytes (3KB)
 *
 * Design: Single allocation at startup, no runtime malloc.
 * On MMU-less ESP32, memory turnover causes fragmentation.
 */
struct __attribute__((packed)) ColdRecord {
    uint8_t mac[6];         // BLE MAC address (6 bytes)
    int8_t rssi;            // Last RSSI reading (1 byte)
    int8_t rssiMax;         // Maximum RSSI seen (1 byte)
    uint8_t addrType;       // BLE address type (1 byte)
    uint8_t idType;         // Classification hint (1 byte)
    uint8_t flags;          // State bitfield (1 byte)
    uint8_t seenCount;      // Times seen, capped at 255 (1 byte)
    uint32_t lastSeenMs;    // Last seen timestamp (4 bytes)
    uint32_t firstSeenMs;   // First seen timestamp (4 bytes)
    int8_t rssiAvg;         // Running average RSSI (1 byte) - NOT sum to avoid overflow
    uint8_t rssiSamples;    // Sample count for average (1 byte)
    uint8_t reserved[2];    // Future use, alignment (2 bytes)
    // Total: 6+1+1+1+1+1+1+4+4+1+1+2 = 24 bytes

    // Flag bits - CRITICAL: tombstone support for hash table correctness
    static constexpr uint8_t FLAG_EMPTY = 0x00;      // Never used slot
    static constexpr uint8_t FLAG_VALID = 0x01;      // Active record
    static constexpr uint8_t FLAG_TOMBSTONE = 0x02;  // Deleted but keep probing
    static constexpr uint8_t FLAG_PROMOTED = 0x04;   // Promoted to HOT tier

    bool isEmpty() const { return flags == FLAG_EMPTY; }
    bool isValid() const { return flags & FLAG_VALID; }
    bool isTombstone() const { return flags == FLAG_TOMBSTONE; }
    bool canStopProbing() const { return isEmpty(); }  // Only stop on truly empty
    bool canInsertHere() const { return isEmpty() || isTombstone(); }

    void setValid() { flags = FLAG_VALID; }
    void markTombstone() { flags = FLAG_TOMBSTONE; }  // CRITICAL: Don't clear(), use tombstone

    void clear() {
        memset(this, 0, sizeof(ColdRecord));
        // flags is now FLAG_EMPTY (0x00) - truly empty slot
    }

    // Update running average RSSI without overflow
    void updateRssiAverage(int8_t newRssi) {
        if (rssiSamples < 255) {
            // Exponential moving average: new_avg = old_avg * 0.875 + new * 0.125
            // Using integer math: (old * 7 + new) / 8
            int16_t newAvg = ((int16_t)rssiAvg * 7 + (int16_t)newRssi) / 8;
            rssiAvg = (int8_t)newAvg;
            rssiSamples++;
        }
    }
};

static_assert(sizeof(ColdRecord) == 24, "ColdRecord must be exactly 24 bytes");

// ============================================================================
// FNV-1a Hash Function
// ============================================================================

/**
 * FNV-1a hash for MAC address lookup.
 * Fast, deterministic, good distribution for small keys.
 */
inline uint32_t fnv1a_hash(const uint8_t* data, size_t len) {
    uint32_t hash = 2166136261u;  // FNV offset basis
    for (size_t i = 0; i < len; i++) {
        hash ^= data[i];
        hash *= 16777619u;  // FNV prime
    }
    return hash;
}

// ============================================================================
// Quick Classifier - Stack-only classification
// ============================================================================

/**
 * Quick classifier that determines device tier BEFORE heap allocation.
 * All operations are stack-based to avoid contributing to memory pressure.
 *
 * Memory footprint: 192 bytes for known MAC list + 8 bytes overhead = ~200 bytes
 * This is allocated ONCE at startup as part of TieredMemoryManager.
 */
class QuickClassifier {
public:
    static constexpr int8_t MIN_RSSI = -90;       // Drop below this
    static constexpr int8_t MIN_RSSI_COLD = -85;  // Cold tier below this

    QuickClassifier() : knownMacCount_(0) {
        memset(knownMacs_, 0, sizeof(knownMacs_));
    }

    /**
     * Add a MAC address to the known list (for HOT tier promotion).
     * @param mac 6-byte MAC address
     * @return true if added, false if list full
     */
    bool addKnownMac(const uint8_t* mac) {
        if (knownMacCount_ >= MAX_KNOWN_MACS) {
            MEM_LOG("Known MAC list full (%zu)", MAX_KNOWN_MACS);
            return false;
        }
        memcpy(knownMacs_[knownMacCount_], mac, 6);
        knownMacCount_++;
        MEM_LOG("Added known MAC: %02X:%02X:%02X:%02X:%02X:%02X",
                mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        return true;
    }

    /**
     * Classify a device based on MAC, advertisement data, and RSSI.
     * IMPORTANT: This runs in BLE callback context - keep it fast, no heap alloc.
     *
     * @param mac 6-byte MAC address
     * @param advData Advertisement data (may be null)
     * @param advLen Length of advertisement data
     * @param rssi Signal strength
     * @return ClassifyResult (DROP, COLD, or HOT)
     */
    ClassifyResult classify(const uint8_t* mac, const uint8_t* advData, size_t advLen, int8_t rssi) {
        // Drop weak signals immediately - most common case, check first
        if (rssi < MIN_RSSI) {
            return ClassifyResult::DROP;
        }

        // Known MACs always go HOT
        if (isKnownMac(mac)) {
            return ClassifyResult::HOT;
        }

        // Check for iBeacon signature
        if (isIBeacon(advData, advLen)) {
            return ClassifyResult::HOT;
        }

        // Moderate RSSI goes to COLD
        if (rssi < MIN_RSSI_COLD) {
            return ClassifyResult::COLD;
        }

        // Default: COLD tier for unknown devices with decent signal
        return ClassifyResult::COLD;
    }

private:
    static constexpr size_t MAX_KNOWN_MACS = 32;
    uint8_t knownMacs_[MAX_KNOWN_MACS][6];
    size_t knownMacCount_;

    bool isKnownMac(const uint8_t* mac) const {
        for (size_t i = 0; i < knownMacCount_; i++) {
            if (memcmp(knownMacs_[i], mac, 6) == 0) {
                return true;
            }
        }
        return false;
    }

    bool isIBeacon(const uint8_t* advData, size_t advLen) const {
        // iBeacon format: 0x02 0x15 followed by 21 bytes (UUID + Major + Minor + TX)
        if (advData == nullptr || advLen < 23) return false;
        return (advData[0] == 0x02 && advData[1] == 0x15);
    }
};

/**
 * Free function for simple classification without known MAC tracking.
 */
inline ClassifyResult quickClassify(const uint8_t* mac, const uint8_t* advData, size_t advLen, int8_t rssi) {
    (void)mac;  // Unused in simple version
    (void)advData;
    (void)advLen;

    // Drop weak signals immediately
    if (rssi < QuickClassifier::MIN_RSSI) {
        return ClassifyResult::DROP;
    }

    // Moderate RSSI goes to COLD
    if (rssi < QuickClassifier::MIN_RSSI_COLD) {
        return ClassifyResult::COLD;
    }

    // Default: COLD tier
    return ClassifyResult::COLD;
}

// ============================================================================
// Cold Tier - Hash table of ColdRecords with PSRAM support
// ============================================================================

/**
 * Cold tier storage using open addressing hash table with tombstone deletion.
 *
 * CRITICAL DESIGN DECISIONS:
 * 1. Single allocation at startup - no runtime malloc (MMU-less device safety)
 * 2. PSRAM preferred if available, fallback to internal SRAM
 * 3. Tombstone deletion to maintain hash chain integrity
 * 4. Fixed capacity for predictable memory usage
 */
class ColdTier {
public:
    static constexpr size_t DEFAULT_CAPACITY = 128;

    // Non-copyable, non-movable (owns heap memory)
    ColdTier(const ColdTier&) = delete;
    ColdTier& operator=(const ColdTier&) = delete;
    ColdTier(ColdTier&&) = delete;
    ColdTier& operator=(ColdTier&&) = delete;

    explicit ColdTier(size_t capacity = DEFAULT_CAPACITY)
        : capacity_(capacity > 0 ? capacity : 1), count_(0), tombstoneCount_(0),
          records_(nullptr), usingPsram_(false) {

        allocateStorage();
    }

    ~ColdTier() {
        freeStorage();
    }

    /**
     * Check if cold tier is operational (allocation succeeded).
     */
    bool isEnabled() const { return records_ != nullptr && capacity_ > 0; }
    bool isUsingPsram() const { return usingPsram_; }

    /**
     * Insert or update a device in the cold tier.
     * @param mac 6-byte MAC address
     * @param rssi Signal strength
     * @param nowMs Current timestamp
     * @return true if inserted/updated
     */
    bool insert(const uint8_t* mac, int8_t rssi, uint32_t nowMs) {
        if (!isEnabled()) return false;

        // Single probe pass: find existing OR first available slot
        size_t insertSlot = capacity_;  // Invalid sentinel
        ColdRecord* existing = nullptr;

        uint32_t hash = fnv1a_hash(mac, 6);
        size_t startSlot = hash % capacity_;

        for (size_t i = 0; i < capacity_; i++) {
            size_t slot = (startSlot + i) % capacity_;
            ColdRecord* record = &records_[slot];

            if (record->isValid()) {
                if (memcmp(record->mac, mac, 6) == 0) {
                    existing = record;
                    break;  // Found existing
                }
                // Continue probing
            } else if (record->canInsertHere()) {
                // First available slot (empty or tombstone)
                if (insertSlot == capacity_) {
                    insertSlot = slot;
                }
                if (record->canStopProbing()) {
                    break;  // Truly empty = end of chain, item not found
                }
                // Tombstone: remember slot but keep searching for existing
            }
        }

        if (existing) {
            // Update existing record
            existing->rssi = rssi;
            existing->lastSeenMs = nowMs;  // CRITICAL: Update timestamp for eviction
            existing->updateRssiAverage(rssi);
            if (rssi > existing->rssiMax) {
                existing->rssiMax = rssi;
            }
            if (existing->seenCount < 255) {
                existing->seenCount++;
            }
            MEM_LOG("Updated MAC %02X:..:%02X rssi=%d seen=%d",
                    mac[0], mac[5], rssi, existing->seenCount);
            return true;
        }

        // Check if we should evict (table > 75% full including tombstones)
        size_t totalUsed = count_ + tombstoneCount_;
        if (totalUsed >= (capacity_ * 3) / 4) {
            size_t evictedSlot = evictLowestValue(nowMs);
            // Use evicted slot if we don't have one yet
            if (insertSlot == capacity_ && evictedSlot < capacity_) {
                insertSlot = evictedSlot;
            }
        }

        // Need to insert new record
        if (insertSlot == capacity_) {
            // Table completely full (shouldn't happen with proper load factor)
            MEM_LOG("Hash table completely full!");
            return false;
        }

        // Insert new record
        ColdRecord* record = &records_[insertSlot];
        bool wasTombstone = record->isTombstone();

        record->clear();  // Reset to empty first
        memcpy(record->mac, mac, 6);
        record->rssi = rssi;
        record->rssiMax = rssi;
        record->rssiAvg = rssi;
        record->rssiSamples = 1;
        record->lastSeenMs = nowMs;
        record->firstSeenMs = nowMs;
        record->seenCount = 1;
        record->setValid();

        count_++;
        if (wasTombstone) {
            tombstoneCount_--;  // Reused a tombstone slot
        }

        MEM_LOG("Inserted MAC %02X:..:%02X at slot %zu (count=%zu, tomb=%zu)",
                mac[0], mac[5], insertSlot, count_, tombstoneCount_);
        return true;
    }

    /**
     * Look up a device by MAC address.
     * Uses tombstone-aware probing - only stops on truly empty slots.
     *
     * @param mac 6-byte MAC address
     * @return Pointer to record, or nullptr if not found
     */
    ColdRecord* lookup(const uint8_t* mac) {
        if (!isEnabled()) return nullptr;

        uint32_t hash = fnv1a_hash(mac, 6);
        size_t startSlot = hash % capacity_;

        // Linear probing with tombstone awareness
        for (size_t i = 0; i < capacity_; i++) {
            size_t slot = (startSlot + i) % capacity_;
            ColdRecord* record = &records_[slot];

            if (record->canStopProbing()) {
                return nullptr;  // Truly empty = end of chain
            }

            if (record->isValid() && memcmp(record->mac, mac, 6) == 0) {
                return record;
            }
            // Continue probing on tombstones and non-matching valid records
        }

        return nullptr;
    }

    /**
     * Update an existing record with new RSSI and timestamp.
     * @param mac 6-byte MAC address
     * @param rssi New signal strength
     * @param nowMs Current timestamp (CRITICAL for eviction)
     */
    void update(const uint8_t* mac, int8_t rssi, uint32_t nowMs = 0) {
        ColdRecord* record = lookup(mac);
        if (record) {
            record->rssi = rssi;
            if (nowMs > 0) {
                record->lastSeenMs = nowMs;  // CRITICAL: Update timestamp
            }
            record->updateRssiAverage(rssi);
            if (rssi > record->rssiMax) {
                record->rssiMax = rssi;
            }
            if (record->seenCount < 255) {
                record->seenCount++;
            }
        }
    }

    size_t count() const { return count_; }
    size_t capacity() const { return capacity_; }
    size_t tombstoneCount() const { return tombstoneCount_; }

    /**
     * Get memory usage statistics for monitoring.
     */
    void getStats(size_t& used, size_t& tombstones, size_t& total, bool& psram) const {
        used = count_;
        tombstones = tombstoneCount_;
        total = capacity_;
        psram = usingPsram_;
    }

private:
    size_t capacity_;
    size_t count_;
    size_t tombstoneCount_;
    ColdRecord* records_;
    bool usingPsram_;

    void allocateStorage() {
        size_t allocSize = sizeof(ColdRecord) * capacity_;

#ifdef ESP32
        // Strategy: Prefer PSRAM for cold tier (it's cold data!)
        // This frees precious internal SRAM for BLE/WiFi stack
        records_ = (ColdRecord*)heap_caps_malloc(allocSize, MALLOC_CAP_SPIRAM);
        if (records_) {
            usingPsram_ = true;
            MEM_LOG("Allocated %zu bytes in PSRAM for %zu records", allocSize, capacity_);
        } else {
            // Fallback to internal SRAM
            records_ = (ColdRecord*)heap_caps_malloc(allocSize, MALLOC_CAP_INTERNAL);
            usingPsram_ = false;
            if (records_) {
                MEM_LOG("Allocated %zu bytes in internal SRAM (no PSRAM)", allocSize, capacity_);
            } else {
                MEM_LOG("CRITICAL: Failed to allocate cold tier storage!");
                capacity_ = 0;
            }
        }
#else
        // Native testing
        records_ = new (std::nothrow) ColdRecord[capacity_];
        usingPsram_ = false;
#endif

        // Initialize all slots to empty
        if (records_) {
            for (size_t i = 0; i < capacity_; i++) {
                records_[i].clear();
            }
        }
    }

    void freeStorage() {
#ifdef ESP32
        if (records_) {
            heap_caps_free(records_);
        }
#else
        delete[] records_;
#endif
        records_ = nullptr;
    }

    /**
     * Evict the lowest-value record using scoring.
     * Uses tombstone marking to maintain hash chain integrity.
     * @return Slot index of evicted record, or capacity_ if none evicted
     */
    size_t evictLowestValue(uint32_t nowMs) {
        if (count_ == 0) return capacity_;

        float minScore = INFINITY;
        size_t victim = capacity_;

        for (size_t i = 0; i < capacity_; i++) {
            if (!records_[i].isValid()) continue;

            // Calculate simple eviction score (higher = keep, lower = evict)
            float score = calculateEvictionScore(records_[i], nowMs);

            if (score < minScore) {
                minScore = score;
                victim = i;
            }
        }

        if (victim < capacity_ && records_[victim].isValid()) {
            MEM_LOG("Evicting MAC %02X:..:%02X (score=%.1f) at slot %zu",
                    records_[victim].mac[0], records_[victim].mac[5], minScore, victim);

            // CRITICAL: Use tombstone, don't clear()!
            records_[victim].markTombstone();
            count_--;
            tombstoneCount_++;
        }

        // Tombstone reuse in insert() naturally manages table health.
        // No compaction needed - insert() reuses tombstone slots.

        return victim;
    }

    /**
     * Simple eviction score using integer math (ESP32 friendly).
     * Higher score = more valuable = less likely to evict.
     */
    float calculateEvictionScore(const ColdRecord& record, uint32_t nowMs) const {
        float score = 1.0f;

        // ID type bonus
        if (record.idType == ID_TYPE_KNOWN_MAC) {
            score += 100.0f;
        } else if (record.idType == ID_TYPE_IBEACON) {
            score += 50.0f;
        }

        // Age penalty (linear: -1 point per minute since last seen)
        uint32_t ageMs = nowMs - record.lastSeenMs;
        float ageMinutes = (float)ageMs / 60000.0f;
        score -= ageMinutes;

        // Seen count bonus (devices seen often are more valuable)
        if (record.seenCount > 10) {
            score += 5.0f;
        } else if (record.seenCount > 5) {
            score += 3.0f;
        }

        // RSSI bonus (stronger signal = closer = more valuable)
        if (record.rssiMax > -60) {
            score += 3.0f;
        } else if (record.rssiMax > -70) {
            score += 1.0f;
        }

        return score > 0 ? score : 0.0f;
    }
};

// ============================================================================
// Memory Watchdog with Deep Sleep Restart
// ============================================================================

/**
 * Memory watchdog that monitors heap and triggers safe restart if needed.
 *
 * Deep sleep strategy for SRAM clearing:
 * - ESP32 deep sleep clears most SRAM (only RTC slow mem preserved ~8KB)
 * - On wake, BLE/WiFi stack reinitializes with fresh memory
 * - Rate-limited to prevent boot loops
 */
class MemoryWatchdog {
public:
    static constexpr size_t CRITICAL_HEAP = 30000;       // 30KB threshold (conservative)
    static constexpr size_t WARNING_HEAP = 50000;        // 50KB warning threshold
    static constexpr float FRAGMENTATION_RATIO = 0.5f;   // Largest block < 50% of free = fragmented
    static constexpr int MAX_RESTARTS_PER_HOUR = 3;

    MemoryWatchdog() : restartCount_(0), lastCheckMs_(0), consecutiveLowCount_(0) {}

    /**
     * Check if system is under memory pressure.
     * @return true if free heap below critical threshold
     */
    bool isMemoryPressure() const {
        size_t freeHeap = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
        return freeHeap < CRITICAL_HEAP;
    }

    /**
     * Check if system is in warning zone (not critical but concerning).
     */
    bool isMemoryWarning() const {
        size_t freeHeap = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
        return freeHeap < WARNING_HEAP;
    }

    /**
     * Check if heap is fragmented.
     * @return true if largest free block is small relative to total free
     */
    bool isFragmented() const {
        size_t freeHeap = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
        size_t largestBlock = heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL);

        if (freeHeap == 0) return true;
        return (float)largestBlock / (float)freeHeap < FRAGMENTATION_RATIO;
    }

    /**
     * Check if restart is allowed (rate limited).
     * @return true if restart count below limit
     */
    bool canRestart() const {
        return restartCount_ < MAX_RESTARTS_PER_HOUR;
    }

    /**
     * Record a restart attempt.
     */
    void recordRestart() {
        restartCount_++;
    }

    /**
     * Reset restart counter (call after 1 hour).
     */
    void resetRestartCount() {
        restartCount_ = 0;
    }

    size_t getFreeHeap() const {
        return heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
    }

    size_t getLargestBlock() const {
        return heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL);
    }

    /**
     * Periodic check - call this from main loop every few seconds.
     * Returns true if restart was triggered.
     */
    bool check(uint32_t nowMs) {
        // Rate limit checks to every 5 seconds
        if (nowMs - lastCheckMs_ < 5000) {
            return false;
        }
        lastCheckMs_ = nowMs;

        size_t freeHeap = getFreeHeap();
        size_t largestBlock = getLargestBlock();

        MEM_LOG("Heap: %zu free, %zu largest, frag=%.1f%%",
                freeHeap, largestBlock,
                freeHeap > 0 ? (1.0f - (float)largestBlock / freeHeap) * 100 : 100.0f);

        if (isMemoryPressure()) {
            consecutiveLowCount_++;
            MEM_LOG("MEMORY PRESSURE! consecutive=%d", consecutiveLowCount_);

            // Trigger restart after 3 consecutive low readings (15 seconds)
            if (consecutiveLowCount_ >= 3 && canRestart()) {
                MEM_LOG("Triggering safe restart via deep sleep...");
                triggerSafeRestart();
                return true;
            }
        } else {
            consecutiveLowCount_ = 0;
        }

        return false;
    }

    /**
     * Trigger a safe restart using deep sleep.
     * Deep sleep clears most SRAM, giving a fresh start.
     */
    void triggerSafeRestart() {
#ifdef ESP32
        recordRestart();

        MEM_LOG("=== INITIATING DEEP SLEEP RESTART ===");
        MEM_LOG("Free heap before: %zu", getFreeHeap());

        // Very short deep sleep (100ms) to clear SRAM but wake quickly
        // Deep sleep clears all SRAM except RTC slow memory (~8KB)
        esp_sleep_enable_timer_wakeup(100 * 1000);  // 100ms in microseconds
        esp_deep_sleep_start();

        // Will not reach here - device restarts
#else
        MEM_LOG("Deep sleep restart not available on native platform");
#endif
    }

private:
    int restartCount_;
    uint32_t lastCheckMs_;
    int consecutiveLowCount_;
};

// ============================================================================
// Eviction Scorer
// ============================================================================

/**
 * Value-based eviction scoring using piecewise linear functions.
 * Higher score = more valuable = less likely to evict.
 * Uses no exp() or heavy math - ESP32 friendly.
 */
class EvictionScorer {
public:
    /**
     * Calculate eviction score for a device.
     * @param idType Device classification (iBeacon, known MAC, etc.)
     * @param isIncluded Is device in include filter
     * @param isExcluded Is device in exclude filter
     * @param ageMs Age since last seen (milliseconds)
     * @param distance Estimated distance (meters)
     * @param seenCount Number of times seen
     * @return Score (higher = more valuable, INFINITY = non-evictable)
     */
    float calculate(uint8_t idType, bool isIncluded, bool isExcluded,
                    uint32_t ageMs, float distance, uint8_t seenCount) const {
        // Excluded devices have lowest score
        if (isExcluded) {
            return 0.0f;
        }

        // Included devices are never evicted
        if (isIncluded) {
            return INFINITY;
        }

        float score = 1.0f;

        // ID type bonus (piecewise)
        if (idType == ID_TYPE_KNOWN_MAC) {
            score += 100.0f;
        } else if (idType == ID_TYPE_IBEACON) {
            score += 50.0f;
        }

        // Age penalty (linear decay: -1 point per minute)
        float ageMinutes = (float)ageMs / 60000.0f;
        score -= ageMinutes;

        // Distance penalty (linear: -1 point per meter beyond 2m)
        if (distance > 2.0f) {
            score -= (distance - 2.0f);
        }

        // Seen count bonus (logarithmic approximation using piecewise)
        if (seenCount > 10) {
            score += 5.0f;
        } else if (seenCount > 5) {
            score += 3.0f;
        } else if (seenCount > 1) {
            score += 1.0f;
        }

        return score > 0 ? score : 0.0f;
    }
};

// ============================================================================
// Configuration
// ============================================================================

/**
 * Configuration for tiered memory management.
 * Conservative defaults for stability.
 */
struct TieredMemoryConfig {
    bool enabled = true;
    size_t maxHot = 64;           // Max HOT tier devices
    size_t maxCold = 128;         // Max COLD tier records (reduced from 256)
    int8_t minRssi = -90;         // Drop below this RSSI
    int8_t minRssiCold = -85;     // COLD tier below this RSSI
    size_t criticalHeap = 30000;  // Increased from 20KB for more headroom
    int maxRestartsPerHour = 3;   // Rate limit restarts
};

// ============================================================================
// Tiered Memory Manager
// ============================================================================

/**
 * Main manager that coordinates all tiers.
 *
 * Memory footprint (single allocation at startup):
 * - QuickClassifier: ~200 bytes (stack, part of this object)
 * - ColdTier: 3KB in PSRAM or SRAM
 * - MemoryWatchdog: ~20 bytes (stack)
 * Total: ~3.2KB (mostly in PSRAM if available)
 */
class TieredMemoryManager {
public:
    // Non-copyable (contains non-copyable ColdTier)
    TieredMemoryManager(const TieredMemoryManager&) = delete;
    TieredMemoryManager& operator=(const TieredMemoryManager&) = delete;

    explicit TieredMemoryManager(const TieredMemoryConfig& config)
        : config_(config), coldTier_(config.maxCold) {

        MEM_LOG("TieredMemoryManager initialized: enabled=%d, maxCold=%zu, PSRAM=%d",
                config_.enabled, config_.maxCold, coldTier_.isUsingPsram());
    }

    /**
     * Classify a device and route to appropriate tier.
     * @param mac 6-byte MAC address
     * @param advData Advertisement data
     * @param advLen Advertisement data length
     * @param rssi Signal strength
     * @return Classification result
     */
    ClassifyResult classify(const uint8_t* mac, const uint8_t* advData,
                            size_t advLen, int8_t rssi) {
        if (!config_.enabled) {
            // Backward compatibility: everything goes to HOT
            return ClassifyResult::HOT;
        }

        return classifier_.classify(mac, advData, advLen, rssi);
    }

    /**
     * Periodic maintenance - call from main loop.
     */
    void tick(uint32_t nowMs) {
        watchdog_.check(nowMs);
    }

    QuickClassifier& classifier() { return classifier_; }
    ColdTier& coldTier() { return coldTier_; }
    MemoryWatchdog& watchdog() { return watchdog_; }
    const TieredMemoryConfig& config() const { return config_; }

    /**
     * Get status string for debugging.
     */
    void getStatus(char* buf, size_t bufLen) const {
        size_t used, tombs, total;
        bool psram;
        coldTier_.getStats(used, tombs, total, psram);

        snprintf(buf, bufLen,
                 "Cold:%zu/%zu(t%zu) Heap:%zu PSRAM:%s",
                 used, total, tombs,
                 watchdog_.getFreeHeap(),
                 psram ? "Y" : "N");
    }

private:
    TieredMemoryConfig config_;
    QuickClassifier classifier_;
    ColdTier coldTier_;
    MemoryWatchdog watchdog_;
};

#endif // BLE_MEMORY_TIER_H
