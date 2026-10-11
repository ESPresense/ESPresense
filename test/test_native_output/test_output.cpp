// Relay power-on state, pin polarity and MQTT payload parsing.
#include <unity.h>

#include "../../main/OutputLogic.h"

void test_power_on_off_and_on_ignore_last(void) {
    TEST_ASSERT_FALSE(outputPowerOnState(OutputPowerOn::Off, "1"));
    TEST_ASSERT_TRUE(outputPowerOnState(OutputPowerOn::On, "0"));
    TEST_ASSERT_TRUE(outputPowerOnState(OutputPowerOn::On, ""));
}

void test_power_on_restore(void) {
    TEST_ASSERT_TRUE(outputPowerOnState(OutputPowerOn::Restore, "1"));
    TEST_ASSERT_FALSE(outputPowerOnState(OutputPowerOn::Restore, "0"));
    // Never saved: a fresh node must not switch its outlet on.
    TEST_ASSERT_FALSE(outputPowerOnState(OutputPowerOn::Restore, ""));
    TEST_ASSERT_FALSE(outputPowerOnState(OutputPowerOn::Restore, "garbage"));
}

void test_unknown_mode_is_off(void) {
    TEST_ASSERT_FALSE(outputPowerOnState((OutputPowerOn)7, "1"));
}

void test_level(void) {
    TEST_ASSERT_EQUAL_INT(1, outputLevel(true, false));
    TEST_ASSERT_EQUAL_INT(0, outputLevel(false, false));
    TEST_ASSERT_EQUAL_INT(0, outputLevel(true, true));
    TEST_ASSERT_EQUAL_INT(1, outputLevel(false, true));
}

void test_payloads(void) {
    bool out = false;
    TEST_ASSERT_TRUE(outputParsePayload("ON", false, out));
    TEST_ASSERT_TRUE(out);
    TEST_ASSERT_TRUE(outputParsePayload("off", true, out));
    TEST_ASSERT_FALSE(out);
    TEST_ASSERT_TRUE(outputParsePayload(" On\n", false, out));
    TEST_ASSERT_TRUE(out);
    TEST_ASSERT_TRUE(outputParsePayload("TOGGLE", true, out));
    TEST_ASSERT_FALSE(out);
    TEST_ASSERT_TRUE(outputParsePayload("toggle", false, out));
    TEST_ASSERT_TRUE(out);
}

void test_bad_payload_leaves_output_alone(void) {
    bool out = true;
    TEST_ASSERT_FALSE(outputParsePayload("", false, out));
    TEST_ASSERT_FALSE(outputParsePayload("1", false, out));
    TEST_ASSERT_FALSE(outputParsePayload("{\"state\":\"OFF\"}", false, out));
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
