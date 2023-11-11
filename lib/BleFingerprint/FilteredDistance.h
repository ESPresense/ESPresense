#ifndef RSSISMOOTHER_H
#define RSSISMOOTHER_H

#include <Arduino.h>

class FilteredDistance {
   public:
    FilteredDistance(float minCutoff = 1.0f, float beta = 0.0f, float dcutoff = 1.0f);
    void addMeasurement(float dist);
    const float getMedianDistance() const;
    const float getDistance() const;
    bool hasValue() const { return lastTime != 0;}

   private:
    static const size_t bufferSize = 5;
    std::pair<unsigned long, float> rssiBuffer[bufferSize];  // Fixed-size buffer
    size_t bufferIndex = 0;                                  // Current index in the buffer

    float minCutoff;
    float beta;
    float dcutoff;
    float x, dx;
    float lastDist;
    unsigned long lastTime;

    float getAlpha(float cutoff, float dT);
};

#endif  // RSSISMOOTHER_H
