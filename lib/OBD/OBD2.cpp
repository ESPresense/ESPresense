// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <math.h>

#ifdef __AVR__
#include <avr/pgmspace.h>
#else
#define PROGMEM
#endif

#define CAN CAN1
#include <esp32_can.h>

#include "OBD2.h"

const char PID_NAME_0x00[] PROGMEM = "PIDs supported [01 - 20]";
const char PID_NAME_0x01[] PROGMEM = "Monitor status since DTCs cleared";
const char PID_NAME_0x02[] PROGMEM = "Freeze DTC";
const char PID_NAME_0x03[] PROGMEM = "Fuel system status";
const char PID_NAME_0x04[] PROGMEM = "Calculated engine load";
const char PID_NAME_0x05[] PROGMEM = "Engine coolant temperature";
const char PID_NAME_0x06[] PROGMEM = "Short term fuel trim — Bank 1";
const char PID_NAME_0x07[] PROGMEM = "Long term fuel trim — Bank 1";
const char PID_NAME_0x08[] PROGMEM = "Short term fuel trim — Bank 2";
const char PID_NAME_0x09[] PROGMEM = "Long term fuel trim — Bank 2";
const char PID_NAME_0x0a[] PROGMEM = "Fuel pressure";
const char PID_NAME_0x0b[] PROGMEM = "Intake manifold absolute pressure";
const char PID_NAME_0x0c[] PROGMEM = "Engine RPM";
const char PID_NAME_0x0d[] PROGMEM = "Vehicle speed";
const char PID_NAME_0x0e[] PROGMEM = "Timing advance";
const char PID_NAME_0x0f[] PROGMEM = "Intake air temperature";
const char PID_NAME_0x10[] PROGMEM = "MAF air flow rate";
const char PID_NAME_0x11[] PROGMEM = "Throttle position";
const char PID_NAME_0x12[] PROGMEM = "Commanded secondary air status";
const char PID_NAME_0x13[] PROGMEM = "Oxygen sensors present (in 2 banks)";
const char PID_NAME_0x14[] PROGMEM = "Oxygen Sensor 1 - Short term fuel trim";
const char PID_NAME_0x15[] PROGMEM = "Oxygen Sensor 2 - Short term fuel trim";
const char PID_NAME_0x16[] PROGMEM = "Oxygen Sensor 3 - Short term fuel trim";
const char PID_NAME_0x17[] PROGMEM = "Oxygen Sensor 4 - Short term fuel trim";
const char PID_NAME_0x18[] PROGMEM = "Oxygen Sensor 5 - Short term fuel trim";
const char PID_NAME_0x19[] PROGMEM = "Oxygen Sensor 6 - Short term fuel trim";
const char PID_NAME_0x1a[] PROGMEM = "Oxygen Sensor 7 - Short term fuel trim";
const char PID_NAME_0x1b[] PROGMEM = "Oxygen Sensor 8 - Short term fuel trim";
const char PID_NAME_0x1c[] PROGMEM = "OBD standards this vehicle conforms to";
const char PID_NAME_0x1d[] PROGMEM = "Oxygen sensors present (in 4 banks)";
const char PID_NAME_0x1e[] PROGMEM = "Auxiliary input status";
const char PID_NAME_0x1f[] PROGMEM = "Run time since engine start";
const char PID_NAME_0x20[] PROGMEM = "PIDs supported [21 - 40]";
const char PID_NAME_0x21[] PROGMEM = "Distance traveled with malfunction indicator lamp (MIL) on";
const char PID_NAME_0x22[] PROGMEM = "Fuel Rail Pressure (relative to manifold vacuum)";
const char PID_NAME_0x23[] PROGMEM = "Fuel Rail Gauge Pressure (diesel, or gasoline direct injection)";
const char PID_NAME_0x24[] PROGMEM = "Oxygen Sensor 1 - Fuel–Air Equivalence Ratio";
const char PID_NAME_0x25[] PROGMEM = "Oxygen Sensor 2 - Fuel–Air Equivalence Ratio";
const char PID_NAME_0x26[] PROGMEM = "Oxygen Sensor 3 - Fuel–Air Equivalence Ratio";
const char PID_NAME_0x27[] PROGMEM = "Oxygen Sensor 4 - Fuel–Air Equivalence Ratio";
const char PID_NAME_0x28[] PROGMEM = "Oxygen Sensor 5 - Fuel–Air Equivalence Ratio";
const char PID_NAME_0x29[] PROGMEM = "Oxygen Sensor 6 - Fuel–Air Equivalence Ratio";
const char PID_NAME_0x2a[] PROGMEM = "Oxygen Sensor 7 - Fuel–Air Equivalence Ratio";
const char PID_NAME_0x2b[] PROGMEM = "Oxygen Sensor 8 - Fuel–Air Equivalence Ratio";
const char PID_NAME_0x2c[] PROGMEM = "Commanded EGR";
const char PID_NAME_0x2d[] PROGMEM = "EGR Error";
const char PID_NAME_0x2e[] PROGMEM = "Commanded evaporative purge";
const char PID_NAME_0x2f[] PROGMEM = "Fuel Tank Level Input";
const char PID_NAME_0x30[] PROGMEM = "Warm-ups since codes cleared";
const char PID_NAME_0x31[] PROGMEM = "Distance traveled since codes cleared";
const char PID_NAME_0x32[] PROGMEM = "Evap. System Vapor Pressure";
const char PID_NAME_0x33[] PROGMEM = "Absolute Barometric Pressure";
const char PID_NAME_0x34[] PROGMEM = "Oxygen Sensor 1 - Fuel–Air Equivalence Ratio";
const char PID_NAME_0x35[] PROGMEM = "Oxygen Sensor 2 - Fuel–Air Equivalence Ratio";
const char PID_NAME_0x36[] PROGMEM = "Oxygen Sensor 3 - Fuel–Air Equivalence Ratio";
const char PID_NAME_0x37[] PROGMEM = "Oxygen Sensor 4 - Fuel–Air Equivalence Ratio";
const char PID_NAME_0x38[] PROGMEM = "Oxygen Sensor 5 - Fuel–Air Equivalence Ratio";
const char PID_NAME_0x39[] PROGMEM = "Oxygen Sensor 6 - Fuel–Air Equivalence Ratio";
const char PID_NAME_0x3a[] PROGMEM = "Oxygen Sensor 7 - Fuel–Air Equivalence Ratio";
const char PID_NAME_0x3b[] PROGMEM = "Oxygen Sensor 8 - Fuel–Air Equivalence Ratio";
const char PID_NAME_0x3c[] PROGMEM = "Catalyst Temperature: Bank 1, Sensor 1";
const char PID_NAME_0x3d[] PROGMEM = "Catalyst Temperature: Bank 2, Sensor 1";
const char PID_NAME_0x3e[] PROGMEM = "Catalyst Temperature: Bank 1, Sensor 2";
const char PID_NAME_0x3f[] PROGMEM = "Catalyst Temperature: Bank 2, Sensor 2";
const char PID_NAME_0x40[] PROGMEM = "PIDs supported [41 - 60]";
const char PID_NAME_0x41[] PROGMEM = "Monitor status this drive cycle";
const char PID_NAME_0x42[] PROGMEM = "Control module voltage";
const char PID_NAME_0x43[] PROGMEM = "Absolute load value";
const char PID_NAME_0x44[] PROGMEM = "Fuel–Air commanded equivalence ratio";
const char PID_NAME_0x45[] PROGMEM = "Relative throttle position";
const char PID_NAME_0x46[] PROGMEM = "Ambient air temperature";
const char PID_NAME_0x47[] PROGMEM = "Absolute throttle position B";
const char PID_NAME_0x48[] PROGMEM = "Absolute throttle position C";
const char PID_NAME_0x49[] PROGMEM = "Absolute throttle position D";
const char PID_NAME_0x4a[] PROGMEM = "Absolute throttle position E";
const char PID_NAME_0x4b[] PROGMEM = "Absolute throttle position F";
const char PID_NAME_0x4c[] PROGMEM = "Commanded throttle actuator";
const char PID_NAME_0x4d[] PROGMEM = "Time run with MIL on";
const char PID_NAME_0x4e[] PROGMEM = "Time since trouble codes cleared";
const char PID_NAME_0x4f[] PROGMEM = "Maximum value for Fuel–Air equivalence ratio, oxygen sensor voltage, oxygen sensor current, and intake manifold absolute pressure";
const char PID_NAME_0x50[] PROGMEM = "Maximum value for air flow rate from mass air flow sensor";
const char PID_NAME_0x51[] PROGMEM = "Fuel Type";
const char PID_NAME_0x52[] PROGMEM = "Ethanol fuel percentage";
const char PID_NAME_0x53[] PROGMEM = "Absolute Evap system Vapor Pressure";
const char PID_NAME_0x54[] PROGMEM = "Evap system vapor pressure";
const char PID_NAME_0x55[] PROGMEM = "Short term secondary oxygen sensor trim";
const char PID_NAME_0x56[] PROGMEM = "Long term secondary oxygen sensor trim";
const char PID_NAME_0x57[] PROGMEM = "Short term secondary oxygen sensor trim";
const char PID_NAME_0x58[] PROGMEM = "Long term secondary oxygen sensor trim";
const char PID_NAME_0x59[] PROGMEM = "Fuel rail absolute pressure";
const char PID_NAME_0x5a[] PROGMEM = "Relative accelerator pedal position";
const char PID_NAME_0x5b[] PROGMEM = "Hybrid battery pack remaining life";
const char PID_NAME_0x5c[] PROGMEM = "Engine oil temperature";
const char PID_NAME_0x5d[] PROGMEM = "Fuel injection timing";
const char PID_NAME_0x5e[] PROGMEM = "Engine fuel rate";
const char PID_NAME_0x5f[] PROGMEM = "Emission requirements to which vehicle is designed";

