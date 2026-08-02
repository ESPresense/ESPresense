#include "AdaptivePercentileRSSI.h"
#include <Arduino.h>
#include <algorithm>
#include <new>

AdaptivePercentileRSSI::AdaptivePercentileRSSI(uint32_t timeWindowMs, uint16_t initialMaxReadings)
    : timeWindowMs(timeWindowMs),
      maxReadings(initialMaxReadings),
      allocated(min(INITIAL_ALLOCATED, initialMaxReadings)),
      head(0),
      tail(0),
      count(0),
      totalReadings(0),
      lastRateCheck(0) {
    readings = new (std::nothrow) Reading[allocated];
}

AdaptivePercentileRSSI::~AdaptivePercentileRSSI() {
    delete[] readings;
}

AdaptivePercentileRSSI::AdaptivePercentileRSSI(const AdaptivePercentileRSSI& other)
    : timeWindowMs(other.timeWindowMs),
      maxReadings(other.maxReadings),
      allocated(other.allocated),
      head(other.head),
      tail(other.tail),
      count(other.count),
      totalReadings(other.totalReadings),
      lastRateCheck(other.lastRateCheck) {

    // Allocate new memory and deep-copy the (right-sized) buffer
    readings = new (std::nothrow) Reading[allocated];
    if (readings)
        for (uint16_t i = 0; i < allocated; i++) readings[i] = other.readings[i];
}

AdaptivePercentileRSSI& AdaptivePercentileRSSI::operator=(const AdaptivePercentileRSSI& other) {
    if (this != &other) {
        // Handle self-assignment

        // Clean up existing resources
        delete[] readings;

        // Copy all member variables
        timeWindowMs = other.timeWindowMs;
        maxReadings = other.maxReadings;
        allocated = other.allocated;
        head = other.head;
        tail = other.tail;
        count = other.count;
        totalReadings = other.totalReadings;
        lastRateCheck = other.lastRateCheck;

        // Allocate new memory and deep-copy the (right-sized) buffer
        readings = new (std::nothrow) Reading[allocated];
        if (readings)
            for (uint16_t i = 0; i < allocated; i++) readings[i] = other.readings[i];
    }
    return *this;
}

void AdaptivePercentileRSSI::addMeasurement(float rssi) {
    uint32_t currentTime = millis();
    totalReadings++;

    // Grow the buffer on demand toward the drop threshold, so a device holding few readings keeps
    // a small array instead of pre-allocating all of maxReadings. Lossless: the set of readings
    // kept (newest up to maxReadings, within the time window) is unchanged.
    if (count == allocated && allocated < maxReadings)
        growBuffer();

    // Add new reading to the circular buffer
    if (count < allocated) {
        readings[head] = {rssi, currentTime};
        head = (head + 1) % allocated;
        count++;
    } else {  // buffer full at the drop threshold: overwrite oldest
        readings[head] = {rssi, currentTime};
        head = (head + 1) % allocated;
        tail = (tail + 1) % allocated;
    }

    // Remove expired readings
    removeExpiredReadings(currentTime);

    // Periodically check and adjust buffer size based on incoming rate
    if (currentTime - lastRateCheck > RATE_CHECK_INTERVAL_MS) {
        adjustBufferSize(currentTime);
        lastRateCheck = currentTime;
    }
}

void AdaptivePercentileRSSI::removeExpiredReadings(uint32_t currentTime) {
    while (count > 0) {
        uint32_t age = currentTime - readings[tail].timestamp;

        // Handle uint32_t overflow
        if (age > timeWindowMs && age < 0xFFFFFFFF - timeWindowMs) {
            tail = (tail + 1) % allocated;
            count--;
        } else {
            break;
        }
    }
}

