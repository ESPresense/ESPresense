#pragma once
#include <NimBLEAddress.h>

class QueryReport {
public:
    QueryReport(const String& topic, const String& payload) : topic(topic), payload(payload) {}

    String getTopic() const { return topic; }
    String getPayload() const { return payload; }

    void setTopic(const String& topic) { this->topic = topic; }
    void setPayload(const String& payload) { this->payload = payload; }

private:
    String topic;
    String payload;
};