const char* const PID_NAME_MAPPER[] PROGMEM = {
  PID_NAME_0x00,
  PID_NAME_0x01,
  PID_NAME_0x02,
  PID_NAME_0x03,
  PID_NAME_0x04,
  PID_NAME_0x05,
  PID_NAME_0x06,
  PID_NAME_0x07,
  PID_NAME_0x08,
  PID_NAME_0x09,
  PID_NAME_0x0a,
  PID_NAME_0x0b,
  PID_NAME_0x0c,
  PID_NAME_0x0d,
  PID_NAME_0x0e,
  PID_NAME_0x0f,
  PID_NAME_0x10,
  PID_NAME_0x11,
  PID_NAME_0x12,
  PID_NAME_0x13,
  PID_NAME_0x14,
  PID_NAME_0x15,
  PID_NAME_0x16,
  PID_NAME_0x17,
  PID_NAME_0x18,
  PID_NAME_0x19,
  PID_NAME_0x1a,
  PID_NAME_0x1b,
  PID_NAME_0x1c,
  PID_NAME_0x1d,
  PID_NAME_0x1e,
  PID_NAME_0x1f,
  PID_NAME_0x20,
  PID_NAME_0x21,
  PID_NAME_0x22,
  PID_NAME_0x23,
  PID_NAME_0x24,
  PID_NAME_0x25,
  PID_NAME_0x26,
  PID_NAME_0x27,
  PID_NAME_0x28,
  PID_NAME_0x29,
  PID_NAME_0x2a,
  PID_NAME_0x2b,
  PID_NAME_0x2c,
  PID_NAME_0x2d,
  PID_NAME_0x2e,
  PID_NAME_0x2f,
  PID_NAME_0x30,
  PID_NAME_0x31,
  PID_NAME_0x32,
  PID_NAME_0x33,
  PID_NAME_0x34,
  PID_NAME_0x35,
  PID_NAME_0x36,
  PID_NAME_0x37,
  PID_NAME_0x38,
  PID_NAME_0x39,
  PID_NAME_0x3a,
  PID_NAME_0x3b,
  PID_NAME_0x3c,
  PID_NAME_0x3d,
  PID_NAME_0x3e,
  PID_NAME_0x3f,
  PID_NAME_0x40,
  PID_NAME_0x41,
  PID_NAME_0x42,
  PID_NAME_0x43,
  PID_NAME_0x44,
  PID_NAME_0x45,
  PID_NAME_0x46,
  PID_NAME_0x47,
  PID_NAME_0x48,
  PID_NAME_0x49,
  PID_NAME_0x4a,
  PID_NAME_0x4b,
  PID_NAME_0x4c,
  PID_NAME_0x4d,
  PID_NAME_0x4e,
  PID_NAME_0x4f,
  PID_NAME_0x50,
  PID_NAME_0x51,
  PID_NAME_0x52,
  PID_NAME_0x53,
  PID_NAME_0x54,
  PID_NAME_0x55,
  PID_NAME_0x56,
  PID_NAME_0x57,
  PID_NAME_0x58,
  PID_NAME_0x59,
  PID_NAME_0x5a,
  PID_NAME_0x5b,
  PID_NAME_0x5c,
  PID_NAME_0x5d,
  PID_NAME_0x5e,
  PID_NAME_0x5f,
};

