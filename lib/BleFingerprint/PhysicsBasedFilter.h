#ifndef PBF_H
#define PBF_H

#include <cstddef>
#include <algorithm>
#include <utility>
#include <bitset>
#include <numeric>

#define PBF_STATIONARY_WINDOW 15
#define PBF_MOVEMENT_DECISION_WINDOW 3
#define PBF_MOVEMENT_LOOKBACK_WINDOW 6

static_assert(PBF_STATIONARY_WINDOW > PBF_MOVEMENT_DECISION_WINDOW);
static_assert(PBF_MOVEMENT_LOOKBACK_WINDOW >= PBF_MOVEMENT_DECISION_WINDOW);

struct PhysicsBasedFilter {
    void update(float measuredDistance, float deltaTime) {
        //std::cout << " measured: " << distanceMeasured << "\n";

        if (!initialized_) {
            // use the first measurement to initialize the entire array because we
            // have no better values at this point
            std::fill(measurements_.begin(), measurements_.end(), measuredDistance);
            mean_ = measuredDistance;
            distance_ = measuredDistance;
            initialized_ = true;
            return;
        }

        // shift all existing measurements to the left and insert the new measurement at the end
        std::copy(measurements_.begin() + 1, measurements_.end(), measurements_.begin());
        measurements_.back() = measuredDistance;

        //std::cout << " mean: " << mean_ << " variance: " << variance_ << " stdev: " << stdev << "\n";

        // update mean and variance - this ignores the latest PBS_MOVEMENT_LOOKBACK_WINDOW values
        const auto laggedLatestMeasurement = *(measurements_.end() - PBF_MOVEMENT_LOOKBACK_WINDOW - 1);
        auto newMean = mean_ + (laggedLatestMeasurement - measurements_.front()) / static_cast<double>(measurements_.size() - PBF_MOVEMENT_LOOKBACK_WINDOW);
        varianceSum_ += (laggedLatestMeasurement - mean_) * (laggedLatestMeasurement - newMean) - (measurements_.front() - mean_) * (measurements_.front() - newMean);
        mean_ = newMean;

        movementTime_ = std::max(0.0f, movementTime_ - deltaTime);

        //if (movementTime_ > 0)
        //    std::cout << " still moving\n";

        // remember whether we're currently moving away from the node
        const bool movement = measuredDistance > mean_;
        movements_ <<= 1;
        movements_[0] = movement;

        //std::cout << " movement: " << movement << " movements: " << movements_ << "\n";

        if (movements_.all() && !movementTime_) {
            //std::cout << " detected movement\n";
            movementTime_ = 10;
        }

        if (!movementTime_)
            distance_ = *std::min_element(measurements_.begin(), measurements_.end());
        else
            distance_ = *std::min_element(measurements_.end() - PBF_MOVEMENT_DECISION_WINDOW - 1, measurements_.end());
    }

    float distance() const {
        return distance_;
    }

    float mean() const {
        return mean_;
    }

    float variance() const {
        return varianceSum_ / static_cast<float>(measurements_.size());
    }

private:
    bool initialized_ { false };
    std::array<float, PBF_STATIONARY_WINDOW> measurements_;
    double distance_;
    double mean_;
    double varianceSum_ { 0 };
    float movementTime_ { 0 };
    std::bitset<PBF_MOVEMENT_DECISION_WINDOW> movements_;
};

#endif /* PBF_H */
