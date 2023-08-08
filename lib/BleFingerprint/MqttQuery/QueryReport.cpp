#include <MqttQuery/QueryReport.h>

QueryReport::QueryReport(NimBLEAddress* bleaddress) {
    changed = false;
    address = bleaddress;
}
QueryReport::QueryReport() {
    changed = false;
};

void QueryReport::GetBuffer(std::string* input) {
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
