#include <atomic>
#include <cstdlib>
#include <string>
#include <thread>
#include <unistd.h>
#include "unity.h"
#include "SettingsStorage.h"
#include "ArduinoJson.h"


void test_missing_and_read_error_are_distinct() {
    SettingsStorage storage;
    char dir[] = "/tmp/settings-storage-XXXXXX";
    TEST_ASSERT_NOT_NULL(mkdtemp(dir));
    std::string value = "old";
    TEST_ASSERT_EQUAL((int)SettingsStorage::ReadResult::Missing, (int)storage.read(std::string(dir) + "/absent", value));
    TEST_ASSERT_TRUE(value.empty());
    // On the host a directory opens, but fread fails with EISDIR.
    TEST_ASSERT_EQUAL((int)SettingsStorage::ReadResult::Error, (int)storage.read(dir, value));
    TEST_ASSERT_TRUE(value.empty());
    TEST_ASSERT_FALSE(storage.write("/dev/full", "write failure"));
    rmdir(dir);
}

void test_readers_never_observe_a_partial_write() {
    SettingsStorage storage;
    char dir[] = "/tmp/settings-storage-XXXXXX";
    TEST_ASSERT_NOT_NULL(mkdtemp(dir));
    const std::string path = std::string(dir) + "/setting";
    const std::string a(8192, 'a'), b(8192, 'b');
    TEST_ASSERT_TRUE(storage.write(path, a));
    std::atomic<bool> failed{false};
    std::thread writer([&] {
        for (int i = 0; i < 500; ++i)
            if (!storage.write(path, i % 2 ? a : b)) failed = true;
    });
    for (int i = 0; i < 500; ++i) {
        std::string actual;
        if (storage.read(path, actual) != SettingsStorage::ReadResult::Ok || (actual != a && actual != b)) failed = true;
    }
    writer.join();
    TEST_ASSERT_FALSE(failed.load());
    TEST_ASSERT_TRUE(storage.write(path, ""));
    std::string value;
    TEST_ASSERT_EQUAL((int)SettingsStorage::ReadResult::Missing, (int)storage.read(path, value));
    TEST_ASSERT_TRUE(storage.write(path, ""));
    rmdir(dir);
}

void test_snapshot_keeps_large_json_and_binary_strings_until_export() {
    SettingsStorage storage;
    char dir[] = "/tmp/settings-storage-XXXXXX";
    TEST_ASSERT_NOT_NULL(mkdtemp(dir));
    const std::string path = std::string(dir) + "/setting";
    const std::string raw = "[{\"name\":\"" + std::string(7000, 'x') + "\"}]";
    TEST_ASSERT_TRUE(storage.write(path, raw));
    SettingsSnapshot snapshot;
    const char* data = nullptr;
    size_t length = 0;
    TEST_ASSERT_EQUAL((int)SettingsStorage::ReadResult::Ok, (int)storage.readSnapshot(path, snapshot, data, length));
    TEST_ASSERT_EQUAL_UINT(raw.size(), length);
    DynamicJsonDocument doc(256);
    doc["outputs"].set(serialized(data, length));
    // A later overwrite must not change the already captured backup.
    TEST_ASSERT_TRUE(storage.write(path, "[]"));
    std::string exported;
    serializeJson(doc, exported);
    TEST_ASSERT_TRUE(exported == "{\"outputs\":" + raw + "}");
    std::string binary("a\0b", 3);
    TEST_ASSERT_TRUE(storage.write(path, binary));
    TEST_ASSERT_EQUAL((int)SettingsStorage::ReadResult::Ok, (int)storage.readSnapshot(path, snapshot, data, length));
    doc.clear();
    doc["text"].set(JsonString(data, length, JsonString::Linked));
    exported.clear();
    serializeJson(doc, exported);
    TEST_ASSERT_EQUAL_STRING("{\"text\":\"a\\u0000b\"}", exported.c_str());
    TEST_ASSERT_TRUE(storage.write(path, ""));
    TEST_ASSERT_EQUAL((int)SettingsStorage::ReadResult::Missing, (int)storage.readSnapshot(path, snapshot, data, length));
    TEST_ASSERT_NULL(data);
    TEST_ASSERT_EQUAL_UINT(0, length);
    rmdir(dir);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_missing_and_read_error_are_distinct);
    RUN_TEST(test_readers_never_observe_a_partial_write);
    RUN_TEST(test_snapshot_keeps_large_json_and_binary_strings_until_export);
    return UNITY_END();
}
