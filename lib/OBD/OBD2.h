// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef OBD2_H
#define OBD2_H

enum {
  PIDS_SUPPORT_01_20                                = 0x00,
  MONITOR_STATUS_SINCE_DTCS_CLEARED                 = 0x01,
  FREEZE_DTC                                        = 0x02,
  FUEL_SYSTEM_STATUS                                = 0x03,
  CALCULATED_ENGINE_LOAD                            = 0x04,
  ENGINE_COOLANT_TEMPERATURE                        = 0x05,
  SHORT_TERM_FUEL_TRIM_BANK_1                       = 0x06,
  LONG_TERM_FUEL_TRIM_BANK_1                        = 0x07,
  SHORT_TERM_FUEL_TRIM_BANK_2                       = 0x08,
  LONG_TERM_FUEL_TRIM_BANK_2                        = 0x09,
  FUEL_PRESSURE                                     = 0x0a,
  INTAKE_MANIFOLD_ABSOLUTE_PRESSURE                 = 0x0b,
  ENGINE_RPM                                        = 0x0c,
  VEHICLE_SPEED                                     = 0x0d,
  TIMING_ADVANCE                                    = 0x0e,
  AIR_INTAKE_TEMPERATURE                            = 0x0f,
  MAF_AIR_FLOW_RATE                                 = 0x10,
  THROTTLE_POSITION                                 = 0x11,
  COMMANDED_SECONDARY_AIR_STATUS                    = 0x12,
  OXYGEN_SENSORS_PRESENT_IN_2_BANKS                 = 0x13,
  OXYGEN_SENSOR_1_SHORT_TERM_FUEL_TRIM              = 0x14,
  OXYGEN_SENSOR_2_SHORT_TERM_FUEL_TRIM              = 0x15,
  OXYGEN_SENSOR_3_SHORT_TERM_FUEL_TRIM              = 0x16,
  OXYGEN_SENSOR_4_SHORT_TERM_FUEL_TRIM              = 0x17,
  OXYGEN_SENSOR_5_SHORT_TERM_FUEL_TRIM              = 0x18,
  OXYGEN_SENSOR_6_SHORT_TERM_FUEL_TRIM              = 0x19,
  OXYGEN_SENSOR_7_SHORT_TERM_FUEL_TRIM              = 0x1a,
  OXYGEN_SENSOR_8_SHORT_TERM_FUEL_TRIM              = 0x1b,
  OBD_STANDARDS_THIS_VEHICLE_CONFORMS_TO            = 0x1c,
  OXYGEN_SENSORS_PRESENT_IN_4_BANKS                 = 0x1d,
  AUXILIARY_INPUT_STATUS                            = 0x1e,
  RUN_TIME_SINCE_ENGINE_START                       = 0x1f,

  PIDS_SUPPORT_21_40                                = 0x20,
  DISTANCE_TRAVELED_WITH_MIL_ON                     = 0x21,
  FUEL_RAIL_PRESSURE                                = 0x22,
  FUEL_RAIL_GAUGE_PRESSURE                          = 0x23,
  OXYGEN_SENSOR_1_FUEL_AIR_EQUIVALENCE_RATIO        = 0x24,
  OXYGEN_SENSOR_2_FUEL_AIR_EQUIVALENCE_RATIO        = 0x25,
  OXYGEN_SENSOR_3_FUEL_AIR_EQUIVALENCE_RATIO        = 0x26,
  OXYGEN_SENSOR_4_FUEL_AIR_EQUIVALENCE_RATIO        = 0x27,
  OXYGEN_SENSOR_5_FUEL_AIR_EQUIVALENCE_RATIO        = 0x28,
  OXYGEN_SENSOR_6_FUEL_AIR_EQUIVALENCE_RATIO        = 0x29,
  OXYGEN_SENSOR_7_FUEL_AIR_EQUIVALENCE_RATIO        = 0x2a,
  OXYGEN_SENSOR_8_FUEL_AIR_EQUIVALENCE_RATIO        = 0x2b,
  COMMANDED_EGR                                     = 0x2c,
  EGR_ERROR                                         = 0x2d,
  COMMANDED_EVAPORATIVE_PURGE                       = 0x2e,
  FUEL_TANK_LEVEL_INPUT                             = 0x2f,
  WARM_UPS_SINCE_CODES_CLEARED                      = 0x30,
  DISTANCE_TRAVELED_SINCE_CODES_CLEARED             = 0x31,
  EVAP_SYSTEM_VAPOR_PRESSURE                        = 0x32,
  ABSOLULTE_BAROMETRIC_PRESSURE                     = 0x33,
/*OXYGEN_SENSOR_1_FUEL_AIR_EQUIVALENCE_RATIO        = 0x34,
  OXYGEN_SENSOR_2_FUEL_AIR_EQUIVALENCE_RATIO        = 0x35,
  OXYGEN_SENSOR_3_FUEL_AIR_EQUIVALENCE_RATIO        = 0x36,
  OXYGEN_SENSOR_4_FUEL_AIR_EQUIVALENCE_RATIO        = 0x37,
  OXYGEN_SENSOR_5_FUEL_AIR_EQUIVALENCE_RATIO        = 0x38,
  OXYGEN_SENSOR_6_FUEL_AIR_EQUIVALENCE_RATIO        = 0x39,
  OXYGEN_SENSOR_7_FUEL_AIR_EQUIVALENCE_RATIO        = 0x3a,
  OXYGEN_SENSOR_8_FUEL_AIR_EQUIVALENCE_RATIO        = 0x3b,*/
  CATALYST_TEMPERATURE_BANK_1_SENSOR_1              = 0x3c,
  CATALYST_TEMPERATURE_BANK_2_SENSOR_1              = 0x3d,
  CATALYST_TEMPERATURE_BANK_1_SENSOR_2              = 0x3e,
  CATALYST_TEMPERATURE_BANK_2_SENSOR_2              = 0x3f,

