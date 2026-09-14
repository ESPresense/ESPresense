#pragma once
#include <string>

#include "esp_http_server.h"

namespace HttpWebServer {
// Start the server: web UI, /json*, /wifi*, /ws, /restart. captive=true adds the portal
// redirect for foreign Host headers.
void Init(bool captive = false);
void Loop();
void UpdateStart();
void UpdateEnd();
void SendState();  // push enrollment state to websocket clients (thread-safe)
}  // namespace HttpWebServer
