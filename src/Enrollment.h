#pragma once

#include "string_utils.h"

namespace Enrollment
{
    bool Busy();
    bool Loop();
    void Setup();
    bool Command(String& command, String& pay);
    bool SendDiscovery();
}
