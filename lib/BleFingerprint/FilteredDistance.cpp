#include "FilteredDistance.h"

#include <Arduino.h>

#include <cmath>
#include <numeric>
#include <vector>

FilteredDistance::FilteredDistance(float minCutoff, float beta, float dcutoff)
    : minCutoff(minCutoff), beta(beta), dcutoff(dcutoff), x(0), dx(0), lastDist(0), lastTime(0), total(0), totalSquared(0), readIndex(0) {
}

void FilteredDistance::initSpike(float dist) {
    for (size_t i = 0; i < NUM_READINGS; i++) {
        readings[i] = dist;
    }
    total = dist * NUM_READINGS;
    totalSquared = dist * dist * NUM_READINGS;  // Initialize sum of squared distances
}

float FilteredDistance::removeSpike(float dist) {
    total -= readings[readIndex];                               // Subtract the last reading
    totalSquared -= readings[readIndex] * readings[readIndex];  // Subtract the square of the last reading

    readings[readIndex] = dist;                                 // Read the sensor
    total += readings[readIndex];                               // Add the reading to the total
    totalSquared += readings[readIndex] * readings[readIndex];  // Add the square of the reading

    readIndex = (readIndex + 1) % NUM_READINGS;  // Advance to the next position in the array

    auto average = total / static_cast<float>(NUM_READINGS);  // Calculate the average

    if (std::fabs(dist - average) > SPIKE_THRESHOLD)
        return average;  // Spike detected, use the average as the filtered value

    return dist;  // No spike, return the new value
}

void FilteredDistance::addMeasurement(float dist) {
    const bool initialized = lastTime != 0;
    const unsigned long now = micros();
    const unsigned long elapsed = now - lastTime;
    lastTime = now;

    if (!initialized) {
        x = dist;  // Set initial filter state to the first reading
        dx = 0;    // Initial derivative is unknown, so we set it to zero
        lastDist = dist;
        initSpike(dist);
    } else {
        float dT = std::max(elapsed * 0.000001f, 0.05f);  // Convert microseconds to seconds, enforce a minimum dT
        const float alpha = getAlpha(minCutoff, dT);
        const float dAlpha = getAlpha(dcutoff, dT);

        dist = removeSpike(dist);
        x += alpha * (dist - x);
        dx = dAlpha * ((dist - lastDist) / dT);
        lastDist = x + beta * dx;
    }
}

const float FilteredDistance::getDistance() const {
    return lastDist;
}

float FilteredDistance::getAlpha(float cutoff, float dT) {
    float tau = 1.0f / (2 * M_PI * cutoff);
    return 1.0f / (1.0f + tau / dT);
}

const float FilteredDistance::getVariance() const {
    auto mean = total / static_cast<float>(NUM_READINGS);
    auto meanOfSquares = totalSquared / static_cast<float>(NUM_READINGS);
    auto variance = meanOfSquares - (mean * mean);  // Variance formula: E(X^2) - (E(X))^2
    if (variance < 0.0f) return 0.0f;
    return variance;
}
