#pragma once
#include <NimBLEAddress.h>

class QueryReport {
    private:
     std::string* _buffer;
     bool _changed;
     NimBLEAddress* _address;

   public:
    explicit QueryReport(NimBLEAddress* address);
    explicit QueryReport();

    virtual void GetTopic(std::string* input, std::string roomsTopic) = 0;
    void GetBuffer(std::string* input);
    void UpdateBuffer(std::string* buffer);
   auto HasChanged() const -> bool { return _changed; }
    auto GetAddress() -> NimBLEAddress* { return _address; }
};
