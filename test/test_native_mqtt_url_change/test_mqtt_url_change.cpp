// Moving a node to another broker over MQTT, on trial with rollback (MqttUrlChange.h, #2647).
#include <map>
#include <string>

#include "unity.h"
#include "MqttUrlChange.h"

using namespace MqttUrlChange;
typedef std::map<std::string, std::string> Store;

// A put of "" deletes, as Settings::spurt does. failAfter = writes that succeed before one fails.
struct Disk {
    Store s;
    int failAfter = -1;
    std::string at(const char* k) const { auto i = s.find(k); return i == s.end() ? "" : i->second; }
    auto get() { return [this](const std::string& k) { auto i = s.find(k); return i == s.end() ? std::string() : i->second; }; }
    auto put() {
        return [this](const std::string& k, const std::string& v) {
            if (failAfter == 0) return false;
            if (failAfter > 0) failAfter--;
            if (v.empty()) s.erase(k); else s[k] = v;
            return true;
        };
    }
};

static const std::string own = "espresense/rooms/office/mqtt-url/set";

static void only_this_nodes_own_live_topic() {
    TEST_ASSERT_EQUAL_STRING("", refuse(own, own, false, "mqtts://broker", "mqtt://broker").c_str());
    TEST_ASSERT_NOT_EQUAL(0, refuse("espresense/rooms/*/mqtt-url/set", own, false, "mqtts://broker", "mqtt://broker").size());
    TEST_ASSERT_NOT_EQUAL(0, refuse(own, own, true, "mqtts://broker", "mqtt://broker").size());
}

static void refuses_what_cant_be_a_broker() {
    TEST_ASSERT_NOT_EQUAL(0, refuse(own, own, false, "", "mqtt://broker").size());
    TEST_ASSERT_NOT_EQUAL(0, refuse(own, own, false, "broker.lan", "mqtt://broker").size());  // a URL, so the scheme is said
    TEST_ASSERT_NOT_EQUAL(0, refuse(own, own, false, "wss://broker", "mqtt://broker").size());
    TEST_ASSERT_NOT_EQUAL(0, refuse(own, own, false, "mqtts://", "mqtt://broker").size());
    TEST_ASSERT_NOT_EQUAL(0, refuse(own, own, false, "mqtt://broker", "mqtt://broker").size());  // already there
    TEST_ASSERT_EQUAL_STRING("", refuse(own, own, false, "MQTTS://broker:9883", "mqtt://broker").c_str());
}

static void a_change_that_connects_is_kept() {
    Disk d{{{"mqtt_host", "broker.lan"}}};
    TEST_ASSERT_TRUE(apply(d.put(), "mqtt://broker.lan", "mqtts://broker.lan"));
    TEST_ASSERT_EQUAL_STRING("mqtts://broker.lan", d.at(kHost).c_str());
    TEST_ASSERT_TRUE(atBoot(d.get()) == Boot::Trial);
    Clock clock;
    TEST_ASSERT_TRUE(step(true, true, 5000, clock) == Step::Commit);
    TEST_ASSERT_TRUE(clear(d.put()));
    TEST_ASSERT_EQUAL(1, d.s.size());
    TEST_ASSERT_TRUE(atBoot(d.get()) == Boot::None);
}

static void a_change_that_doesnt_connect_rolls_back() {
    Disk d{{{"mqtt_host", "broker.lan"}}};
    apply(d.put(), "mqtt://broker.lan", "mqtts://broker.lan");
    TEST_ASSERT_TRUE(rollback(d.get(), d.put()));
    TEST_ASSERT_EQUAL_STRING("mqtt://broker.lan", d.at(kHost).c_str());
    TEST_ASSERT_EQUAL_STRING("mqtts://broker.lan", d.at(kFailed).c_str());
    TEST_ASSERT_EQUAL(2, d.s.size());
    TEST_ASSERT_TRUE(atBoot(d.get()) == Boot::None);
}

