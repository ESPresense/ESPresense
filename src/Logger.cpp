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

using SerialType = decltype(::Serial);

namespace {

// TCP logging state
AsyncServer* tcpServer = nullptr;
AsyncClient* tcpClient = nullptr;
SemaphoreHandle_t tcpWriteMutex = nullptr;
int (*originalVprintf)(const char*, va_list) = nullptr;
uint16_t tcpPort = 0;
Logger* loggerInstance = nullptr;
bool tcpEnabled = false;

/**
 * @brief Safely writes a byte buffer to the currently connected TCP client if available.
 *
 * Attempts to send `length` bytes from `data` to the active TCP client while serializing access
 * with an internal mutex. If the input is invalid, no client is connected, or the mutex is not
 * present, the function returns without performing any write.
 *
 * @param data Pointer to the buffer to send.
 * @param length Number of bytes to send from `data`.
 *
 * @note The function acquires an internal semaphore to serialize concurrent writes to the TCP client.
 */
void SafeTcpWrite(const uint8_t* data, size_t length) {
    if (!data || length == 0) return;
    if (!tcpEnabled) return;
    if (!tcpWriteMutex) return;

    if (xSemaphoreTake(tcpWriteMutex, portMAX_DELAY) == pdTRUE) {
        AsyncClient* client = tcpClient;
        if (client && client->connected()) {
            client->write(reinterpret_cast<const char*>(data), length);
        }
        xSemaphoreGive(tcpWriteMutex);
    }
}

/**
 * @brief Forwards a formatted log message to a connected TCP client (if any) and then invokes the original vprintf handler.
 *
 * @param format printf-style format string; if nullptr the function returns 0.
 * @param args variadic arguments matching `format`.
 * @return int Number of characters printed by the downstream vprintf call, or 0 if `format` is nullptr.
 */
int LoggerVprintf(const char* format, va_list args) {
    if (!format) return 0;

    // Calculate required buffer size
    va_list argsForLength;
    va_copy(argsForLength, args);
    int required = vsnprintf(nullptr, 0, format, argsForLength);
    va_end(argsForLength);

    // Format and send to TCP client if connected
    if (required > 0 && tcpEnabled && tcpClient && tcpClient->connected()) {
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

/**
 * @brief Accepts a new TCP client connection and installs lifecycle handling.
 *
 * If a different client is already connected, that client is disconnected. The supplied
 * client becomes the active TCP client and receives a disconnect handler that clears
 * the active client pointer when it disconnects.
 *
 * @param _ctx Unused callback context pointer.
 * @param client Pointer to the newly connected AsyncClient; ignored if null.
 */
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

}  /**
 * @brief Construct a Logger that forwards output to the provided serial port.
 *
 * Creates a Logger tied to the given HardwareSerial and enables serial output by default.
 * The constructed object becomes the global active Logger instance.
 *
 * @param serial Reference to the HardwareSerial used for serial logging.
 */

Logger::Logger(LoggerSerialType& serial) : serial_(serial), serialEnabled_(true) {
    loggerInstance = this;
}

/**
 * @brief Provides the process-wide Logger singleton bound to the default Serial.
 *
 * @return Logger& Reference to the singleton Logger associated with the global Serial interface.
 */
Logger& Logger::instance() {
    static Logger instance(::Serial);
    return instance;
}

/**
 * @brief Writes a single byte to the logger outputs.
 *
 * Sends the byte to the TCP client if TCP logging is enabled, and to the
 * underlying serial port if serial output is enabled.
 *
 * @param ch The byte to write.
 * @return size_t Number of bytes written to the serial port when serial is enabled; otherwise returns 1.
 */
size_t Logger::write(uint8_t ch) {
    // Write to TCP if enabled
    SafeTcpWrite(&ch, 1);

    // Write to serial if enabled
    if (serialEnabled_) {
        return serial_.write(ch);
    }
    return 1;
}

/**
 * @brief Sends a buffer to the active logging outputs.
 *
 * Sends the provided bytes to a connected TCP client if TCP logging is enabled,
 * and writes them to the underlying serial port if serial logging is enabled.
 *
 * @param buffer Pointer to the bytes to send.
 * @param size Number of bytes to send from `buffer`.
 * @return size_t Number of bytes written to the serial port if serial logging is enabled; otherwise returns `size`.
 */
size_t Logger::write(const uint8_t* buffer, size_t size) {
    // Write to TCP if enabled
    SafeTcpWrite(buffer, size);

    // Write to serial if enabled
    if (serialEnabled_) {
        return serial_.write(buffer, size);
    }
    return size;
}

/**
 * @brief Query how many bytes can be read without blocking from the underlying serial.
 *
 * @return int Number of bytes available to read from the underlying HardwareSerial; 0 if none.
 */
int Logger::available() {
    return serial_.available();
}

/**
 * @brief Read a single byte from the underlying serial input.
 *
 * @return The next byte as an integer in the range 0–255, or -1 if no data is available.
 */
int Logger::read() {
    return serial_.read();
}

/**
 * @brief Peeks at the next byte available from the serial input without removing it.
 *
 * @return int The next byte (0-255) as an int, or -1 if no data is available.
 */
int Logger::peek() {
    return serial_.peek();
}

/**
 * @brief Flushes the underlying serial output if serial logging is enabled.
 *
 * When serial logging is disabled this function has no effect.
 */
void Logger::flush() {
    if (serialEnabled_) {
        serial_.flush();
    }
}

/**
 * @brief Enables or disables debug output on the underlying serial port.
 *
 * @param enable true to enable debug output, false to disable it.
 */
void Logger::setDebugOutput(bool enable) {
    serial_.setDebugOutput(enable);
}

/**
 * @brief Enable TCP-based logging on the given port.
 *
 * Sets up a TCP server that accepts a single client, prepares a mutex for
 * synchronized TCP writes, and hooks the ESP logging vprintf handler so
 * formatted log output is forwarded to connected TCP clients.
 *
 * Calling this when TCP logging is already enabled has no effect.
 *
 * @param port TCP port to listen on for log client connections.
 */
void Logger::enableTcp(uint16_t port) {
    if (tcpEnabled) return;

    tcpPort = port;
    tcpWriteMutex = xSemaphoreCreateMutex();
    if (!tcpWriteMutex) return;

    tcpServer = new AsyncServer(tcpPort);
    if (!tcpServer) {
        vSemaphoreDelete(tcpWriteMutex);
        tcpWriteMutex = nullptr;
        return;
    }
    tcpServer->onClient(HandleTcpClient, nullptr);
    tcpServer->begin();

    // Hook into ESP logging system
    originalVprintf = esp_log_set_vprintf(LoggerVprintf);
    tcpEnabled = true;
}

/**
 * @brief Disables TCP logging and releases all associated resources.
 *
 * Restores any previously installed log vprintf hook, stops and destroys the TCP server,
 * deletes the TCP write mutex, and closes any active TCP client connection. Uses a finite
 * mutex timeout (5 seconds) to avoid hanging during shutdown; proceeds with cleanup even
 * if the mutex cannot be acquired, logging a warning in that case.
 *
 * Calling this when TCP logging is not enabled has no effect.
 */
void Logger::disableTcp() {
    if (!tcpEnabled) return;
    tcpEnabled = false;

    // Restore original vprintf first so no further writes arrive via the hook.
    if (originalVprintf) {
        esp_log_set_vprintf(originalVprintf);
        originalVprintf = nullptr;
    }

    // Snapshot client pointer under the mutex and clear the global reference.
    // Use finite timeout to avoid hanging during shutdown.
    AsyncClient* clientToClose = nullptr;
    if (tcpWriteMutex) {
        if (xSemaphoreTake(tcpWriteMutex, pdMS_TO_TICKS(5000)) == pdTRUE) {
            // Successfully acquired mutex - safely snapshot and clear
            clientToClose = tcpClient;
            tcpClient = nullptr;
            xSemaphoreGive(tcpWriteMutex);
        } else {
            // Timeout - proceed without mutex to avoid deadlock
            // This is safe because tcpEnabled is now false, preventing new writes
            log_w("Logger::disableTcp() mutex timeout - proceeding without lock");
            clientToClose = tcpClient;
            tcpClient = nullptr;
        }
    } else {
        // No mutex exists - just snapshot
        clientToClose = tcpClient;
        tcpClient = nullptr;
    }

    if (clientToClose) {
        clientToClose->close(true);
    }

    if (tcpServer) {
        tcpServer->end();
        delete tcpServer;
        tcpServer = nullptr;
    }

    if (tcpWriteMutex) {
        vSemaphoreDelete(tcpWriteMutex);
        tcpWriteMutex = nullptr;
    }
}

/**
 * @brief Check whether a TCP client is currently connected.
 *
 * @return `true` if a TCP client exists and its connection is active, `false` otherwise.
 */
bool Logger::isTcpConnected() const {
    return tcpClient && tcpClient->connected();
}

/**
 * @brief Enable or disable serial output for the logger.
 *
 * @param enable true to enable serial output, false to disable it.
 */
void Logger::enableSerial(bool enable) {
    serialEnabled_ = enable;
}

/**
 * @brief Reports whether the logger forwards output to the serial port.
 *
 * @return `true` if serial logging is enabled, `false` otherwise.
 */
bool Logger::isSerialEnabled() const {
    return serialEnabled_;
}

/**
 * @brief Get a reference to the underlying HardwareSerial used for serial I/O.
 *
 * @return HardwareSerial& Reference to the wrapped HardwareSerial instance.
 */
LoggerSerialType& Logger::raw() {
    return serial_;
}

/**
 * @brief Provide pointer-like access to the underlying HardwareSerial.
 *
 * @return HardwareSerial* Pointer to the underlying HardwareSerial instance.
 */
LoggerSerialType* Logger::operator->() {
    return &serial_;
}

/**
 * @brief Provide access to the underlying serial interface.
 *
 * @return HardwareSerial& Reference to the underlying HardwareSerial instance.
 */
Logger::operator LoggerSerialType&() {
    return serial_;
}

Logger& Log = Logger::instance();

#else  // !ARDUINO

/**
 * @brief Constructs a Logger that forwards operations to the given HardwareSerial in the non-Arduino stub.
 *
 * @param serial Reference to the underlying HardwareSerial used for stubbed logger operations.
 */
Logger::Logger(LoggerSerialType& serial) : serial_(serial) {}

/**
 * @brief Returns the process-wide Logger singleton instance.
 *
 * Provides a single shared Logger used when a platform-specific instance is not constructed elsewhere.
 *
 * @return Logger& Reference to the global Logger singleton.
 */
Logger& Logger::instance() {
    static LoggerSerialType dummySerial;
    static Logger instance(dummySerial);
    return instance;
}

/**
 * @brief Stub implementation that reports a single byte write.
 *
 * @return size_t `1` indicating one byte was accepted/written.
 */
size_t Logger::write(uint8_t) { return 1; }
/**
 * @brief Writes a buffer of bytes to the logger output.
 *
 * Writes up to `size` bytes from `buffer` to the logger. In non-Arduino builds this is a no-op that
 * reports the entire buffer as written.
 *
 * @param buffer Pointer to the bytes to write.
 * @param size Number of bytes to write.
 * @return size_t Number of bytes written (equal to `size`).
 */
size_t Logger::write(const uint8_t*, size_t size) { return size; }
/**
 * @brief Query how many bytes can be read from the logger's underlying serial buffer.
 *
 * On non-ARDUINO builds this stub implementation always reports no available data.
 *
 * @return int Number of bytes available to read; `0` if none.
 */
int Logger::available() { return 0; }
/**
 * @brief Attempt to read a byte from the logger input stream.
 *
 * @return int `-1` to indicate no data is available or end of stream.
 */
int Logger::read() { return -1; }
/**
 * @brief Peek at the next byte in the input buffer without removing it.
 *
 * @return `-1` if no data is available or input is unsupported, otherwise the next byte value (0–255).
 */
int Logger::peek() { return -1; }
/**
 * @brief Flushes any buffered serial output.
 *
 * Ensures any pending data queued for the underlying serial interface is transmitted.
 * When serial output is disabled or unavailable, this call has no effect.
 */
void Logger::flush() {}
/**
 * @brief Enable or disable debug output on the underlying serial interface.
 *
 * @param enable True to enable debug output, false to disable it.
 */
void Logger::setDebugOutput(bool) {}
/**
 * @brief Start TCP-based logging on the given port.
 *
 * Starts a TCP server that accepts a single client and forwards formatted log output
 * and write() data to the connected client. Integrates the logger with the ESP
 * logging vprintf hook so standard log messages are sent over TCP. Calling this
 * function when TCP logging is already enabled has no effect.
 *
 * @param port TCP port to listen on for log client connections.
 */
void Logger::enableTcp(uint16_t) {}
/**
 * @brief Disables TCP logging and releases all associated network resources.
 *
 * Restores the original logging vprintf hook (if it was overridden), stops and deletes the TCP server,
 * closes any active client connection, clears internal TCP pointers, and frees the TCP write mutex.
 */
void Logger::disableTcp() {}
/**
 * @brief Report whether a TCP client is currently connected.
 *
 * @return `true` if a TCP client is connected, `false` otherwise.
 */
bool Logger::isTcpConnected() const { return false; }
/**
 * @brief Enable or disable writing log output to the underlying serial port.
 *
 * @param enable `true` to enable writing to the underlying HardwareSerial, `false` to disable it.
 */
void Logger::enableSerial(bool) {}
/**
 * @brief Indicates whether serial output is enabled.
 *
 * @return `true` if serial output is enabled, `false` otherwise.
 */
bool Logger::isSerialEnabled() const { return true; }
/**
 * @brief Get the underlying HardwareSerial instance used by the logger.
 *
 * @return HardwareSerial& Reference to the underlying serial interface.
 */
LoggerSerialType& Logger::raw() { return serial_; }
/**
 * @brief Provides pointer-like access to the underlying serial instance.
 *
 * Allows using Logger with pointer syntax (e.g., Log->begin(...)) to call
 * HardwareSerial methods directly.
 *
 * @return LoggerSerialType* Pointer to the underlying serial instance.
 */
LoggerSerialType* Logger::operator->() { return &serial_; }
/**
 * @brief Accesses the underlying serial instance used by this Logger.
 *
 * @return LoggerSerialType& Reference to the underlying serial.
 */
Logger::operator LoggerSerialType&() { return serial_; }

Logger& Log = Logger::instance();

#endif  // ARDUINO