const char PERCENTAGE[] PROGMEM = "%";
const char KPA[] PROGMEM = "kPa";
const char PA[] PROGMEM = "Pa";
const char RPM[] PROGMEM = "rpm";
const char KPH[] PROGMEM = "km/h";
const char DEGREES_BEFORE_TDC[] PROGMEM = "° before TDC";
const char GRAMS_PER_SECOND[] PROGMEM = "grams/sec";
const char SECONDS[] PROGMEM = "seconds";
const char RATIO[] PROGMEM = "ratio";
const char COUNT[] PROGMEM = "count";
const char KM[] PROGMEM = "km";
const char VOLTS[] PROGMEM = "V";
const char MINUTES[] PROGMEM = "minutes";
const char GPS[] PROGMEM = "g/s";
const char DEGREES[] PROGMEM = "°";
const char DEGREES_CELCIUS[] PROGMEM = "°C";
const char LPH[] PROGMEM = "L/h";

const char* const PID_UNIT_MAPPER[] PROGMEM = {
  NULL,
  NULL,
  NULL,
  NULL,
  PERCENTAGE,
  DEGREES_CELCIUS,
  PERCENTAGE,
  PERCENTAGE,
  PERCENTAGE,
  PERCENTAGE,
  KPA,
  KPA,
  RPM,
  KPH,
  DEGREES_BEFORE_TDC,
  DEGREES_CELCIUS,
  GRAMS_PER_SECOND,
  PERCENTAGE,
  NULL,
  NULL,
  PERCENTAGE,
  PERCENTAGE,
  PERCENTAGE,
  PERCENTAGE,
  PERCENTAGE,
  PERCENTAGE,
  PERCENTAGE,
  PERCENTAGE,
  NULL,
  NULL,
  NULL,
  SECONDS,
  NULL,
  KM,
  KPA,
  KPA,
  RATIO,
  RATIO,
  RATIO,
  RATIO,
  RATIO,
  RATIO,
  RATIO,
  RATIO,
  PERCENTAGE,
  PERCENTAGE,
  PERCENTAGE,
  PERCENTAGE,
  COUNT,
  KM,
  PA,
  KPA,
  RATIO,
  RATIO,
  RATIO,
  RATIO,
  RATIO,
  RATIO,
  RATIO,
  RATIO,
  DEGREES_CELCIUS,
  DEGREES_CELCIUS,
  DEGREES_CELCIUS,
  DEGREES_CELCIUS,
  NULL,
  NULL,
  VOLTS,
  PERCENTAGE,
  RATIO,
  PERCENTAGE,
  DEGREES_CELCIUS,
  PERCENTAGE,
  PERCENTAGE,
  PERCENTAGE,
  PERCENTAGE,
  PERCENTAGE,
  PERCENTAGE,
  MINUTES,
  MINUTES,
  NULL,
  GPS,
  NULL,
  PERCENTAGE,
  KPA,
  PA,
  PERCENTAGE,
  PERCENTAGE,
  PERCENTAGE,
  PERCENTAGE,
  KPA,
  PERCENTAGE,
  PERCENTAGE,
  DEGREES_CELCIUS,
  DEGREES,
  LPH,
  NULL,
};

