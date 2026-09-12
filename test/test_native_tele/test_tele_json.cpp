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

std::string build(const char* room, size_t size = 512) {
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
        "{\"room\":\"Dining\",\"freeHeap\":84710,\"maxHeap\":28236,\"fingerprints\":29,"
        "\"uptime\":0,\"minHeap\":0,\"fpNew\":0,\"fpDel\":0,\"telePubs\":0,"
        "\"mqttRetries\":0,\"allocFails\":0}",
        body.c_str());
}

// Every counter is emitted even at zero (see buildTeleJson): the endpoint is polled into a
// CSV, and a column that disappears on a zero would silently shift the others.
void test_counters_are_reported(void) {
    TeleCounters counters;
    counters.uptime = 24635;
    counters.minHeap = 19112;
    counters.fpNew = 1463;
    counters.fpDel = 1379;
    counters.telePubs = 1560;
    counters.mqttRetries = 3500;
    counters.allocFails = 3395;

    std::string out(512, '\0');
    size_t const len =
        buildTeleJson(&out[0], out.size(), "Dining", 84710, 28236, 29, counters);
    TEST_ASSERT_TRUE(len > 0);
    out.resize(len);

    TEST_ASSERT_TRUE(out.find("\"uptime\":24635") != std::string::npos);
    TEST_ASSERT_TRUE(out.find("\"minHeap\":19112") != std::string::npos);
    TEST_ASSERT_TRUE(out.find("\"fpNew\":1463") != std::string::npos);
    TEST_ASSERT_TRUE(out.find("\"fpDel\":1379") != std::string::npos);
    TEST_ASSERT_TRUE(out.find("\"telePubs\":1560") != std::string::npos);
    TEST_ASSERT_TRUE(out.find("\"mqttRetries\":3500") != std::string::npos);
    TEST_ASSERT_TRUE(out.find("\"allocFails\":3395") != std::string::npos);
    expect_fields(out);
}

// Ten-digit counters are what the document has to survive at the top of the uint32 range;
// the buffer sizing in serveTele() is chosen for this case, not for the zeros above.
void test_saturated_counters_still_fit(void) {
    TeleCounters counters;
    counters.uptime = counters.minHeap = counters.fpNew = counters.fpDel =
        counters.telePubs = counters.mqttRetries = counters.allocFails = 4294967295u;

    std::string out(512, '\0');
    size_t const len = buildTeleJson(&out[0], out.size(), "Dining", 4294967295u,
                                     4294967295u, 4294967295u, counters);
    TEST_ASSERT_TRUE(len > 0);
    out.resize(len);
    TEST_ASSERT_TRUE(out.find("\"allocFails\":4294967295") != std::string::npos);
}

// A room that fit comfortably under the four-field document no longer fits under the
// eleven-field one. It must still be refused rather than truncated — the widening must not
// have opened a hole in the rule the rest of this file exists to enforce.
void test_counters_do_not_break_the_refusal(void) {
    std::string const room(200, 'x');
    std::string out(256, '\0');
    TeleCounters counters;
    counters.uptime = 24635;
    TEST_ASSERT_EQUAL_UINT(
        0, buildTeleJson(&out[0], out.size(), room.c_str(), 84710, 28236, 29, counters));
    // Nothing was written: the buffer is untouched, not half-filled.
    TEST_ASSERT_TRUE(out.find_first_not_of('\0') == std::string::npos);
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
    RUN_TEST(test_counters_are_reported);
    RUN_TEST(test_saturated_counters_still_fit);
    RUN_TEST(test_counters_do_not_break_the_refusal);
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
