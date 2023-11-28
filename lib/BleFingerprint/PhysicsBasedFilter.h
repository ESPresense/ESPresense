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
    float operator()(float measuredDistance, float deltaTime) {
        //std::cout << " measured: " << distanceMeasured << "\n";

        if (!initialized_) {
            // use the first measurement to initialize the entire array because we
            // have no better values at this point
            std::fill(measurements_.begin(), measurements_.end(), measuredDistance);
            mean_ = measuredDistance;
            initialized_ = true;
            return measuredDistance;
        }

        // shift all existing measurements to the left and insert the new measurement at the end
        std::copy(measurements_.begin() + 1, measurements_.end(), measurements_.begin());
        measurements_.back() = measuredDistance;

        //std::cout << " mean: " << mean_ << " variance: " << variance_ << " stdev: " << stdev << "\n";

        // update mean - this ignores the latest PBS_MOVEMENT_LOOKBACK_WINDOW values
        const auto laggedLatestMeasurement = *(measurements_.end() - PBF_MOVEMENT_LOOKBACK_WINDOW - 1);
        mean_ += (laggedLatestMeasurement - measurements_.front()) / static_cast<double>(measurements_.size() - PBF_MOVEMENT_LOOKBACK_WINDOW);

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
            return *std::min_element(measurements_.begin(), measurements_.end());
        else
            return *std::min_element(measurements_.end() - PBF_MOVEMENT_DECISION_WINDOW - 1, measurements_.end());
    }

private:
    bool initialized_ { false };
    std::array<float, PBF_STATIONARY_WINDOW> measurements_;
    double mean_;
    float movementTime_ { 0 };
    std::bitset<PBF_MOVEMENT_DECISION_WINDOW> movements_;
};

#endif /* PBF_H */
