#include "FilteredDistance.h"

#include <Arduino.h>

#include <cmath>
#include <numeric>
#include <vector>

FilteredDistance::FilteredDistance(float minCutoff, float beta, float dcutoff)
    : minCutoff(minCutoff), beta(beta), dcutoff(dcutoff), x(0), dx(0), lastDist(0), lastTime(micros()), total(0), average(0), readIndex(0) {
    for (size_t i = 0; i < NUM_READINGS; i++) {
        readings[i] = 0;
    }
}

float FilteredDistance::removeSpike(float newValue) {
    total -= readings[readIndex]; // Subtract the last reading
    readings[readIndex] = newValue; // Read the sensor
    total += readings[readIndex]; // Add the reading to the total
    readIndex = (readIndex + 1) % NUM_READINGS; // Advance to the next position in the array

    average = total / static_cast<float>(NUM_READINGS); // Calculate the average

    if (std::fabs(newValue - average) > SPIKE_THRESHOLD) { // Use fabs for floating-point
        return average; // Spike detected, use the average as the filtered value
    } else {
        return newValue; // No spike, return the new value
    }
}

void FilteredDistance::addMeasurement(float dist) {
    unsigned long now = micros();
    unsigned long elapsed = now - lastTime;
    float dT = elapsed * 0.000001f; // Convert microseconds to seconds
    lastTime = now;

    dT = std::max(dT, 0.05f);  // Enforce a minimum dT

    float alpha = getAlpha(minCutoff, dT);
    float dAlpha = getAlpha(dcutoff, dT);

    dist = removeSpike(dist);
    x += alpha * (dist - x);

    float dxTemp = (dist - lastDist) / dT;

    dx = dAlpha * (dx + (1 - dAlpha) * dxTemp);

    lastDist = x + beta * dx;
}


const float FilteredDistance::getDistance() const {
    return lastDist;
}

float FilteredDistance::getAlpha(float cutoff, float dT) {
    float tau = 1.0f / (2 * M_PI * cutoff);
    return 1.0f / (1.0f + tau / dT);
}
