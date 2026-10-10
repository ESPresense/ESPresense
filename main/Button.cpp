#include "Button.h"

#include "GUI.h"
#include "InputGroup.h"

namespace Button {
InputGroup inputs("button", "Button", "Buttons", 2);

void Setup() { inputs.setup(); }
void ConnectToWifi(bool updating) { inputs.connectToWifi(); }
void SerialReport() { inputs.serialReport(); }

void Loop() {
    if (inputs.loop()) GUI::Button(inputs.mask(), inputs.count());
}

bool SendDiscovery() { return inputs.sendDiscovery(); }
bool SendOnline() { return inputs.sendOnline(); }
bool Command(std::string& command, std::string& pay) { return inputs.command(command, pay); }
int8_t Value(int index) { return inputs.value(index); }
}  // namespace Button
