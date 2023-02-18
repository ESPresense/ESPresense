#include "OBDII.h"

#include <Arduino.h>

#ifdef MACCHINA_A0
#include <esp32_can.h>
#endif

#include "defaults.h"
#include "globals.h"
#include "mqtt.h"

namespace OBDII {

int smoothMilliVolts;

void Setup() {
#if MACCHINA_A0
    if(CAN1.begin(500000))
	{
		Serial.println("MCP2517FD Init OK ...");
        CAN1.watchFor();
	} else {
		Serial.println("MCP2517FD Init Failed ...");
	}

      CAN_FRAME txFrame;
  txFrame.rtr = 0;
  txFrame.id = 0x123;
  txFrame.extended = false;
  txFrame.length = 4;
  txFrame.data.byte[0] = 0x10;
  txFrame.data.byte[1] = 0x1A;
  txFrame.data.byte[2] = 0xFF;
  txFrame.data.byte[3] = 0x5D;
  CAN1.sendFrame(txFrame);
#endif
}

bool SendDiscovery() {
#ifdef MACCHINA_A0
    return sendTeleSensorDiscovery("Battery", EC_NONE, "{{ value_json.batt }}", "battery", "%") && sendTeleBinarySensorDiscovery("Charging", EC_NONE, "{{ value_json.charging }}", "battery_charging");
#else
    return true;
#endif
}

void SendTelemetry() {
#if MACCHINA_A0
    auto mv = a0_read_batt_mv();
    doc["mV"] = mv;
    bool charging = (mv > 13200);
    bool dead = (mv < 11883);
    unsigned int soc = round(-13275.04 + 2.049731 * mv - (0.00007847975 * mv) * mv);
    doc["batt"] = dead ? 0 : (charging ? (unsigned int)100 : max((unsigned int)0, min((unsigned int)100, soc)));
    doc["charging"] = charging ? "ON" : "OFF";
#endif
}
}  // namespace OBDII
