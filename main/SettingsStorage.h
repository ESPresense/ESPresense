#pragma once

#include <cerrno>
#include <cstdio>
#include <mutex>
#include <string>

// Serialize all firmware file access so readers never observe a truncated write.
// Missing files represent defaults; all other I/O errors must remain distinguishable.
class SettingsStorage {
public:
    enum class ReadResult { Ok, Missing, Error };

    ReadResult read(const std::string& path, std::string& value) {
        std::lock_guard<std::mutex> guard(mutex_);
        value.clear();
        FILE* f = fopen(path.c_str(), "rb");
        if (!f) return errno == ENOENT ? ReadResult::Missing : ReadResult::Error;
        char buf[128];
        size_t n;
        while ((n = fread(buf, 1, sizeof(buf), f)) != 0) value.append(buf, n);
        bool ok = !ferror(f);
        if (fclose(f) != 0) ok = false;
        if (!ok) { value.clear(); return ReadResult::Error; }
        return ReadResult::Ok;
    }

    bool write(const std::string& path, const std::string& value) {
        std::lock_guard<std::mutex> guard(mutex_);
        if (value.empty()) return std::remove(path.c_str()) == 0 || errno == ENOENT;
        FILE* f = fopen(path.c_str(), "wb");
        if (!f) return false;
        bool ok = fwrite(value.data(), 1, value.size(), f) == value.size();
        if (fclose(f) != 0) ok = false;
        return ok;
    }

    bool remove(const std::string& path) {
        std::lock_guard<std::mutex> guard(mutex_);
        return std::remove(path.c_str()) == 0;
    }

private:
    std::mutex mutex_;
};
