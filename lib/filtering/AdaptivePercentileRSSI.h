#ifndef ADAPTIVE_PERCENTILE_RSSI_H
#define ADAPTIVE_PERCENTILE_RSSI_H

#include <Arduino.h>

class AdaptivePercentileRSSI {
public:
    AdaptivePercentileRSSI(uint32_t timeWindowMs = 15000, uint16_t initialMaxReadings = 20);
    ~AdaptivePercentileRSSI();

    AdaptivePercentileRSSI(const AdaptivePercentileRSSI& other);
    AdaptivePercentileRSSI& operator=(const AdaptivePercentileRSSI& other);

    void addMeasurement(float rssi);
    float getDistanceVariance(float refRSSI, float pathLossExponent);
    float getRSSIVariance();
    float getPercentileRSSI(float percentile);
    float getP75RSSI();
    float getMedianIQR(float k = 1.5f);
    uint16_t getReadingCount();
    float getAverageInterval();
    void setTimeWindow(uint32_t newTimeWindowMs);

private:
    struct Reading {
        float rssi;
        uint32_t timestamp;
    };

    static constexpr uint16_t MIN_READINGS = 10;
    static constexpr uint16_t MAX_READINGS = 200;
    static constexpr uint32_t RATE_CHECK_INTERVAL_MS = 10000; // Check rate every 10 seconds

    Reading* readings;      // Dynamic array for readings
    uint32_t timeWindowMs;  // Time window in milliseconds
    uint16_t maxReadings;   // Current maximum readings
    uint16_t head;          // Index for newest reading
    uint16_t tail;          // Index for oldest reading
    uint16_t count;         // Current number of readings
    uint32_t totalReadings; // Total readings received since initialization
    uint32_t lastRateCheck; // Timestamp of last rate check

    void removeExpiredReadings(uint32_t currentTime);
    void adjustBufferSize(uint32_t currentTime);
    void resizeBuffer(uint16_t newSize);
};

#endif
