#include "FilteredDistance.h"

#include <Arduino.h>

#include <cmath>
#include <vector>
#include <numeric>

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
    std::vector<float> distances;

    // Gather all distances
    for (size_t i = 0; i < bufferSize; ++i) {
        size_t index = (bufferIndex + bufferSize - i - 1) % bufferSize;
        unsigned long elementTimestamp = rssiBuffer[index].first;

        if (elementTimestamp == 0) continue; // Skip uninitialized slots

        distances.push_back(rssiBuffer[index].second);
    }

    if (distances.empty()) return lastDist;

    // Calculate median of distances
    std::nth_element(distances.begin(), distances.begin() + distances.size() / 2, distances.end());
    float medianDistance = distances[distances.size() / 2];

    // Filter out distances significantly greater than the median
    std::vector<float> filteredDistances;
    for (float distance : distances) {
        if (distance <= medianDistance * 1.2) { // Adjust multiplier as needed
            filteredDistances.push_back(distance);
        }
    }

    // Compute average of filtered distances
    if (filteredDistances.empty()) return lastDist;

    float sum = std::accumulate(filteredDistances.begin(), filteredDistances.end(), 0.0f);
    return sum / filteredDistances.size();
}


float FilteredDistance::getAlpha(float cutoff, float dT) {
    float tau = 1.0f / (2 * M_PI * cutoff);
    float te = 1.0f / (1.0f + tau / dT);
    return te;
}
