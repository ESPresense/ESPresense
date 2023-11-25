#ifndef RSSISMOOTHER_H
#define RSSISMOOTHER_H

#include <Arduino.h>

class RSSISmoother {
   public:
    RSSISmoother(float timeConstant, float minCutoff = 1.0f, float beta = 0.0f, float dcutoff = 1.0f);
    void addRSSIValue(float rssi);
    float getSmoothedRSSI();

   private:
    static const size_t bufferSize = 10;
    std::pair<unsigned long, float> rssiBuffer[bufferSize];  // Fixed-size buffer
    size_t bufferIndex = 0;                                  // Current index in the buffer

    float timeConstant;
    float minCutoff;
    float beta;
    float dcutoff;
    float x, dx;
    float lastRSSI;
    unsigned long lastTime;

    float getAlpha(float cutoff, float dT);
};

#endif  // RSSISMOOTHER_H
