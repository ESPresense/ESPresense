#include "QueryReport.h"

QueryReport::QueryReport(NimBLEAddress* bleaddress) {
    _changed = false;
    _address = bleaddress;
}

QueryReport::QueryReport() {
    _changed = false;
};

void QueryReport::GetBuffer(std::string* input) {
    *input = *_buffer;
    _changed = false;
}

void QueryReport::UpdateBuffer(std::string* buffer) {
    if (_buffer != buffer) {
        _buffer = buffer;
        _changed = true;
    }
}
