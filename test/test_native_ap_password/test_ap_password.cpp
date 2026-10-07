// /wifi/main AP password rule.
//
// Enabling "protect the configuration AP" with no usable password would have stored
// ap-password-enabled=1 next to an empty /ap-password, so portal() would fall through to
// WIFI_AUTH_OPEN and leave the setup portal open while the UI said it was protected. The
// masked placeholder the UI echoes back must count as "unchanged", not as a literal
// 9-character password, and must not be length-checked.
#include <string>

#include <unity.h>

#include "../../main/ApPassword.h"

namespace {

using Settings::apPasswordAcceptable;
using Settings::maskedPassword;

// 8 and 63 are the WPA2-PSK bounds; esp_wifi_set_config() rejects anything outside them.
void test_bounds(void) {
    TEST_ASSERT_FALSE(apPasswordAcceptable(false, "short", false));
    TEST_ASSERT_FALSE(apPasswordAcceptable(false, "1234567", false));
    TEST_ASSERT_TRUE(apPasswordAcceptable(false, "12345678", false));
    TEST_ASSERT_TRUE(apPasswordAcceptable(false, std::string(63, 'x'), false));
    TEST_ASSERT_FALSE(apPasswordAcceptable(false, std::string(64, 'x'), false));
}

// Empty is fine while protection is off: nothing is stored, so there is nothing to validate.
void test_empty_allowed_when_disabled(void) {
    TEST_ASSERT_TRUE(apPasswordAcceptable(false, "", false));
}

// The case the original code got wrong: enabling with no password anywhere is not a valid
// state and must be refused rather than silently producing an open AP.
void test_enabling_without_password_is_refused(void) {
    TEST_ASSERT_FALSE(apPasswordAcceptable(true, "", false));
}

// ...but enabling alongside a password in the same request is the normal path.
void test_enabling_with_password_in_request(void) {
    TEST_ASSERT_TRUE(apPasswordAcceptable(true, "AccessPoint8", false));
}

void test_enabling_with_bad_length_is_refused(void) {
    TEST_ASSERT_FALSE(apPasswordAcceptable(true, "short", false));
    TEST_ASSERT_FALSE(apPasswordAcceptable(true, std::string(64, 'x'), false));
}

// A masked resubmit means "keep what is saved". It is never stored, so it is not a 9-char
// password and must not be rejected for length.
void test_masked_is_unchanged_not_a_password(void) {
    TEST_ASSERT_TRUE(apPasswordAcceptable(false, maskedPassword, true));
    TEST_ASSERT_TRUE(apPasswordAcceptable(true, maskedPassword, true));
}

// Masked with nothing saved: the UI echoed a placeholder for a password that does not exist.
// Enabling must still be refused, since there is no usable password behind it.
void test_masked_with_nothing_stored_cannot_enable(void) {
    TEST_ASSERT_FALSE(apPasswordAcceptable(true, maskedPassword, false));
    // Disabled is fine: it just clears a setting that was never set.
    TEST_ASSERT_TRUE(apPasswordAcceptable(false, maskedPassword, false));
}

// Disabling with the stored password masked keeps it; Param::set drops the mask.
void test_disable_keeps_masked_password(void) {
    TEST_ASSERT_TRUE(apPasswordAcceptable(false, maskedPassword, true));
}

// portal() calls this with the value it just read from /ap-password. An empty file must not
// pass as "there is a stored password", or WPA2 gets a zero-length key. Regression guard:
// the third argument must be derived from the read value, not hardcoded true.
void test_empty_stored_password_exposes_no_usable_key(void) {
    TEST_ASSERT_FALSE(apPasswordAcceptable(true, "", false));
}

}  // namespace

int main(int, char**) {
    UNITY_BEGIN();
    RUN_TEST(test_bounds);
    RUN_TEST(test_empty_allowed_when_disabled);
    RUN_TEST(test_enabling_without_password_is_refused);
    RUN_TEST(test_enabling_with_password_in_request);
    RUN_TEST(test_enabling_with_bad_length_is_refused);
    RUN_TEST(test_masked_is_unchanged_not_a_password);
    RUN_TEST(test_masked_with_nothing_stored_cannot_enable);
    RUN_TEST(test_disable_keeps_masked_password);
    RUN_TEST(test_empty_stored_password_exposes_no_usable_key);
    return UNITY_END();
}