static void the_clock_only_runs_while_the_network_is_up() {
    Clock c;
    TEST_ASSERT_TRUE(step(false, false, 1000, c) == Step::Wait);
    TEST_ASSERT_FALSE(c.running);
    TEST_ASSERT_TRUE(step(false, true, 2000, c) == Step::Wait);
    TEST_ASSERT_TRUE(step(false, true, 2000 + trialMs - 1, c) == Step::Wait);
    TEST_ASSERT_TRUE(step(false, false, 2000 + trialMs, c) == Step::Wait);  // network dropped: restart the clock
    TEST_ASSERT_TRUE(step(false, true, 100000, c) == Step::Wait);
    TEST_ASSERT_TRUE(step(false, true, 100000 + trialMs - 1, c) == Step::Wait);
    TEST_ASSERT_TRUE(step(false, true, 100000 + trialMs, c) == Step::Rollback);
    Clock z;
    TEST_ASSERT_TRUE(step(false, true, 0, z) == Step::Wait);  // millis() 0 still starts it
    TEST_ASSERT_TRUE(step(false, true, trialMs, z) == Step::Rollback);
}

static void a_power_cut_never_leaves_the_wrong_broker() {
    // Apply cut after kPrev and kTrial, before mqtt_host: the old broker, and the trial is stale.
    Disk a{{{"mqtt_host", "broker.lan"}}};
    a.failAfter = 2;
    TEST_ASSERT_FALSE(apply(a.put(), "mqtt://broker.lan", "mqtts://broker.lan"));
    TEST_ASSERT_EQUAL_STRING("broker.lan", a.at(kHost).c_str());
    TEST_ASSERT_TRUE(atBoot(a.get()) == Boot::Stale);

    // Rollback cut after mqtt_host: back on the old broker, and the trial is stale.
    Disk r{{{"mqtt_host", "broker.lan"}}};
    apply(r.put(), "mqtt://broker.lan", "mqtts://broker.lan");
    r.failAfter = 1;
    TEST_ASSERT_FALSE(rollback(r.get(), r.put()));
    TEST_ASSERT_EQUAL_STRING("mqtt://broker.lan", r.at(kHost).c_str());
    TEST_ASSERT_TRUE(atBoot(r.get()) == Boot::Stale);

    // Commit cut after the trial key: kPrev alone is stale, not a trial.
    Disk c{{{"mqtt_host", "broker.lan"}}};
    apply(c.put(), "mqtt://broker.lan", "mqtts://broker.lan");
    c.failAfter = 1;
    TEST_ASSERT_FALSE(clear(c.put()));
    TEST_ASSERT_TRUE(atBoot(c.get()) == Boot::Stale);
    c.failAfter = -1;  // next boot clears it
    TEST_ASSERT_TRUE(clear(c.put()));
    TEST_ASSERT_TRUE(atBoot(c.get()) == Boot::None);
}

static void a_fix_in_the_web_ui_ends_the_trial() {
    Disk d{{{"mqtt_host", "broker.lan"}}};
    apply(d.put(), "mqtt://broker.lan", "mqtts://broker.lan");
    d.s[kHost] = "mqtts://other.lan";  // saved from the web UI while on trial
    TEST_ASSERT_TRUE(atBoot(d.get()) == Boot::Stale);
}

static void reports_leave_out_credentials() {
    TEST_ASSERT_EQUAL_STRING("mqtts://broker:8883", redact("mqtts://user:pa@ss@broker:8883").c_str());
    TEST_ASSERT_EQUAL_STRING("mqtt://broker", redact("mqtt://broker").c_str());
    TEST_ASSERT_EQUAL_STRING("mqtt://broker/p@th", redact("mqtt://broker/p@th").c_str());
}

int main(int, char**) {
    UNITY_BEGIN();
    RUN_TEST(only_this_nodes_own_live_topic);
    RUN_TEST(refuses_what_cant_be_a_broker);
    RUN_TEST(a_change_that_connects_is_kept);
    RUN_TEST(a_change_that_doesnt_connect_rolls_back);
    RUN_TEST(the_clock_only_runs_while_the_network_is_up);
    RUN_TEST(a_power_cut_never_leaves_the_wrong_broker);
    RUN_TEST(a_fix_in_the_web_ui_ends_the_trial);
    RUN_TEST(reports_leave_out_credentials);
    return UNITY_END();
}