OBD2Class::OBD2Class() :
  _responseTimeout(OBD2_DEFAULT_TIMEOUT),
  _lastPidResponseMillis(0)
{
  memset(_supportedPids, 0x00, sizeof(_supportedPids));
}

OBD2Class::~OBD2Class()
{
}

int OBD2Class::begin()
{
  if (!CAN.begin(500E3)) {
    return 0;
  }

  memset(_supportedPids, 0x00, sizeof(_supportedPids));

  // first try standard addressing
  _useExtendedAddressing = false;
  CAN.filter(0x7e8);
  if (!supportedPidsRead()) {
    // next try extended addressing
    _useExtendedAddressing = true;
    CAN.filterExtended(0x18daf110);

    if (!supportedPidsRead()) {
      return 0;
    }
  }

  return 1;
}


void OBD2Class::end()
{
  CAN.end();
}

bool OBD2Class::pidSupported(uint8_t pid)
{
  if (pid == 0) {
    return true;
  }

  return (_supportedPids[(pid - 1) / 0x20] & ((1UL << (31 - ((pid - 1) % 32))))) ? true : false;
}

bool OBD2Class::pidValueRaw(uint8_t pid)
{
  switch(pid) {
    case PIDS_SUPPORT_01_20: // raw
    case MONITOR_STATUS_SINCE_DTCS_CLEARED: // raw
    case FREEZE_DTC: // raw
    case PIDS_SUPPORT_21_40: // raw
    case PIDS_SUPPORT_41_60: // raw
    case MONITOR_STATUS_THIS_DRIVE_CYCLE: // raw
    case FUEL_SYSTEM_STATUS: // raw
    case COMMANDED_SECONDARY_AIR_STATUS: // raw
    case OBD_STANDARDS_THIS_VEHICLE_CONFORMS_TO: // raw
    case OXYGEN_SENSORS_PRESENT_IN_2_BANKS: // raw
    case OXYGEN_SENSORS_PRESENT_IN_4_BANKS: // raw
    case AUXILIARY_INPUT_STATUS: // raw
    case FUEL_TYPE: // raw
    case EMISSION_REQUIREMENT_TO_WHICH_VEHICLE_IS_DESIGNED: // raw
      return true;

    default:
      return (pid > 0x5f);
  }
}

