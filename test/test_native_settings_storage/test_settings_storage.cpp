#include <atomic>
#include <cstdlib>
#include <string>
#include <thread>
#include <unistd.h>
#include "unity.h"
#include "SettingsStorage.h"


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

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_missing_and_read_error_are_distinct);
    RUN_TEST(test_readers_never_observe_a_partial_write);
    return UNITY_END();
}
