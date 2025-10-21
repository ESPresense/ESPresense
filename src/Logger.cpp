#include "Logger.h"

#ifdef ARDUINO

#include <AsyncTCP.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <vector>

#ifdef Serial
#undef Serial
#endif

extern HardwareSerial Serial;

namespace {

// TCP logging state
AsyncServer* tcpServer = nullptr;
AsyncClient* tcpClient = nullptr;
SemaphoreHandle_t tcpWriteMutex = nullptr;
int (*originalVprintf)(const char*, va_list) = nullptr;
uint16_t tcpPort = 0;
Logger* loggerInstance = nullptr;

void SafeTcpWrite(const uint8_t* data, size_t length) {
    if (!data || length == 0) return;
    if (!tcpClient || !tcpClient->connected()) return;
    if (!tcpWriteMutex) return;

    if (xSemaphoreTake(tcpWriteMutex, portMAX_DELAY) == pdTRUE) {
        tcpClient->write(reinterpret_cast<const char*>(data), length);
        xSemaphoreGive(tcpWriteMutex);
    }
}

int LoggerVprintf(const char* format, va_list args) {
    if (!format) return 0;

    // Calculate required buffer size
    va_list argsForLength;
    va_copy(argsForLength, args);
    int required = vsnprintf(nullptr, 0, format, argsForLength);
    va_end(argsForLength);

    // Format and send to TCP client if connected
    if (required > 0 && tcpClient && tcpClient->connected()) {
        std::vector<char> buffer(static_cast<size_t>(required) + 1);
        va_list argsForBuffer;
        va_copy(argsForBuffer, args);
        vsnprintf(buffer.data(), buffer.size(), format, argsForBuffer);
        va_end(argsForBuffer);
        SafeTcpWrite(reinterpret_cast<const uint8_t*>(buffer.data()), static_cast<size_t>(required));
    }

    // Call original vprintf (or default if none was set)
    if (!originalVprintf) {
        va_list argsCopy;
        va_copy(argsCopy, args);
        int result = vprintf(format, argsCopy);
        va_end(argsCopy);
        return result;
    }

    va_list argsForOriginal;
    va_copy(argsForOriginal, args);
    int result = originalVprintf(format, argsForOriginal);
    va_end(argsForOriginal);
    return result;
}

void HandleTcpClient(void*, AsyncClient* client) {
    if (!client) return;

    // Disconnect any existing client
    if (tcpClient && tcpClient != client) {
        tcpClient->close(true);
    }

    tcpClient = client;

    // Handle disconnection
    tcpClient->onDisconnect([](void*, AsyncClient* c) {
        if (c == tcpClient) {
            tcpClient = nullptr;
        }
    }, nullptr);
}

}  // anonymous namespace

Logger::Logger(HardwareSerial& serial) : serial_(serial), serialEnabled_(true) {
    loggerInstance = this;
}

Logger& Logger::instance() {
    static Logger instance(Serial);
    return instance;
}

size_t Logger::write(uint8_t ch) {
    // Write to TCP if enabled
    SafeTcpWrite(&ch, 1);

    // Write to serial if enabled
    if (serialEnabled_) {
        return serial_.write(ch);
    }
    return 1;
}

size_t Logger::write(const uint8_t* buffer, size_t size) {
    // Write to TCP if enabled
    SafeTcpWrite(buffer, size);

    // Write to serial if enabled
    if (serialEnabled_) {
        return serial_.write(buffer, size);
    }
    return size;
}

int Logger::available() {
    return serial_.available();
}

int Logger::read() {
    return serial_.read();
}

int Logger::peek() {
    return serial_.peek();
}

void Logger::flush() {
    if (serialEnabled_) {
        serial_.flush();
    }
}

void Logger::setDebugOutput(bool enable) {
    serial_.setDebugOutput(enable);
}

void Logger::enableTcp(uint16_t port) {
    if (tcpServer) return;  // Already enabled

    tcpPort = port;
    tcpWriteMutex = xSemaphoreCreateMutex();
    tcpServer = new AsyncServer(tcpPort);
    tcpServer->onClient(HandleTcpClient, nullptr);
    tcpServer->begin();

    // Hook into ESP logging system
    originalVprintf = esp_log_set_vprintf(LoggerVprintf);
}

void Logger::disableTcp() {
    // Restore original vprintf
    if (originalVprintf) {
        esp_log_set_vprintf(originalVprintf);
        originalVprintf = nullptr;
    }

    // Close and cleanup server
    if (tcpServer) {
        tcpServer->end();
        delete tcpServer;
        tcpServer = nullptr;
    }

    // Cleanup mutex
    if (tcpWriteMutex) {
        vSemaphoreDelete(tcpWriteMutex);
        tcpWriteMutex = nullptr;
    }

    // Close client connection
    if (tcpClient) {
        tcpClient->close(true);
        tcpClient = nullptr;
    }
}

bool Logger::isTcpConnected() const {
    return tcpClient && tcpClient->connected();
}

void Logger::enableSerial(bool enable) {
    serialEnabled_ = enable;
}

bool Logger::isSerialEnabled() const {
    return serialEnabled_;
}

HardwareSerial& Logger::raw() {
    return serial_;
}

HardwareSerial* Logger::operator->() {
    return &serial_;
}

Logger::operator HardwareSerial&() {
    return serial_;
}

Logger& Log = Logger::instance();

#else  // !ARDUINO

// Non-Arduino stub implementation
Logger::Logger(HardwareSerial& serial) : serial_(serial) {}

Logger& Logger::instance() {
    static HardwareSerial dummySerial;
    static Logger instance(dummySerial);
    return instance;
}

size_t Logger::write(uint8_t) { return 1; }
size_t Logger::write(const uint8_t*, size_t size) { return size; }
int Logger::available() { return 0; }
int Logger::read() { return -1; }
int Logger::peek() { return -1; }
void Logger::flush() {}
void Logger::setDebugOutput(bool) {}
void Logger::enableTcp(uint16_t) {}
void Logger::disableTcp() {}
bool Logger::isTcpConnected() const { return false; }
void Logger::enableSerial(bool) {}
bool Logger::isSerialEnabled() const { return true; }
HardwareSerial& Logger::raw() { return serial_; }
HardwareSerial* Logger::operator->() { return &serial_; }
Logger::operator HardwareSerial&() { return serial_; }

Logger& Log = Logger::instance();

#endif  // ARDUINO