String OBD2Class::pidName(uint8_t pid)
{
  if (pid > 0x5f) {
    return "Unknown";
  }

#ifdef __AVR__
  const char* pgmName = pgm_read_ptr(&PID_NAME_MAPPER[pid]);
  String name;

  if (pgmName != NULL) {
    while (char c = pgm_read_byte(pgmName++)) {
      name += c;
    }
  }

  return name;
#else
  return PID_NAME_MAPPER[pid];
#endif
}

String OBD2Class::pidUnits(uint8_t pid)
{
  if (pid > 0x5f) {
    return "";
  }

#ifdef __AVR__
  const char* pgmUnits = pgm_read_ptr(&PID_UNIT_MAPPER[pid]);
  String units;

  if (pgmUnits != NULL) {
    while (char c = pgm_read_byte(pgmUnits++)) {
      units += c;
    }
  }

  return units;
#else
  return PID_UNIT_MAPPER[pid];
#endif
}

float OBD2Class::pidRead(uint8_t pid)
{
  if (!pidSupported(pid)) {
    return NAN;
  }

  #define A value[0]
  #define B value[1]
  #define C value[2]
  #define D value[3]
  uint8_t value[4];

  if (!pidRead(0x01, pid, &value, sizeof(value))) {
    return NAN;
  }

  switch (pid) {
    default:
    case PIDS_SUPPORT_01_20: // raw
    case MONITOR_STATUS_SINCE_DTCS_CLEARED: // raw
    case FREEZE_DTC: // raw
    case PIDS_SUPPORT_21_40: // raw
    case PIDS_SUPPORT_41_60: // raw
    case MONITOR_STATUS_THIS_DRIVE_CYCLE: // raw
      // NOTE: return value can lose precision!
      return ((uint32_t)A << 24 | (uint32_t)B << 16 | (uint32_t)C << 8 | (uint32_t)D);

    case FUEL_SYSTEM_STATUS: // raw
    case RUN_TIME_SINCE_ENGINE_START:
    case DISTANCE_TRAVELED_WITH_MIL_ON:
    case DISTANCE_TRAVELED_SINCE_CODES_CLEARED:
    case TIME_RUN_WITH_MIL_ON:
    case TIME_SINCE_TROUBLE_CODES_CLEARED:
      return (A * 256.0 + B);

    case CALCULATED_ENGINE_LOAD:
    case THROTTLE_POSITION:
    case COMMANDED_EGR:
    case COMMANDED_EVAPORATIVE_PURGE:
    case FUEL_TANK_LEVEL_INPUT:
    case RELATIVE_THROTTLE_POSITION:
    case ABSOLUTE_THROTTLE_POSITION_B:
    case ABSOLUTE_THROTTLE_POSITION_C:
    case ABSOLUTE_THROTTLE_POSITION_D:
    case ABSOLUTE_THROTTLE_POSITION_E:
    case ABSOLUTE_THROTTLE_POSITION_F:
    case COMMANDED_THROTTLE_ACTUATOR:
    case ETHANOL_FUEL_PERCENTAGE:
    case RELATIVE_ACCELERATOR_PEDAL_POSITTION:
    case HYBRID_BATTERY_PACK_REMAINING_LIFE:
      return (A / 2.55);

    case COMMANDED_SECONDARY_AIR_STATUS: // raw
    case OBD_STANDARDS_THIS_VEHICLE_CONFORMS_TO: // raw
    case OXYGEN_SENSORS_PRESENT_IN_2_BANKS: // raw
    case OXYGEN_SENSORS_PRESENT_IN_4_BANKS: // raw
    case AUXILIARY_INPUT_STATUS: // raw
    case FUEL_TYPE: // raw
    case EMISSION_REQUIREMENT_TO_WHICH_VEHICLE_IS_DESIGNED: // raw
      return (A);

    case OXYGEN_SENSOR_1_SHORT_TERM_FUEL_TRIM:
    case OXYGEN_SENSOR_2_SHORT_TERM_FUEL_TRIM:
    case OXYGEN_SENSOR_3_SHORT_TERM_FUEL_TRIM:
    case OXYGEN_SENSOR_4_SHORT_TERM_FUEL_TRIM:
    case OXYGEN_SENSOR_5_SHORT_TERM_FUEL_TRIM:
    case OXYGEN_SENSOR_6_SHORT_TERM_FUEL_TRIM:
    case OXYGEN_SENSOR_7_SHORT_TERM_FUEL_TRIM:
    case OXYGEN_SENSOR_8_SHORT_TERM_FUEL_TRIM:
      return ((B / 1.28) - 100.0);
      break;

    case ENGINE_COOLANT_TEMPERATURE:
    case AIR_INTAKE_TEMPERATURE:
    case AMBIENT_AIR_TEMPERATURE:
    case ENGINE_OIL_TEMPERATURE:
      return (A - 40.0);

    case SHORT_TERM_FUEL_TRIM_BANK_1:
    case LONG_TERM_FUEL_TRIM_BANK_1:
    case SHORT_TERM_FUEL_TRIM_BANK_2:
    case LONG_TERM_FUEL_TRIM_BANK_2:
    case EGR_ERROR:
      return ((A / 1.28) - 100.0);

    case FUEL_PRESSURE:
      return (A * 3.0);

    case INTAKE_MANIFOLD_ABSOLUTE_PRESSURE:
    case VEHICLE_SPEED:
    case WARM_UPS_SINCE_CODES_CLEARED:
    case ABSOLULTE_BAROMETRIC_PRESSURE:
      return (A);

    case ENGINE_RPM:
      return ((A * 256.0 + B) / 4.0);

    case TIMING_ADVANCE:
      return ((A / 2.0) - 64.0);

    case MAF_AIR_FLOW_RATE:
      return ((A * 256.0 + B) / 100.0);

    case FUEL_RAIL_PRESSURE:
      return ((A * 256.0 + B) * 0.079);

    case FUEL_RAIL_GAUGE_PRESSURE:
    case FUEL_RAIL_ABSOLUTE_PRESSURE:
      return ((A * 256.0 + B) * 10.0);

    case OXYGEN_SENSOR_1_FUEL_AIR_EQUIVALENCE_RATIO:
    case OXYGEN_SENSOR_2_FUEL_AIR_EQUIVALENCE_RATIO:
    case OXYGEN_SENSOR_3_FUEL_AIR_EQUIVALENCE_RATIO:
    case OXYGEN_SENSOR_4_FUEL_AIR_EQUIVALENCE_RATIO:
    case OXYGEN_SENSOR_5_FUEL_AIR_EQUIVALENCE_RATIO:
    case OXYGEN_SENSOR_6_FUEL_AIR_EQUIVALENCE_RATIO:
    case OXYGEN_SENSOR_7_FUEL_AIR_EQUIVALENCE_RATIO:
    case OXYGEN_SENSOR_8_FUEL_AIR_EQUIVALENCE_RATIO:
    case 0x34:
    case 0x35:
    case 0x36:
    case 0x37:
    case 0x38:
    case 0x39:
    case 0x3a:
    case 0x3b:
      return (((A * 256.0 + B) * 2.0) / 65536.0);

    case EVAP_SYSTEM_VAPOR_PRESSURE:
      return (((int16_t)(A * 256.0 + B)) / 4.0);

    case CATALYST_TEMPERATURE_BANK_1_SENSOR_1:
    case CATALYST_TEMPERATURE_BANK_2_SENSOR_1:
    case CATALYST_TEMPERATURE_BANK_1_SENSOR_2:
    case CATALYST_TEMPERATURE_BANK_2_SENSOR_2:
      return (((A * 256.0 + B) / 10.0) - 40.0);

    case CONTROL_MODULE_VOLTAGE:
      return ((A * 256.0 + B) / 1000.0);

    case ABSOLUTE_LOAD_VALUE:
      return ((A * 256.0 + B) / 2.55);

    case FUEL_AIR_COMMANDED_EQUIVALENCE_RATE:
      return (2.0 * (A * 256.0 + B) / 65536.0);

    case ABSOLUTE_EVAP_SYSTEM_VAPOR_PRESSURE:
      return ((A * 256.0 + B) / 200.0);

    case 0x54:
      return ((A * 256.0 + B) - 32767.0);

    case FUEL_INJECTION_TIMING:
      return (((A * 256.0 + B) / 128.0) - 210.0);

    case ENGINE_FUEL_RATE:
      return ((A * 256.0 + B) / 20.0);
  }
}

