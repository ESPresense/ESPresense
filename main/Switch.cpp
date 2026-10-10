#include "Switch.h"

#include "GUI.h"
#include "InputGroup.h"

namespace Switch {
InputGroup inputs("switch", "Switch", "Switches", 2);

void Setup() { inputs.setup(); }
void ConnectToWifi(bool updating) { inputs.connectToWifi(); }
void SerialReport() { inputs.serialReport(); }

void Loop() {
    if (inputs.loop()) GUI::Switch(inputs.mask(), inputs.count());
}

bool SendDiscovery() { return inputs.sendDiscovery(); }
bool SendOnline() { return inputs.sendOnline(); }
bool Command(std::string& command, std::string& pay) { return inputs.command(command, pay); }
}  // namespace Switch