void AdaptivePercentileRSSI::adjustBufferSize(uint32_t currentTime) {
    // Calculate the rate of incoming readings (readings per second)
    float elapsedSeconds = (currentTime - readings[tail].timestamp) / 1000.0f;
    if (elapsedSeconds <= 0) return;

    float readingsPerSecond = count / elapsedSeconds;

    // Calculate ideal buffer size based on rate and time window
    uint16_t idealSize = ceil(readingsPerSecond * (timeWindowMs / 1000.0f)) * 1.5f; // 50% overhead

    // Limit to reasonable bounds
    idealSize = constrain(idealSize, MIN_READINGS, MAX_READINGS);

    // Only resize if there's a significant difference
    if (abs(idealSize - maxReadings) > maxReadings / 4) {
        resizeBuffer(idealSize);
    }
}

void AdaptivePercentileRSSI::resizeBuffer(uint16_t newSize) {
    // newSize is the new drop threshold. Growth toward a larger threshold is lazy (growBuffer);
    // here we only shrink the allocation when the threshold drops below it, dropping the oldest
    // readings that no longer fit — same as the old behavior, just without over-allocating.
    maxReadings = newSize;
    uint16_t newAlloc = min(allocated, newSize);
    if (newAlloc < INITIAL_ALLOCATED) newAlloc = min((uint16_t)INITIAL_ALLOCATED, newSize);
    if (newAlloc == allocated) return;

    Reading* newReadings = new (std::nothrow) Reading[newAlloc];
    if (!newReadings) return;

    uint16_t newCount = min(count, newAlloc);
    uint16_t oldIdx = (tail + count - newCount) % allocated; // start from the most recent newCount
    for (uint16_t i = 0; i < newCount; i++) {
        newReadings[i] = readings[oldIdx];
        oldIdx = (oldIdx + 1) % allocated;
    }

    delete[] readings;
    readings = newReadings;
    allocated = newAlloc;
    head = newCount % newAlloc;
    tail = 0;
    count = newCount;
}

void AdaptivePercentileRSSI::growBuffer() {
    uint16_t newAlloc = min((uint16_t)(allocated * 2), maxReadings);
    if (newAlloc <= allocated) return;

    Reading* newReadings = new (std::nothrow) Reading[newAlloc];
    if (!newReadings) return; // OOM: keep the smaller buffer (drops oldest early, but never crashes)

    for (uint16_t i = 0; i < count; i++)
        newReadings[i] = readings[(tail + i) % allocated];

    delete[] readings;
    readings = newReadings;
    allocated = newAlloc;
    tail = 0;
    head = count % newAlloc; // count < newAlloc here, so head = count
}

float AdaptivePercentileRSSI::getPercentileRSSI(float percentile) {
    if (count == 0) return 0;

    // Create temporary array for sorting
    float* values = new float[count];
    uint16_t validCount = 0;

    uint32_t currentTime = millis();
    uint16_t idx = tail;

    for (uint16_t i = 0; i < count; i++) {
        uint32_t age = currentTime - readings[idx].timestamp;

        if (age <= timeWindowMs || age > 0xFFFFFFFF - timeWindowMs) {
            values[validCount++] = readings[idx].rssi;
        }

        idx = (idx + 1) % allocated;
    }

    if (validCount == 0) {
        delete[] values;
        return 0;
    }

    std::sort(values, values + validCount);

    float index = percentile * (validCount - 1);
    uint16_t lowerIdx = static_cast<uint16_t>(index);
    float fraction = index - lowerIdx;

    float result;
    if (lowerIdx + 1 < validCount) {
        result = values[lowerIdx] * (1 - fraction) + values[lowerIdx + 1] * fraction;
    } else {
        result = values[lowerIdx];
    }

    delete[] values;
    return result;
}

