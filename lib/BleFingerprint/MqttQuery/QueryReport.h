#pragma once
#include <NimBLEAddress.h>
#include<HardwareSerial.h>
class QueryReport
{
private:
    std::string* buffer;
    bool changed;
    NimBLEAddress* address;
public:
    QueryReport(NimBLEAddress* address);
    QueryReport();
    virtual void GetTopic(std::string* input, std::string roomsTopic) = 0;
    void GetBuffer(std::string* input);
    void UpdateBuffer( std::string* newbuffer);
    bool HasChanged();
    NimBLEAddress* getAddress(){return address;};
    std::string* topic;
};