String OBD2Class::vinRead()
{
  char vin[18];

  memset(vin, 0x00, sizeof(vin));

  if (!pidRead(0x09, 0x02, vin, 17)) {
    // failed
    return "";
  }

  return vin;
}

uint32_t OBD2Class::pidReadRaw(uint8_t pid)
{
  if (!pidSupported(pid)) {
    return 0;
  }

  #define A value[0]
  #define B value[1]
  #define C value[2]
  #define D value[3]
  uint8_t value[4];

  if (!pidRead(0x01, pid, &value, sizeof(value))) {
    return 0;
  }

  switch (pid) {
    case COMMANDED_SECONDARY_AIR_STATUS:
    case OBD_STANDARDS_THIS_VEHICLE_CONFORMS_TO:
    case OXYGEN_SENSORS_PRESENT_IN_2_BANKS:
    case OXYGEN_SENSORS_PRESENT_IN_4_BANKS:
    case AUXILIARY_INPUT_STATUS:
    case FUEL_TYPE:
    case EMISSION_REQUIREMENT_TO_WHICH_VEHICLE_IS_DESIGNED:
      return (A);

    case FUEL_SYSTEM_STATUS:
      return ((uint32_t)A << 8 | (uint32_t)B);

    default:
      return ((uint32_t)A << 24 | (uint32_t)B << 16 | (uint32_t)C << 8 | (uint32_t)D);
  }
}