float AdaptivePercentileRSSI::getP75RSSI() {
    return getPercentileRSSI(0.75f);
}
float AdaptivePercentileRSSI::getMedianIQR(float k /* = 1.5f */)
{
    if (count == 0) return 0.0f;

    // 1) Copy all current readings into a scratch array
    float* vals = new float[count];
    uint16_t idx = tail;

    for (uint16_t i = 0; i < count; ++i) {
        vals[i] = readings[idx].rssi;
        idx = (idx + 1) % allocated;
    }

    // 2) Sort
    std::sort(vals, vals + count);

    auto interp = [&](float p) -> float {
        float pos   = p * (count - 1);            // rank (0-based)
        uint16_t lo = static_cast<uint16_t>(pos);
        float frac  = pos - lo;
        return (lo + 1 < count)
               ? vals[lo] * (1 - frac) + vals[lo + 1] * frac
               : vals[lo];
    };

    float q1  = interp(0.25f);
    float med = interp(0.50f);
    float q3  = interp(0.75f);
    float iqr = q3 - q1;

    float lower = q1 - k * iqr;
    float upper = q3 + k * iqr;

    // 3) Mean of values inside Tukey fence
    float sum = 0.0f;
    uint16_t survivors = 0;
    for (uint16_t i = 0; i < count; ++i) {
        if (vals[i] >= lower && vals[i] <= upper) {
            sum += vals[i];
            ++survivors;
        }
    }

    delete[] vals;

    return survivors ? (sum / survivors) : med;   // fallback to median if all clipped
}

uint16_t AdaptivePercentileRSSI::getReadingCount() {
    return count;
}

float AdaptivePercentileRSSI::getAverageInterval() {
    if (count < 2) return 0;

    uint32_t firstTimestamp = readings[tail].timestamp;
    uint32_t lastTimestamp = readings[(head + allocated - 1) % allocated].timestamp;

    return (lastTimestamp - firstTimestamp) / (float)(count - 1);
}

void AdaptivePercentileRSSI::setTimeWindow(uint32_t newTimeWindowMs) {
    timeWindowMs = newTimeWindowMs;
    removeExpiredReadings(millis());
    adjustBufferSize(millis());
}

float AdaptivePercentileRSSI::getRSSIVariance() {
    if (count < 2) return 0;

    uint32_t currentTime = millis();
    uint16_t validCount = 0;
    float sum = 0;
    float sumSquared = 0;

    uint16_t idx = tail;

    // Calculate sum and sum of squares in one pass
    for (uint16_t i = 0; i < count; i++) {
        uint32_t age = currentTime - readings[idx].timestamp;

        // Include only readings within the time window
        if (age <= timeWindowMs || age > 0xFFFFFFFF - timeWindowMs) {
            sum += readings[idx].rssi;
            sumSquared += readings[idx].rssi * readings[idx].rssi;
            validCount++;
        }

        idx = (idx + 1) % allocated;
    }

    if (validCount < 2) return 0;

    // Calculate variance using the computational formula: Var(X) = E(X²) - (E(X))²
    float mean = sum / validCount;
    float variance = (sumSquared / validCount) - (mean * mean);

    // Handle potential floating-point errors that might result in tiny negative values
    return variance < 0 ? 0 : variance;
}

float AdaptivePercentileRSSI::getDistanceVariance(float refRSSI, float pathLossExponent) {
    if (count < 2) return 0;

    uint32_t currentTime = millis();
    uint16_t validCount = 0;
    float sumDistance = 0;
    float sumDistanceSquared = 0;

    uint16_t idx = tail;

    // Calculate sum and sum of squares of distances in one pass
    for (uint16_t i = 0; i < count; i++) {
        uint32_t age = currentTime - readings[idx].timestamp;

        // Include only readings within the time window
        if (age <= timeWindowMs || age > 0xFFFFFFFF - timeWindowMs) {
            // Convert RSSI to distance
            float rssi = readings[idx].rssi;
            float exponent = (refRSSI - rssi) / (10.0f * pathLossExponent);
            float distance = pow(10.0f, exponent); // d = 10^((P0 - RSSI) / (10 * n))

            sumDistance += distance;
            sumDistanceSquared += distance * distance;
            validCount++;
        }

        idx = (idx + 1) % allocated;
    }

    if (validCount < 2) return 0;

    // Calculate variance using the computational formula: Var(d) = E(d²) - (E(d))²
    float meanDistance = sumDistance / validCount;
    float varianceDistance = (sumDistanceSquared / validCount) - (meanDistance * meanDistance);

    // Handle potential floating-point errors
    return varianceDistance < 0 ? 0 : varianceDistance;
}