  PIDS_SUPPORT_41_60                                = 0x40,
  MONITOR_STATUS_THIS_DRIVE_CYCLE                   = 0x41,
  CONTROL_MODULE_VOLTAGE                            = 0x42,
  ABSOLUTE_LOAD_VALUE                               = 0x43,
  FUEL_AIR_COMMANDED_EQUIVALENCE_RATE               = 0x44,
  RELATIVE_THROTTLE_POSITION                        = 0x45,
  AMBIENT_AIR_TEMPERATURE                           = 0x46,
  ABSOLUTE_THROTTLE_POSITION_B                      = 0x47,
  ABSOLUTE_THROTTLE_POSITION_C                      = 0x48,
  ABSOLUTE_THROTTLE_POSITION_D                      = 0x49,
  ABSOLUTE_THROTTLE_POSITION_E                      = 0x4a,
  ABSOLUTE_THROTTLE_POSITION_F                      = 0x4b,
  COMMANDED_THROTTLE_ACTUATOR                       = 0x4c,
  TIME_RUN_WITH_MIL_ON                              = 0x4d,
  TIME_SINCE_TROUBLE_CODES_CLEARED                  = 0x4e,
/*                                                  = 0x4f,
                                                    = 0x50,*/
  FUEL_TYPE                                         = 0x51,
  ETHANOL_FUEL_PERCENTAGE                           = 0x52,
  ABSOLUTE_EVAP_SYSTEM_VAPOR_PRESSURE               = 0x53,
/*EVAP_SYSTEM_VAPOR_PRESSURE                        = 0x54,*/
/*                                                  = 0x55,
                                                    = 0x56,
                                                    = 0x57,
                                                    = 0x58,*/
  FUEL_RAIL_ABSOLUTE_PRESSURE                       = 0x59,
  RELATIVE_ACCELERATOR_PEDAL_POSITTION              = 0x5a,
  HYBRID_BATTERY_PACK_REMAINING_LIFE                = 0x5b,
  ENGINE_OIL_TEMPERATURE                            = 0x5c,
  FUEL_INJECTION_TIMING                             = 0x5d,
  ENGINE_FUEL_RATE                                  = 0x5e,
  EMISSION_REQUIREMENT_TO_WHICH_VEHICLE_IS_DESIGNED = 0x5f,

  // more PIDs can be added from: https://en.wikipedia.org/wiki/OBD-II_PIDs
};

// default timeout for a response in milliseconds
#define OBD2_DEFAULT_TIMEOUT 2000

class OBD2Class {
public:
  OBD2Class();
  virtual ~OBD2Class();

  int begin();
  void end();

  bool pidSupported(uint8_t pid);
  bool pidValueRaw(uint8_t pid);
  String pidName(uint8_t pid);
  String pidUnits(uint8_t pid);

  float pidRead(uint8_t pid);
  uint32_t pidReadRaw(uint8_t pid);

  String vinRead();
  String ecuNameRead();

  void setTimeout(unsigned long timeout);

  int clearAllStoredDTC();

private:
  int supportedPidsRead();

  int pidRead(uint8_t mode, uint8_t pid, void* data, int length);

private:
  unsigned long _responseTimeout;
  bool _useExtendedAddressing;
  unsigned long _lastPidResponseMillis;
  uint32_t _supportedPids[32];
};

extern OBD2Class OBD2;

#endif