String OBD2Class::ecuNameRead()
{
  char ecuName[21];

  memset(ecuName, 0x00, sizeof(ecuName));

  if (!pidRead(0x09, 0x0a, ecuName, 20)) {
    // failed
    return "";
  }

  return ecuName;
}

void OBD2Class::setTimeout(unsigned long timeout)
{
  _responseTimeout = timeout;
}

int OBD2Class::supportedPidsRead()
{
  for (int pid = 0x00; pid < 0xe0; pid += 0x20) {
    uint8_t value[4];

    if (pidRead(0x01, pid, value, sizeof(value)) != 4) {
      return 0;
    }

    #define A value[0]
    #define B value[1]
    #define C value[2]
    #define D value[3]
    _supportedPids[pid / 0x20] = ((uint32_t)A << 24 | (uint32_t)B << 16 | (uint32_t)C << 8 | (uint32_t)D << 0);

    if ((_supportedPids[pid / 0x20] & 0x00000001) == 0x00000000) {
      // no more
      break;
    }
  }

  return 1;
}

int OBD2Class::clearAllStoredDTC()
{
    //Function clears stored Diagnostic Trouble Codes (DTC)

    // make sure at least 60 ms have passed since the last response
    unsigned long lastResponseDelta = millis() - _lastPidResponseMillis;
    if (lastResponseDelta < 60) {
        delay(60 - lastResponseDelta);
    }

    for (int retries = 10; retries > 0; retries--) {
        if (_useExtendedAddressing) {
            CAN.beginExtendedPacket(0x18db33f1, 8);
        } else {
            CAN.beginPacket(0x7df, 8);
        }
        CAN.write(0x00); // number of additional bytes
        CAN.write(0x04); // Mode / Service 4, for clearing DTC
        if (CAN.endPacket()) {
            // send success
            break;
        } else if (retries <= 1) {
            return 0;
        }
    }

    return 1;
}

