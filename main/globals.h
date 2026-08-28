#pragma once
#include <ArduinoJson.h>

#include <string>

#include "Logger.h"
#include "util.h"

/*----------------------------------------------------------------------------
globals.h — #define VAR_DECLS before including to define; otherwise extern.
----------------------------------------------------------------------------*/
#ifndef VAR_DECLS
#define _DECL extern
#define _INIT(x)
#define _INIT_N(x)
#else
#define _DECL
#define _INIT(x) = x
#define UNPACK(...) __VA_ARGS__
#define _INIT_N(x) UNPACK x
#endif

_DECL std::string room, id, statusTopic, teleTopic, roomsTopic, setTopic, configTopic;
_DECL std::string homeAssistantDiscoveryPrefix;
_DECL DynamicJsonDocument doc _INIT_N(((768)));
_DECL std::string localIp;
_DECL bool enrolling;
_DECL std::string enrolledId;
_DECL unsigned long enrollingEndMillis;

// I2C
_DECL bool I2C_Bus_1_Started;
_DECL bool I2C_Bus_2_Started;
