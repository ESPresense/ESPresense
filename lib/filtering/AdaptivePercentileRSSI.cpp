#include "AdaptivePercentileRSSI.h"
#include <Arduino.h>
#include <algorithm>

AdaptivePercentileRSSI::AdaptivePercentileRSSI(uint32_t timeWindowMs, uint16_t initialMaxReadings)
    : timeWindowMs(timeWindowMs),
      maxReadings(initialMaxReadings),
      head(0),
      tail(0),
      count(0),
      totalReadings(0),
      lastRateCheck(0) {
    readings = new Reading[maxReadings];
}

AdaptivePercentileRSSI::~AdaptivePercentileRSSI() {
    delete[] readings;
}

AdaptivePercentileRSSI::AdaptivePercentileRSSI(const AdaptivePercentileRSSI& other)
    : timeWindowMs(other.timeWindowMs),
      maxReadings(other.maxReadings),
      head(other.head),
      tail(other.tail),
      count(other.count),
      totalReadings(other.totalReadings),
      lastRateCheck(other.lastRateCheck) {

    // Allocate new memory and copy the readings
    readings = new Reading[maxReadings];

    // Deep copy the readings array
    for (uint16_t i = 0; i < maxReadings; i++) {
        readings[i] = other.readings[i];
    }
}

AdaptivePercentileRSSI& AdaptivePercentileRSSI::operator=(const AdaptivePercentileRSSI& other) {
    if (this != &other) {
        // Handle self-assignment

        // Clean up existing resources
        delete[] readings;

        // Copy all member variables
        timeWindowMs = other.timeWindowMs;
        maxReadings = other.maxReadings;
        head = other.head;
        tail = other.tail;
        count = other.count;
        totalReadings = other.totalReadings;
        lastRateCheck = other.lastRateCheck;

        // Allocate new memory and copy the readings
        readings = new Reading[maxReadings];

        // Deep copy the readings array
        for (uint16_t i = 0; i < maxReadings; i++) {
            readings[i] = other.readings[i];
        }
    }
    return *this;
}

void AdaptivePercentileRSSI::addMeasurement(float rssi) {
    uint32_t currentTime = millis();
    totalReadings++;

    // Add new reading to the circular buffer
    if (count < maxReadings) {
        readings[head] = {rssi, currentTime};
        head = (head + 1) % maxReadings;
        count++;
    } else {
        readings[head] = {rssi, currentTime};
        head = (head + 1) % maxReadings;
        tail = (tail + 1) % maxReadings;
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
            tail = (tail + 1) % maxReadings;
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
    // Create new buffer
    Reading* newReadings = new Reading[newSize];

    // Copy existing readings to new buffer
    uint16_t newCount = min(count, newSize);
    uint16_t oldIdx = (tail + count - newCount) % maxReadings; // Start from most recent if we need to drop old readings

    for (uint16_t i = 0; i < newCount; i++) {
        newReadings[i] = readings[oldIdx];
        oldIdx = (oldIdx + 1) % maxReadings;
    }

    // Update pointers
    delete[] readings;
    readings = newReadings;
    maxReadings = newSize;
    head = newCount;
    tail = 0;
    count = newCount;
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

        idx = (idx + 1) % maxReadings;
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

uint16_t AdaptivePercentileRSSI::getReadingCount() {
    return count;
}

float AdaptivePercentileRSSI::getAverageInterval() {
    if (count < 2) return 0;

    uint32_t firstTimestamp = readings[tail].timestamp;
    uint32_t lastTimestamp = readings[(head + maxReadings - 1) % maxReadings].timestamp;

    return (lastTimestamp - firstTimestamp) / (float)(count - 1);
}

void AdaptivePercentileRSSI::setTimeWindow(uint32_t newTimeWindowMs) {
    timeWindowMs = newTimeWindowMs;
    removeExpiredReadings(millis());
    adjustBufferSize(millis());
}

float AdaptivePercentileRSSI::getVariance() {
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

        idx = (idx + 1) % maxReadings;
    }

    if (validCount < 2) return 0;

    // Calculate variance using the computational formula: Var(X) = E(X²) - (E(X))²
    float mean = sum / validCount;
    float variance = (sumSquared / validCount) - (mean * mean);

    // Handle potential floating-point errors that might result in tiny negative values
    return variance < 0 ? 0 : variance;
}