int OBD2Class::pidRead(uint8_t mode, uint8_t pid, void* data, int length)
{
  // make sure at least 60 ms have passed since the last response
  unsigned long lastResponseDelta = millis() - _lastPidResponseMillis;
  if (lastResponseDelta < 60) {
    delay(60 - lastResponseDelta);
  }

  for (int retries = 10; retries > 0; retries--) {
    if (_useExtendedAddressing) {
      CAN.beginExtendedPacket(0x18db33f1, 8);
    } else {
      CAN.beginPacket(0x7df, 8);
    }
    CAN.write(0x02); // number of additional bytes
    CAN.write(mode);
    CAN.write(pid);
    if (CAN.endPacket()) {
      // send success
      break;
    } else if (retries <= 1) {
      return 0;
    }
  }

  bool splitResponse = (length > 5);

  for (unsigned long start = millis(); (millis() - start) < _responseTimeout;) {
    if (CAN.parsePacket() != 0 &&
          (splitResponse ? (CAN.read() == 0x10 && CAN.read()) : CAN.read()) &&
          (CAN.read() == (mode | 0x40) && CAN.read() == pid)) {

      _lastPidResponseMillis = millis();

      // got a response
      if (!splitResponse) {
        return CAN.readBytes((uint8_t*)data, length);
      }

      int read = CAN.readBytes((uint8_t*)data, 3);

      for (int i = 0; read < length; i++) {
        delay(60);

        // send the request for the next chunk
        if (_useExtendedAddressing) {
          CAN.beginExtendedPacket(0x18db33f1, 8);
        } else {
          CAN.beginPacket(0x7df, 8);
        }
        CAN.write(0x30);
        CAN.endPacket();

        // wait for response
        while (CAN.parsePacket() == 0 ||
               CAN.read() != (0x21 + i)); // correct sequence number

        while (CAN.available()) {
          ((uint8_t*)data)[read++] = CAN.read();
        }
      }

      _lastPidResponseMillis = millis();

      return read;
    }
  }

  return 0;
}

OBD2Class OBD2;
