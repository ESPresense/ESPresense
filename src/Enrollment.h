#pragma once

#include "Arduino.h"

namespace Enrollment
{
    bool Loop();
    void Setup();
    bool Command(String& command, String& pay);
    bool SendDiscovery();
}
