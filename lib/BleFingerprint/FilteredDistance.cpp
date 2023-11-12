#include "FilteredDistance.h"

#include <Arduino.h>

#include <cmath>
#include <numeric>
#include <vector>

FilteredDistance::FilteredDistance(float minCutoff, float beta, float dcutoff)
    : minCutoff(minCutoff), beta(beta), dcutoff(dcutoff), x(0), dx(0), lastDist(0), lastTime(micros()) {
    for (size_t i = 0; i < NUM_READINGS; i++) {
        readings[i] = 0;
    }
}

float FilteredDistance::removeSpike(float newValue) {
    // Subtract the last reading:
    total = total - readings[readIndex];
    // Read the sensor:
    readings[readIndex] = newValue;
    // Add the reading to the total:
    total = total + readings[readIndex];
    // Advance to the next position in the array:
    readIndex = (readIndex + 1) % NUM_READINGS;

    // Calculate the average:
    average = total / NUM_READINGS;

    // Detect spike
    if (abs(newValue - average) > SPIKE_THRESHOLD) {
        // Spike detected, use the average as the filtered value
        return average;
    } else {
        // No spike, return the new value
        return newValue;
    }
}

void FilteredDistance::addMeasurement(float dist) {
    unsigned long now = micros();
    float dT = (now - lastTime) * 0.000001f;  // Convert microseconds to seconds
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
    float te = 1.0f / (1.0f + tau / dT);
    return te;
}
