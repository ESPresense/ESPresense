#include <MqttQuery/QueryReport.h>

QueryReport::QueryReport(NimBLEAddress* bleaddress) {
    changed = false;
    address = bleaddress;
    Serial.println(bleaddress->toString().c_str());
}
QueryReport::QueryReport() {
    changed = false;
};

void QueryReport::GetBuffer(std::string* input) {
    Serial.println(buffer->c_str());
    *input = *buffer;
    changed = false;
}
bool QueryReport::HasChanged() {
    return changed;
}
void QueryReport::UpdateBuffer(std::string* newbuffer) {
    if (buffer != newbuffer) {
        buffer = newbuffer;
        changed = true;
    }
}
