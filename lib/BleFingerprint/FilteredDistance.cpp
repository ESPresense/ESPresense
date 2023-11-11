#include "FilteredDistance.h"

#include <Arduino.h>

#include <cmath>
#include <vector>

FilteredDistance::FilteredDistance(float minCutoff, float beta, float dcutoff)
    : timeConstant(1), minCutoff(minCutoff), beta(beta), dcutoff(dcutoff), x(0), dx(0), lastDist(0), lastTime(micros()) {
    for (size_t i = 0; i < bufferSize; ++i) {
        rssiBuffer[i] = std::make_pair(0, 0.0f);  // Initialize the buffer
    }
}

void FilteredDistance::addMeasurement(float dist) {
    unsigned long now = micros();
    float dT = (now - lastTime) * 0.000001f;  // Convert microseconds to seconds
    lastTime = now;

    dT = std::max(dT, 0.001f);  // Enforce a minimum dT

    float alpha = getAlpha(minCutoff, dT);
    float dAlpha = getAlpha(dcutoff, dT);

    x += alpha * (dist - x);

    float dxTemp = (dist - lastDist) / dT;
    dx = dAlpha * (dx + (1 - dAlpha) * dxTemp);

    lastDist = x + beta * dx;

    bufferIndex %= bufferSize;
    rssiBuffer[bufferIndex] = std::make_pair(now, lastDist);
    bufferIndex++;
}

const float FilteredDistance::getDistance() const {
    unsigned long now = micros();

    float total = 0.0f;
    float weightSum = 0.0f;
    std::vector<float> decayedWeights;

    // First, calculate all the decayed weights and sum them up
    for (size_t i = 0; i < bufferSize; ++i) {
        size_t index = (bufferIndex + bufferSize - i - 1) % bufferSize;
        unsigned long elementTimestamp = rssiBuffer[index].first;

        // Skip if the slot is uninitialized (timestamp is 0)
        if (elementTimestamp == 0) continue;

        float age = (now - elementTimestamp) * 0.000001f;  // Convert microseconds to seconds
        float decayedWeight = exp(-age / timeConstant);
        decayedWeights.push_back(decayedWeight);
        weightSum += decayedWeight;
    }

    // Check if weightSum is zero to avoid division by zero
    if (weightSum == 0) return lastDist;

    // Then, use the sum to normalize the weights and calculate the weighted sum
    for (size_t i = 0; i < decayedWeights.size(); ++i) {
        size_t index = (bufferIndex + bufferSize - i - 1) % bufferSize;
        float normalizedWeight = decayedWeights[i] / weightSum;
        total += rssiBuffer[index].second * normalizedWeight;
    }

    return total;
}

float FilteredDistance::getAlpha(float cutoff, float dT) {
    float tau = 1.0f / (2 * M_PI * cutoff);
    float te = 1.0f / (1.0f + tau / dT);
    return te;
}
