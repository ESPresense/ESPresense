#pragma once

#include <Arduino.h>
#include <utility>

class Logger : public Stream {
public:
    static Logger& instance();

    using Print::write;

    // Stream interface
    size_t write(uint8_t) override;
    size_t write(const uint8_t* buffer, size_t size) override;
    int available() override;
    int read() override;
    int peek() override;
    void flush() override;

    // HardwareSerial passthrough methods
    template <typename... Args>
    auto begin(Args&&... args) -> decltype(((HardwareSerial*)nullptr)->begin(std::forward<Args>(args)...)) {
        return serial_.begin(std::forward<Args>(args)...);
    }

    template <typename... Args>
    auto end(Args&&... args) -> decltype(((HardwareSerial*)nullptr)->end(std::forward<Args>(args)...)) {
        return serial_.end(std::forward<Args>(args)...);
    }

    void setDebugOutput(bool enable);

    // TCP logging control
    void enableTcp(uint16_t port = 6053);
    void disableTcp();
    bool isTcpConnected() const;

    // Serial output control
    void enableSerial(bool enable);
    bool isSerialEnabled() const;

    // Direct serial access (if needed)
    HardwareSerial& raw();
    HardwareSerial* operator->();
    operator HardwareSerial&();

private:
    explicit Logger(HardwareSerial& serial);
    ~Logger() = default;
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    HardwareSerial& serial_;
    bool serialEnabled_ = true;
};

extern Logger& Log;
