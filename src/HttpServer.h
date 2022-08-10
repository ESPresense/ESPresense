#pragma once

#include <ESPAsyncWebServer.h>

namespace HttpServer
{

void Init(AsyncWebServer *server);

void Loop();

void UpdateStart();

void UpdateEnd();

void SendState();

}  // namespace HttpServer
