#ifndef NATIVE_TEST_FIXTURES_H
#define NATIVE_TEST_FIXTURES_H

#include <stddef.h>

// ---------------------------------------------------------------------------
// Distance-formula fixtures.
//
// Each row pins one expected output of rssiToLogDistance(calRssi, rssi, n).
// Tolerances are set tight enough to catch a sign flip, base swap (10 -> 2/e),
// or path-loss-exponent placement drift, but loose enough to absorb float32
// rounding on host vs ESP32 toolchains.
//
// `board` is the ESPresense build target where this calibration is typical.
// All four tier-1 boards (esp32, esp32c3, esp32s3, esp32c6) get >=3 rows so
// CI fails per-board if anyone special-cases the formula for one chip and
// breaks another. The math itself is board-agnostic; the rows differ in
// which calRssi values are realistic for each chip's RX gain.
// ---------------------------------------------------------------------------
struct FormulaFixture {
    const char* name;
    const char* board;
    float calRssi;
    float rssi;
    float absorption;
    float expectedDistance;  // meters
    float toleranceMeters;
    const char* note;
};

static const FormulaFixture kFormulaFixtures[] = {
    // --- Anchor cases: rssi == calRssi -> exactly 1.0 m for any absorption.
    {"esp32_at_1m_n2",       "esp32",   -59.0f, -59.0f, 2.0f,  1.0000f, 0.001f,
     "rssi==calRssi anchor; result must be exactly 1.0 m"},
    {"esp32c3_at_1m_n2_5",   "esp32c3", -65.0f, -65.0f, 2.5f,  1.0000f, 0.001f,
     "rssi==calRssi anchor (c3 1m typically -65)"},
    {"esp32s3_at_1m_n3",     "esp32s3", -62.0f, -62.0f, 3.0f,  1.0000f, 0.001f,
     "rssi==calRssi anchor (s3 1m typically -62)"},
    {"esp32c6_at_1m_n2",     "esp32c6", -67.0f, -67.0f, 2.0f,  1.0000f, 0.001f,
     "rssi==calRssi anchor (c6 1m typically -67)"},

    // --- Free-space (n=2.0): every 6.02 dB drop doubles distance.
    {"esp32_n2_6db_drop",    "esp32",   -59.0f, -65.02f, 2.0f, 2.0000f, 0.02f,
     "n=2 free-space; 6.02 dB drop -> 2.0 m"},
    {"esp32c3_n2_12db_drop", "esp32c3", -65.0f, -77.04f, 2.0f, 4.0000f, 0.05f,
     "n=2 free-space; 12.04 dB drop -> 4.0 m"},
    {"esp32s3_n2_20db_drop", "esp32s3", -62.0f, -82.0f,  2.0f, 10.0000f, 0.1f,
     "n=2 free-space; 20 dB drop -> 10.0 m"},
    {"esp32c6_n2_3db_drop",  "esp32c6", -67.0f, -70.0f,  2.0f, 1.4125f, 0.01f,
     "n=2 free-space; 3 dB drop -> ~1.412 m (10^0.15)"},

    // --- Indoor n=3.0: 9.03 dB drop doubles distance.
    {"esp32_n3_close",       "esp32",   -59.0f, -68.03f, 3.0f, 2.0000f, 0.02f,
     "n=3 indoor; 9.03 dB drop -> 2.0 m"},
    {"esp32c3_n3_4m",        "esp32c3", -65.0f, -83.06f, 3.0f, 4.0000f, 0.05f,
     "n=3 indoor; 18.06 dB drop -> 4.0 m"},

    // --- Heavy attenuation n=3.5 (through-wall scenarios).
    {"esp32s3_n3_5_2m",      "esp32s3", -62.0f, -72.535f, 3.5f, 2.0000f, 0.02f,
     "n=3.5 through-wall; 10.535 dB drop -> 2.0 m"},
    {"esp32c6_n3_5_3m",      "esp32c6", -67.0f, -83.71f,  3.5f, 3.0000f, 0.04f,
     "n=3.5 through-wall; 16.71 dB drop -> 3.0 m"},

    // --- Sub-1m: signal stronger than calRssi (closer than reference distance).
    {"esp32_close_quarter_m","esp32",   -59.0f, -47.0f,  2.0f, 0.2512f, 0.005f,
     "rssi 12 dB above calRssi -> 0.251 m (closer than 1m anchor)"},

    // --- Absorption guard: n<=0 should fall back to n=2 (free-space).
    {"absorption_zero_guard","esp32",   -59.0f, -65.02f, 0.0f, 2.0000f, 0.02f,
     "n=0 must NOT divide by zero; helper falls back to n=2"},
    {"absorption_neg_guard", "esp32",   -59.0f, -65.02f, -1.5f, 2.0000f, 0.02f,
     "negative n is meaningless; helper falls back to n=2"},
};
static const size_t kFormulaFixtureCount =
    sizeof(kFormulaFixtures) / sizeof(kFormulaFixtures[0]);

