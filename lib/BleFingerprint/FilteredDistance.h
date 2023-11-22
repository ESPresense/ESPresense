#ifndef FILTEREDDISTANCE_H
#define FILTEREDDISTANCE_H

class FilteredDistance {
   public:
    FilteredDistance(float processNoise, float measurementNoise);
    void addMeasurement(float dist);
    const float getDistance() const;
    const bool hasValue() const { return !isFirstMeasurement; }

   private:
    float state[2];                // State: [0] is distance, [1] is rate of change in distance
    float covariance[2][2];        // State covariance
    unsigned long lastUpdateTime;  // Time of the last update
    float processNoise;            // Process noise (Q)
    float measurementNoise;        // Measurement noise (R)
    bool isFirstMeasurement;

    void prediction(float deltaTime);
    void update(float distanceMeasurement);
};

#endif  // FILTEREDDISTANCE_H
