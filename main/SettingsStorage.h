#pragma once

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <mutex>
#include <string>

// Keep on-flash values alive while ArduinoJson serializes linked strings. Allocate with malloc
// so an exhausted heap is an ordinary failure even when C++ exceptions are disabled.
class SettingsSnapshot {
    struct Node { Node* next; char text[1]; };
    Node* head_ = nullptr;
public:
    SettingsSnapshot() = default;
    SettingsSnapshot(const SettingsSnapshot&) = delete;
    SettingsSnapshot& operator=(const SettingsSnapshot&) = delete;
    ~SettingsSnapshot() { while (head_) { Node* next = head_->next; free(head_); head_ = next; } }
    char* allocate(size_t length) {
        Node* n = static_cast<Node*>(malloc(sizeof(Node) + length));
        if (!n) return nullptr;
        n->next = head_;
        head_ = n;
        return n->text;
    }
};

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

    ReadResult readSnapshot(const std::string& path, SettingsSnapshot& snapshot, const char*& value, size_t& length) {
        std::lock_guard<std::mutex> guard(mutex_);
        value = nullptr;
        length = 0;
        FILE* f = fopen(path.c_str(), "rb");
        if (!f) return errno == ENOENT ? ReadResult::Missing : ReadResult::Error;
        if (fseek(f, 0, SEEK_END) != 0) { fclose(f); return ReadResult::Error; }
        long size = ftell(f);
        // A larger value cannot fit in a restorable template.
        if (size < 0 || size > 32768 || fseek(f, 0, SEEK_SET) != 0) { fclose(f); return ReadResult::Error; }
        char* buffer = snapshot.allocate(static_cast<size_t>(size));
        if (!buffer) { fclose(f); return ReadResult::Error; }
        bool ok = fread(buffer, 1, static_cast<size_t>(size), f) == static_cast<size_t>(size) && !ferror(f);
        if (fclose(f) != 0) ok = false;
        if (!ok) return ReadResult::Error;
        buffer[size] = '\0';
        value = buffer;
        length = static_cast<size_t>(size);
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