// ---------------------------------------------------------------------------
// Filter (AdaptivePercentileRSSI::getMedianIQR) fixtures.
//
// The class buffers RSSI samples in a 15s rolling window then computes a
// Tukey-fence-trimmed mean (k=1.5). These cases pin its behavior on
// synthetic streams. Real-RSSI captures from instrumented runs go in
// test/fixtures/*.csv (see fixtures/README.md) and can be lifted into
// this corpus once available.
// ---------------------------------------------------------------------------
struct FilterFixture {
    const char* name;
    const float* samples;
    size_t sampleCount;
    float expectedMedianIqr;  // expected getMedianIQR() output, dBm
    float toleranceDb;
    const char* note;
};

// Steady-state stream: every sample is -65 dBm. Trimmed mean must be -65.
static const float kSteadyMinus65[] = {
    -65, -65, -65, -65, -65, -65, -65, -65, -65, -65,
    -65, -65, -65, -65, -65, -65, -65, -65, -65, -65,
};

// One outlier at -90 in an otherwise -65 stream. Tukey fence (k=1.5) on
// a constant set has IQR=0 so any non-(-65) sample falls outside the
// fence and is dropped -> trimmed mean still -65.
static const float kOutlierStream[] = {
    -65, -65, -65, -65, -65, -65, -65, -65, -65, -90,
    -65, -65, -65, -65, -65, -65, -65, -65, -65, -65,
};

// Bimodal stream: half at -60, half at -70. IQR = 10, fence = [-85, -45],
// every sample survives, trimmed mean ~ -65.
static const float kBimodalStream[] = {
    -60, -70, -60, -70, -60, -70, -60, -70, -60, -70,
    -60, -70, -60, -70, -60, -70, -60, -70, -60, -70,
};

// Drifting stream from -70 down to -50 in 1 dB steps. Median 60.5,
// IQR ~10, fence wide enough that all samples survive.
static const float kDriftingStream[] = {
    -70, -69, -68, -67, -66, -65, -64, -63, -62, -61,
    -60, -59, -58, -57, -56, -55, -54, -53, -52, -51,
};

// Two heavy outliers (one each side). IQR = 0 on the steady core, both
// outliers fall outside fence and are dropped -> trimmed mean = core value.
static const float kTwoOutlierStream[] = {
    -40, -65, -65, -65, -65, -65, -65, -65, -65, -65,
    -65, -65, -65, -65, -65, -65, -65, -65, -65, -95,
};

static const FilterFixture kFilterFixtures[] = {
    {"filter_steady_minus65", kSteadyMinus65, sizeof(kSteadyMinus65)/sizeof(float),
     -65.0f, 0.01f, "constant -65 dBm -> trimmed mean -65"},
    {"filter_one_outlier",    kOutlierStream, sizeof(kOutlierStream)/sizeof(float),
     -65.0f, 0.01f, "one -90 outlier dropped by Tukey fence on constant core"},
    {"filter_bimodal",        kBimodalStream, sizeof(kBimodalStream)/sizeof(float),
     -65.0f, 0.5f,  "bimodal -60/-70 -> wide fence, mean ~ -65"},
    {"filter_drift_ramp",     kDriftingStream, sizeof(kDriftingStream)/sizeof(float),
     -60.5f, 0.5f,  "linear ramp -70..-51 -> mean of survivors ~ -60.5"},
    {"filter_two_outliers",   kTwoOutlierStream, sizeof(kTwoOutlierStream)/sizeof(float),
     -65.0f, 0.01f, "outliers on both sides dropped by Tukey fence"},
};
static const size_t kFilterFixtureCount =
    sizeof(kFilterFixtures) / sizeof(kFilterFixtures[0]);

#endif
