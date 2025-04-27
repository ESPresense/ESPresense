# ESPresense Adaptive Reporting Plan (v4 - Quantized Divisor)

## Problem

The current reporting logic (report every X seconds or on Y meters movement) can lead to inaccurate trilateration results. A single node might report a position change quickly based on its local measurement, causing the central server to update the device's location prematurely. Subsequent reports from other nodes, arriving later within the base interval, might then correct the position back, causing jitter.

## Goal

Implement an adaptive reporting interval based on detected movement, synchronized across all ESPresense nodes using NTP time, and incorporating time slots to distribute load. The reporting interval should decrease as movement increases, and the interval calculation should be quantized to promote report coalescence from different nodes observing similar movement.

## Implementation Plan

### 1. Time Source

*   Utilize NTP-synchronized time for all reporting schedule calculations.
*   Obtain current time using `gettimeofday()` from `<sys/time.h>`. Convert to milliseconds since epoch (`uint64_t`).

### Example Timings

Assuming default values:
*   `baseInterval` (`skipMs`) = 5000 ms
*   `movementDivisorScale` = 0.2 m
*   `maxDivisor` = 8.0

| Movement (m) | Calculated Divisor | Quantized Divisor (Power of 2) | Effective Divisor (Clamped 1-8) | Effective Interval (ms) |
|--------------|--------------------|--------------------------------|---------------------------------|-------------------------|
| 0.0          | 1.0                | 1.0                            | 1.0                             | 5000                    |
| 0.1          | ~1.41              | 2.0                            | 2.0                             | 2500                    |
| 0.25         | ~2.38              | 2.0                            | 2.0                             | 2500                    |
| 0.5          | ~5.66              | 8.0                            | 8.0                             | 625                     |
| 1.0          | 32.0               | 32.0                           | 8.0                             | 625                     |

*Note: The time slot delay (`slot_delay`) is added *after* calculating the effective interval.*
### 2. Constants and Configuration

*   **Add to `include/defaults.h`:**
    ```c++
    #define DEFAULT_MOVEMENT_DIVISOR_SCALE 0.2f // Movement (meters) required to double the divisor (halve the interval)
    #define DEFAULT_MAX_DIVISOR 8.0f          // Maximum divisor (e.g., 8 means fastest interval is base/8)
    ```
*   **Add to `BleFingerprintCollection` Class:**
    *   In `.h`:
        ```c++
        static float movementDivisorScale;
        static float maxDivisor;
        ```
    *   In `.cpp` (settings load):
        ```c++
        movementDivisorScale = HeadlessWiFiSettings.decimal("mov_divisor_scale", DEFAULT_MOVEMENT_DIVISOR_SCALE, "Movement (m) to double divisor");
        maxDivisor = HeadlessWiFiSettings.decimal("max_divisor", DEFAULT_MAX_DIVISOR, "Max divisor for reporting interval");
        ```

### 3. Time Slot Calculation (`BleFingerprint` Class)

*   **Add Member (in `.h`):**
    ```c++
    uint8_t timeSlot = 0; // Calculated time slot (0-3)
    ```
*   **Add Method (in `.cpp`):**
    ```c++
    void BleFingerprint::calculateTimeSlot() {
        if (id.isEmpty()) {
            timeSlot = 0;
            return;
        }
        // Use a better hash function that distributes across many more slots
        uint32_t hash = 5381; // djb2 hash initial value
        for (int i = 0; i < id.length(); i++) {
            hash = ((hash << 5) + hash) + id[i]; // hash * 33 + character
        }

        // Calculate timeslot between 0 and (numTimeSlots-1)
        timeSlot = hash % MAX_TIME_SLOTS;
    }
    ```
*   **Call `calculateTimeSlot()`:** Invoke this method within `BleFingerprint::setId()` whenever the `id` is assigned or changes.

### 4. Modify `BleFingerprint` State

*   **Change Member Type (in `.h`):** Replace `unsigned long lastReportedMillis = 0;` with:
    ```c++
    uint64_t lastReportedMillis_synced = 0; // Stores synchronized time (ms since epoch) of last report
    ```

