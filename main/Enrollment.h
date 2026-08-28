#pragma once
#include <string>

namespace Enrollment {
bool Loop();
void Setup();
bool Command(std::string& command, std::string& pay);
bool SendDiscovery();
}  // namespace Enrollment
