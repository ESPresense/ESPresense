// Relay power-on state, pin polarity and MQTT payload parsing.
#include <unity.h>

#include "../../main/RelayLogic.h"

void test_power_on_off_and_on_ignore_last(void) {
    TEST_ASSERT_FALSE(relayPowerOnState(RelayPowerOn::Off, "1"));
    TEST_ASSERT_TRUE(relayPowerOnState(RelayPowerOn::On, "0"));
    TEST_ASSERT_TRUE(relayPowerOnState(RelayPowerOn::On, ""));
}

void test_power_on_restore(void) {
    TEST_ASSERT_TRUE(relayPowerOnState(RelayPowerOn::Restore, "1"));
    TEST_ASSERT_FALSE(relayPowerOnState(RelayPowerOn::Restore, "0"));
    // Never saved: a fresh node must not switch its outlet on.
    TEST_ASSERT_FALSE(relayPowerOnState(RelayPowerOn::Restore, ""));
    TEST_ASSERT_FALSE(relayPowerOnState(RelayPowerOn::Restore, "garbage"));
}

void test_unknown_mode_is_off(void) {
    TEST_ASSERT_FALSE(relayPowerOnState((RelayPowerOn)7, "1"));
}

void test_level(void) {
    TEST_ASSERT_EQUAL_INT(1, relayLevel(true, false));
    TEST_ASSERT_EQUAL_INT(0, relayLevel(false, false));
    TEST_ASSERT_EQUAL_INT(0, relayLevel(true, true));
    TEST_ASSERT_EQUAL_INT(1, relayLevel(false, true));
}

void test_payloads(void) {
    bool out = false;
    TEST_ASSERT_TRUE(relayParsePayload("ON", false, out));
    TEST_ASSERT_TRUE(out);
    TEST_ASSERT_TRUE(relayParsePayload("off", true, out));
    TEST_ASSERT_FALSE(out);
    TEST_ASSERT_TRUE(relayParsePayload(" On\n", false, out));
    TEST_ASSERT_TRUE(out);
    TEST_ASSERT_TRUE(relayParsePayload("TOGGLE", true, out));
    TEST_ASSERT_FALSE(out);
    TEST_ASSERT_TRUE(relayParsePayload("toggle", false, out));
    TEST_ASSERT_TRUE(out);
}

void test_bad_payload_leaves_output_alone(void) {
    bool out = true;
    TEST_ASSERT_FALSE(relayParsePayload("", false, out));
    TEST_ASSERT_FALSE(relayParsePayload("1", false, out));
    TEST_ASSERT_FALSE(relayParsePayload("{\"state\":\"OFF\"}", false, out));
    TEST_ASSERT_TRUE(out);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_power_on_off_and_on_ignore_last);
    RUN_TEST(test_power_on_restore);
    RUN_TEST(test_unknown_mode_is_off);
    RUN_TEST(test_level);
    RUN_TEST(test_payloads);
    RUN_TEST(test_bad_payload_leaves_output_alone);
    return UNITY_END();
}