### 5. Modify `BleFingerprint::report()` Logic (`src/BleFingerprint.cpp`)

*   **Include Headers:** Add `#include <sys/time.h>` and `#include <math.h>`.
*   **Replace Reporting Check:** Substitute the existing check (around lines 474-475) with the following logic:

    ```c++
    // Get current synchronized time
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    uint64_t now_synced = (uint64_t)tv.tv_sec * 1000 + (uint64_t)tv.tv_usec / 1000;

    // Check if reporting is needed
    if (lastReportedMillis_synced == 0) {
         // First report for this device since boot/reset, report immediately
    } else {
        // Calculate movement since last report
        float movement = abs(dist - lastReported);

        // Get base interval and divisor settings
        float baseInterval = (float)BleFingerprintCollection::skipMs;
        float scale = BleFingerprintCollection::movementDivisorScale;
        float maxDiv = BleFingerprintCollection::maxDivisor;

        // Calculate raw divisor based on movement (exponentially)
        float calculated_divisor = (movement <= 0.0f || scale <= 0.0f) ? 1.0f : powf(2.0f, movement / scale);

        // Quantize the divisor to the nearest power of 2
        float rounded_log2 = roundf(log2f(calculated_divisor));
        float power_of_2_divisor = powf(2.0f, rounded_log2);

        // Clamp the quantized divisor between 1.0 and maxDivisor
        float effective_divisor = max(1.0f, min(maxDiv, power_of_2_divisor));

        // Calculate effective interval
        float effective_interval = baseInterval / effective_divisor;

        // Calculate time slot delay based on the *base* interval
        float slot_delay = (float)timeSlot * (baseInterval / 4.0f);

        // Calculate the next scheduled report time using synchronized timestamps
        uint64_t next_report_time_synced = lastReportedMillis_synced + (uint64_t)effective_interval + (uint64_t)slot_delay;

        // Check if it's time to report
        if (now_synced < next_report_time_synced) {
            return false; // Not time yet, skip report
        }
    }

    // If we haven't returned false, it's time to report
    if (fill(doc)) {
        lastReportedMillis_synced = now_synced; // Update time of this report using synced time
        lastReported = dist;                  // Update distance at time of this report
        reported = true;                      // Mark as reported for this cycle
        return true;
    }

    // If fill(doc) failed
    return false;
    ```

### 6. Visualization (Reporting Logic Flow)

```mermaid
graph TD
    A[report() called] --> B{Ignore/Hidden/Reported?};
    B -- Yes --> Z[Return false];
    B -- No --> C{dist > maxDistance?};
    C -- Yes --> Z;
    C -- No --> D{First report? (lastReportedMillis_synced == 0)};
    D -- Yes --> K[Proceed to Report];
    D -- No --> E{Calculate Movement};
    E --> F{Calculate Raw Divisor};
    F --> G{Quantize Divisor (Round to Power of 2)};
    G --> H{Clamp Divisor (1.0 to maxDivisor)};
    H --> I{Calculate Effective Interval};
    I --> J{Calculate Slot Delay};
    J --> L{Calculate Next Report Time};
    L --> M{Is now_synced < Next Report Time?};
    M -- Yes --> Z;
    M -- No --> K;
    K --> N{fill(doc) successful?};
    N -- Yes --> O[Update lastReportedMillis_synced, lastReported, reported=true];
    O --> Y[Return true];
    N -- No --> Z;

    subgraph Calculations
        direction LR
        E1[movement = abs(dist - lastReported)] --> E;
        F1[calculated_divisor = pow(2, movement / scale)] --> F;
        G1[quantized_divisor = pow(2, round(log2(calculated_divisor)))] --> G;
        H1[effective_divisor = clamp(quantized_divisor, 1.0, maxDivisor)] --> H;
        I1[effective_interval = baseInterval / effective_divisor] --> I;
        J1[slot_delay = timeSlot * (baseInterval / 4.0)] --> J;
        L1[next_report_time_synced = lastReportedMillis_synced + effective_interval + slot_delay] --> L;
    end
