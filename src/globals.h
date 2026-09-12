#pragma once
#include <Arduino.h>
#include <AsyncMqttClient.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <atomic>
#include "Logger.h"
#include "BleFingerprintCollection.h"

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

_DECL String room, id, statusTopic, teleTopic, roomsTopic, setTopic, configTopic;
_DECL AsyncMqttClient mqttClient;
_DECL String homeAssistantDiscoveryPrefix;
_DECL DynamicJsonDocument doc _INIT_N(((768)));
_DECL String localIp;
_DECL AsyncWebSocket ws _INIT_N((("/ws")));
_DECL bool enrolling;
_DECL String enrolledId;
_DECL unsigned long enrollingEndMillis;

// Leak-hunt event counters (#2309). Three candidates fit that report's ~22KB/h at ~100
// bytes per event and its serial log cannot separate them, so count the events instead of
// guessing: regressing freeHeap against these after a soak names the owner. Sampling free
// heap around a call site does not work here — BLE and WiFi allocate from the other core
// mid-measurement — but a monotonic count is noise-free.
//
// std::atomic because allocFails is bumped from heapCapsAllocFailedHook, which runs in
// whatever context the failing allocation did, and fpNew/fpDel are bumped from the scan
// task while the loop task reads them. Aligned 32-bit RMW is lock-free on both the Xtensa
// and RISC-V targets here, so this costs nothing on the BLE path.
_DECL std::atomic<uint32_t> fpNew _INIT(0),        // BleFingerprint allocations
                            fpDel _INIT(0),        // BleFingerprint destructions
                            telePubs _INIT(0),     // successful telemetry publishes
                            mqttRetries _INIT(0),  // pub() retry iterations
                            allocFails _INIT(0);   // heap_caps allocation failures

// I2C
_DECL bool I2C_Bus_1_Started;
_DECL bool I2C_Bus_2_Started;
