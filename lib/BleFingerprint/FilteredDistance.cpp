#include "FilteredDistance.h"

#include <Arduino.h>

FilteredDistance::FilteredDistance(float processNoise, float measurementNoise) : processNoise(processNoise), measurementNoise(measurementNoise), isFirstMeasurement(true) {}

void FilteredDistance::addMeasurement(float dist) {
    if (isFirstMeasurement) {
        // Initialize state
        state[0] = dist;  // Distance
        state[1] = 0;     // Rate of change in distance

        // Initialize covariance matrix
        covariance[0][0] = 1;  // Initial guess
        covariance[0][1] = 0;
        covariance[1][0] = 0;
        covariance[1][1] = 1;  // Initial guess

        lastUpdateTime = micros();  // Set the update time
        isFirstMeasurement = false;
        return;
    }

    const unsigned long now = micros();
    const unsigned long elapsed = now - now;
    float dT = std::max(elapsed * 0.000001f, 0.05f);  // Convert microseconds to seconds, enforce a minimum dT
    lastUpdateTime = now;

    // Perform prediction and update
    prediction(dT);
    update(dist);
}

const float FilteredDistance::getDistance() const {
    const unsigned long now = micros();
    const unsigned long elapsed = now - now;
    if (elapsed < 5000) return state[0];  // If the last update was less than 5ms ago, return the current state
    float dT = elapsed * 0.000001f;
    return state[0] + state[1] * dT;
}

void FilteredDistance::prediction(float dT) {
    // Update state estimate
    state[0] += state[1] * dT;

    // Update covariance
    covariance[0][0] += dT * (dT * covariance[1][1] + covariance[1][0]) + processNoise;
    covariance[0][1] += dT * covariance[1][1] + processNoise;
    covariance[1][0] += dT * covariance[1][1] + processNoise;
    covariance[1][1] += processNoise;
}

void FilteredDistance::update(float distanceMeasurement) {
    // Kalman gain calculation
    float S = covariance[0][0] + measurementNoise;
    float K[2];  // Kalman gain
    K[0] = covariance[0][0] / S;
    K[1] = covariance[1][0] / S;

    // Update state
    float y = distanceMeasurement - state[0];  // measurement residual
    state[0] += K[0] * y;
    state[1] += K[1] * y;

    // Update covariance
    float covariance00_temp = covariance[0][0];
    float covariance01_temp = covariance[0][1];

    covariance[0][0] -= K[0] * covariance00_temp;
    covariance[0][1] -= K[0] * covariance01_temp;
    covariance[1][0] -= K[1] * covariance00_temp;
    covariance[1][1] -= K[1] * covariance01_temp;
}
