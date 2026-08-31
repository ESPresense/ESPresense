#pragma once
#include <cstdarg>
#include <cstdint>
#include <string>

// Serial console (stdout) plus an optional single-client TCP mirror on port 6053.
class Logger {
   public:
    static Logger& instance();

    void printf(const char* fmt, ...) __attribute__((format(printf, 2, 3)));  /* Flawfinder: ignore */
    void print(const char* s);
    void print(const std::string& s) { print(s.c_str()); }
    void print(int v) { printf("%d", v); }
    void print(unsigned v) { printf("%u", v); }
    void print(long v) { printf("%ld", v); }
    void print(unsigned long v) { printf("%lu", v); }
    void print(float v) { printf("%.2f", (double)v); }
    void print(double v) { printf("%.2f", v); }
    void println() { print("\r\n"); }
    template <typename T>
    void println(const T& v) { print(v); println(); }

    void write(const uint8_t* data, size_t len);

    void enableTcp(uint16_t port = 6053);
    bool isTcpConnected() const;

   private:
    Logger() = default;
};

extern Logger& Log;
