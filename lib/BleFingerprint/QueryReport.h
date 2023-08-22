#pragma once
#include <NimBLEAddress.h>

class QueryReport {
public:
    QueryReport(const String& id, const String& payload) : id(id), payload(payload) {}

    String getId() const { return id; }
    String getPayload() const { return payload; }

    void setId(const String& id) { this->id = id; }
    void setPayload(const String& payload) { this->payload = payload; }

private:
    String id;
    String payload;
};
