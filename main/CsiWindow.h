#pragma once
// How much a run of WiFi CSI frames varied, as one number. Movement in a room changes the shape
// of the channel from frame to frame; a still room leaves it nearly constant.
// No IDF dependency, so the host tests can cover it.
#include <math.h>
#include <stddef.h>
#include <stdint.h>

class CsiWindow {
   public:
    // One L-LTF has 64 subcarriers, stored as [imaginary, real] int8 pairs.
    static constexpr size_t kMaxSubcarriers = 64;
    static constexpr size_t kMaxBytes = kMaxSubcarriers * 2;

    // Adds one frame. A frame of a different length than the first in the window has a different
    // subcarrier layout, so it is skipped rather than compared.
    void add(const int8_t* csi, size_t len) {
        if (len > kMaxBytes) len = kMaxBytes;
        // The first two subcarriers are never usable: subcarrier 0 is DC, and the driver can flag
        // the first four bytes as invalid on the original ESP32.
        if (len < (kFirstSubcarrier + 2) * 2) return;
        const size_t subcarriers = len / 2 - kFirstSubcarrier;
        if (frames_ && subcarriers != subcarriers_) return;

        float amplitude[kMaxSubcarriers];
        float total = 0;
        for (size_t i = 0; i < subcarriers; i++) {
            const int imag = csi[(i + kFirstSubcarrier) * 2];
            const int real = csi[(i + kFirstSubcarrier) * 2 + 1];
            amplitude[i] = sqrtf(static_cast<float>(imag * imag + real * real));
            total += amplitude[i];
        }
        if (total <= 0) return;

        // Dividing by the frame's own mean cancels the receiver's gain control, which rescales
        // whole frames and would otherwise read as movement.
        const float scale = static_cast<float>(subcarriers) / total;
        subcarriers_ = subcarriers;
        frames_++;
        for (size_t i = 0; i < subcarriers; i++) {
            // Welford's running variance.
            const float x = amplitude[i] * scale;
            const float delta = x - mean_[i];
            mean_[i] += delta / static_cast<float>(frames_);
            m2_[i] += delta * (x - mean_[i]);
        }
    }

    uint32_t frames() const { return frames_; }

    // Mean per-subcarrier variance of the normalised amplitude, times 10^4 so that readings are
    // not all leading zeros. 0 with fewer than two frames.
    float variance() const {
        if (frames_ < 2) return 0;
        float sum = 0;
        for (size_t i = 0; i < subcarriers_; i++) sum += m2_[i];
        return sum / static_cast<float>(subcarriers_) / static_cast<float>(frames_ - 1) * 10000.0f;
    }

    void reset() {
        frames_ = 0;
        subcarriers_ = 0;
        for (size_t i = 0; i < kMaxSubcarriers; i++) mean_[i] = m2_[i] = 0;
    }

   private:
    static constexpr size_t kFirstSubcarrier = 2;

    uint32_t frames_ = 0;
    size_t subcarriers_ = 0;
    float mean_[kMaxSubcarriers] = {};
    float m2_[kMaxSubcarriers] = {};
};
