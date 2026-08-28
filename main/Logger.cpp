#include "Logger.h"

#include <lwip/sockets.h>
#include <unistd.h>

#include <cstdio>
#include <cstring>
#include <vector>

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

namespace {
int tcpClient = -1;
SemaphoreHandle_t tcpMutex = nullptr;
vprintf_like_t originalVprintf = nullptr;

void tcpWrite(const uint8_t* data, size_t len) {
    if (tcpClient < 0 || !tcpMutex) return;
    if (xSemaphoreTake(tcpMutex, pdMS_TO_TICKS(50)) != pdTRUE) return;
    int fd = tcpClient;
    if (fd >= 0 && send(fd, data, len, MSG_DONTWAIT) < 0 && errno != EAGAIN) {
        close(fd);
        tcpClient = -1;
    }
    xSemaphoreGive(tcpMutex);
}

// esp_log hook: mirror ESP_LOGx output to the TCP client, then print as usual.
int loggerVprintf(const char* fmt, va_list args) {
    if (tcpClient >= 0) {
        va_list copy;
        va_copy(copy, args);
        int n = vsnprintf(nullptr, 0, fmt, copy);
        va_end(copy);
        if (n > 0) {
            std::vector<char> buf(n + 1);
            va_copy(copy, args);
            vsnprintf(buf.data(), buf.size(), fmt, copy);
            va_end(copy);
            tcpWrite((const uint8_t*)buf.data(), n);
        }
    }
    return originalVprintf ? originalVprintf(fmt, args) : vprintf(fmt, args);
}

// ponytail: one client at a time, a new connection bumps the old one.
void tcpTask(void* arg) {
    uint16_t port = (uint16_t)(uintptr_t)arg;
    int server = socket(AF_INET, SOCK_STREAM, 0);
    int one = 1;
    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
    sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);
    if (bind(server, (sockaddr*)&addr, sizeof(addr)) < 0 || listen(server, 1) < 0) {
        close(server);
        vTaskDelete(nullptr);
        return;
    }
    while (true) {
        int fd = accept(server, nullptr, nullptr);
        if (fd < 0) continue;
        xSemaphoreTake(tcpMutex, portMAX_DELAY);
        if (tcpClient >= 0) close(tcpClient);
        tcpClient = fd;
        xSemaphoreGive(tcpMutex);
    }
}
}  // namespace

Logger& Logger::instance() {
    static Logger instance;
    return instance;
}

void Logger::write(const uint8_t* data, size_t len) {
    fwrite(data, 1, len, stdout);
    tcpWrite(data, len);
}

void Logger::print(const char* s) {
    if (s) write((const uint8_t*)s, strlen(s));
}

void Logger::printf(const char* fmt, ...) {
    char stackBuf[192];
    va_list args;
    va_start(args, fmt);
    va_list copy;
    va_copy(copy, args);
    int n = vsnprintf(stackBuf, sizeof(stackBuf), fmt, copy);
    va_end(copy);
    if (n < 0) {
        va_end(args);
        return;
    }
    if ((size_t)n < sizeof(stackBuf)) {
        write((const uint8_t*)stackBuf, n);
    } else {
        std::vector<char> buf(n + 1);
        vsnprintf(buf.data(), buf.size(), fmt, args);
        write((const uint8_t*)buf.data(), n);
    }
    va_end(args);
}

void Logger::enableTcp(uint16_t port) {
    if (tcpMutex) return;
    tcpMutex = xSemaphoreCreateMutex();
    originalVprintf = esp_log_set_vprintf(loggerVprintf);
    xTaskCreate(tcpTask, "logTcp", 2560, (void*)(uintptr_t)port, 1, nullptr);
}

bool Logger::isTcpConnected() const { return tcpClient >= 0; }

Logger& Log = Logger::instance();
