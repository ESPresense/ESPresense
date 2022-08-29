#pragma once

#include <ESPAsyncWebServer.h>

namespace HttpWebServer
{

void Init(AsyncWebServer *server);
void Loop();
void UpdateStart();
void UpdateEnd();
void SendState();

}  // namespace HttpServer
