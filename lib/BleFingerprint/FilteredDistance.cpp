#include "FilteredDistance.h"
#include <Arduino.h>

FilteredDistance::FilteredDistance(float processNoise, float measurementNoise): processNoise(processNoise), measurementNoise(measurementNoise), isFirstMeasurement(true) {}

void FilteredDistance::addMeasurement(float dist) {
    if (isFirstMeasurement) {
        // Initialize state
        state[0] = dist; // Distance
        state[1] = 0;    // Rate of change in distance

        // Initialize covariance matrix
        covariance[0][0] = 1; // Initial guess
        covariance[0][1] = 0;
        covariance[1][0] = 0;
        covariance[1][1] = 1; // Initial guess

        lastUpdateTime = micros(); // Set the update time
        isFirstMeasurement = false;
        return;
    }

    // Calculate time delta for subsequent measurements
    unsigned long currentTime = micros();
    float deltaTime = (currentTime - lastUpdateTime) / 1.0e6; // Convert micros to seconds
    lastUpdateTime = currentTime;

    // Perform prediction and update
    prediction(deltaTime);
    update(dist);
}

const float FilteredDistance::getDistance() const {
    unsigned long currentTime = micros();
    float deltaTime = (currentTime - lastUpdateTime) / 1.0e6; // Convert micros to seconds

    // Calculate predicted distance
    float predictedDistance = state[0] + state[1] * deltaTime;

    return predictedDistance;
}

void FilteredDistance::prediction(float deltaTime) {
    // Update state estimate
    state[0] += state[1] * deltaTime;

    // Update covariance
    covariance[0][0] += deltaTime * (covariance[1][0] + covariance[0][1]) + processNoise;
    covariance[0][1] += deltaTime * covariance[1][1];
    covariance[1][0] += deltaTime * covariance[1][1];
}

void FilteredDistance::update(float distanceMeasurement) {
    // Kalman gain calculation
    float S = covariance[0][0] + measurementNoise;
    float K[2]; // Kalman gain
    K[0] = covariance[0][0] / S;
    K[1] = covariance[1][0] / S;

    // Update state
    float y = distanceMeasurement - state[0]; // measurement residual
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
