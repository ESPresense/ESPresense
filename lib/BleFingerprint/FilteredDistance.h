#ifndef RSSISMOOTHER_H
#define RSSISMOOTHER_H

#include <Arduino.h>

#define SPIKE_THRESHOLD 1.0f  // Threshold for spike detection
#define NUM_READINGS 10       // Number of readings to keep track of

class FilteredDistance {
   public:
    FilteredDistance(float minCutoff = 1.0f, float beta = 0.0f, float dcutoff = 1.0f);
    void addMeasurement(float dist);
    const float getMedianDistance() const;
    const float getDistance() const;
    bool hasValue() const { return lastTime != 0; }

   private:
    float minCutoff;
    float beta;
    float dcutoff;
    float x, dx;
    float lastDist;
    unsigned long lastTime;

    float getAlpha(float cutoff, float dT);

    float readings[NUM_READINGS];  // Array to store readings
    int readIndex = 0;             // Current position in the array
    float total = 0;               // Total of the readings
    float average = 0;             // Average of the readings

    float removeSpike(float newValue);
};

#endif  // RSSISMOOTHER_H
