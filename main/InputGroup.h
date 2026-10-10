#pragma once
#include <cstdint>
#include <string>
#include <vector>

// A counted set of GPIO inputs (Switch, Button): <prefix>_count, then <prefix>_<n>_type/pin/timeout
// for n = 1..count on the hardware endpoint. Each input publishes ON/OFF to <room>/<prefix>_<n>,
// held ON for its timeout after the last detection, and <room>/<prefix> is ON while any is.
class InputGroup {
   public:
    static constexpr int MAX = 4;

    InputGroup(const char* prefix, const char* label, const char* plural, int defaultCount);

    void connectToWifi();
    void setup();
    void serialReport();
    // True when the combined <room>/<prefix> state changed this pass.
    bool loop();
    bool sendDiscovery();
    bool sendOnline();
    bool command(const std::string& command, const std::string& pay);

    int count() const { return (int)inputs.size(); }
    // Debounced state of input n (1-based): HIGH, LOW, or -1 before the first read/out of range.
    int8_t value(int n) const;
    // Bit n-1 set while input n is ON.
    uint32_t mask() const;

   private:
    struct Input {
        int index;
        int8_t type = 0, pin = -1, detected = 1;
        float timeout = 0;
        int8_t last = -1;
        unsigned long lastMillis = 0;
    };
    std::string prefix, label, plural;
    int defaultCount;
    std::vector<Input> inputs;
    int8_t lastCombined = -1;
    bool online = false;

    std::string key(int n, const char* field) const;
};
