#pragma once

class Stream;

namespace SerialImprov {
void Setup();
void Loop(bool wifiConnected);
void SetSerial(Stream* stream);
}
