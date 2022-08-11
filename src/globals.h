#pragma once
#include <Arduino.h>
#include <AsyncMqttClient.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <BleFingerprintCollection.h>

/*----------------------------------------------------------------------------
globals.h

Note: #define VAR_DECLS 1 before including this file to DECLARE and INITIALIZE
global variables.  Include this file without defining VAR_DECLS to extern
these variables.
----------------------------------------------------------------------------*/

/*----------------------------------------------
Setup variable declaration macros.
----------------------------------------------*/
#ifndef VAR_DECLS
# define _DECL extern
# define _INIT(x)
# define _INIT_N(x)
#else
# define _DECL
# define _INIT(x)  = x
# define UNPACK( ... ) __VA_ARGS__
# define _INIT_N(x) UNPACK x
#endif

_DECL BleFingerprintCollection fingerprints;
_DECL char buffer[2048];
_DECL String room, id, statusTopic, teleTopic, roomsTopic, setTopic, configTopic;
_DECL AsyncMqttClient mqttClient;
_DECL DynamicJsonDocument doc _INIT_N(((2048)));
_DECL String localIp;
_DECL AsyncWebSocket ws _INIT_N((("/ws")));
_DECL bool enrolling;
_DECL unsigned long enrollingEndMillis;

// I2C
_DECL int I2C_Bus_1_SDA;
_DECL int I2C_Bus_1_SCL;
_DECL int I2C_Bus_2_SDA;
_DECL int I2C_Bus_2_SCL;
_DECL bool I2C_Bus_1_Enabled;
_DECL bool I2C_Bus_2_Enabled;
