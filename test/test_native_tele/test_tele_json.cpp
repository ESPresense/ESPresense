// /json/tele body builder: escaping and the refuse-rather-than-truncate rule.
//
// The room name is user-set free text, and the body is parsed by machines (Home Assistant,
// the companion, the HIL monitor). A quote in the room name used to produce a malformed
// body served as 200, and an over-long one used to produce a truncated body served as 200.
#include <cstring>
#include <string>

#include <unity.h>

#include "../../src/TeleJson.h"

namespace {

std::string build(const char* room, size_t size = 256) {
    std::string out(size, '\0');
    size_t const len = buildTeleJson(&out[0], size, room, 84710, 28236, 29);
    if (len == 0) return "";
    out.resize(len);
    return out;
}

void expect_fields(const std::string& body) {
    TEST_ASSERT_TRUE(body.find("\"freeHeap\":84710") != std::string::npos);
    TEST_ASSERT_TRUE(body.find("\"maxHeap\":28236") != std::string::npos);
    TEST_ASSERT_TRUE(body.find("\"fingerprints\":29") != std::string::npos);
}

void test_plain_room(void) {
    std::string const body = build("Dining");
    TEST_ASSERT_EQUAL_STRING(
        "{\"room\":\"Dining\",\"freeHeap\":84710,\"maxHeap\":28236,\"fingerprints\":29}",
        body.c_str());
}

void test_quote_is_escaped(void) {
    std::string const body = build("Da\"ta");
    TEST_ASSERT_TRUE(body.find("\"room\":\"Da\\\"ta\"") != std::string::npos);
    expect_fields(body);
}

void test_backslash_is_escaped(void) {
    std::string const body = build("back\\slash");
    TEST_ASSERT_TRUE(body.find("\"room\":\"back\\\\slash\"") != std::string::npos);
    expect_fields(body);
}

void test_control_characters_are_escaped(void) {
    std::string const body = build("a\nb\tc");
    // Raw control bytes in a JSON string are invalid; they must come out escaped.
    TEST_ASSERT_TRUE(body.find('\n') == std::string::npos);
    TEST_ASSERT_TRUE(body.find('\t') == std::string::npos);
    TEST_ASSERT_TRUE(body.find("\\n") != std::string::npos);
    TEST_ASSERT_TRUE(body.find("\\t") != std::string::npos);
    expect_fields(body);
}

void test_injection_attempt_stays_inside_the_string(void) {
    // A room that tries to close the string and add its own key must not manage it: the
    // body has to keep exactly the four keys.
    std::string const body = build("x\",\"freeHeap\":\"0");
    TEST_ASSERT_TRUE(body.find("\\\"") != std::string::npos);
    TEST_ASSERT_TRUE(body.find("\"freeHeap\":84710") != std::string::npos);
    size_t count = 0;
    for (size_t at = body.find("\"freeHeap\""); at != std::string::npos;
         at = body.find("\"freeHeap\"", at + 1)) {
        count++;
    }
    TEST_ASSERT_EQUAL_UINT(1, count);  // the injected one is inside the room string
}

void test_oversized_room_is_refused_not_truncated(void) {
    std::string const huge(4096, 'x');
    std::string out(256, '\0');
    TEST_ASSERT_EQUAL_UINT(0, buildTeleJson(&out[0], out.size(), huge.c_str(), 1, 2, 3));
}

void test_escaping_growth_is_accounted_for(void) {
    // Escaping can double the length, so a room that fits raw may not fit encoded. The
    // check has to measure the *encoded* size, which is the bug this guards.
    std::string const quotes(200, '"');
    std::string out(256, '\0');
    TEST_ASSERT_EQUAL_UINT(0, buildTeleJson(&out[0], out.size(), quotes.c_str(), 1, 2, 3));
}

void test_exact_fit_boundary(void) {
    std::string const body = build("Dining");
    size_t const needed = body.size();
    std::string out(needed + 1, '\0');
    // One byte short must refuse rather than write a truncated object.
    TEST_ASSERT_EQUAL_UINT(0, buildTeleJson(&out[0], needed, "Dining", 84710, 28236, 29));
    // Exactly enough (plus the NUL) must succeed.
    TEST_ASSERT_EQUAL_UINT(needed,
                           buildTeleJson(&out[0], needed + 1, "Dining", 84710, 28236, 29));
}

void test_degenerate_buffers(void) {
    char buf[8];
    TEST_ASSERT_EQUAL_UINT(0, buildTeleJson(nullptr, sizeof(buf), "Dining", 1, 2, 3));
    TEST_ASSERT_EQUAL_UINT(0, buildTeleJson(buf, 0, "Dining", 1, 2, 3));
}

}  // namespace

int main(int, char**) {
    UNITY_BEGIN();
    RUN_TEST(test_plain_room);
    RUN_TEST(test_quote_is_escaped);
    RUN_TEST(test_backslash_is_escaped);
    RUN_TEST(test_control_characters_are_escaped);
    RUN_TEST(test_injection_attempt_stays_inside_the_string);
    RUN_TEST(test_oversized_room_is_refused_not_truncated);
    RUN_TEST(test_escaping_growth_is_accounted_for);
    RUN_TEST(test_exact_fit_boundary);
    RUN_TEST(test_degenerate_buffers);
    return UNITY_END();
}
