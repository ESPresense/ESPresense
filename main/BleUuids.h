#pragma once
#include "Ble.h"

#define ENDIAN_CHANGE_U16(x) ((((x)&0xFF00) >> 8) + (((x)&0xFF) << 8))

inline const ble_uuid_any_t eddystoneUUID = Ble::uuid16(0xFEAA);
inline const ble_uuid_any_t tileUUID = Ble::uuid16(0xFEED);
inline const ble_uuid_any_t exposureUUID = Ble::uuid16(0xFD6F);
inline const ble_uuid_any_t smartTagUUID = Ble::uuid16(0xFD5A);
inline const ble_uuid_any_t sonosUUID = Ble::uuid16(0xFE07);
inline const ble_uuid_any_t itagUUID = Ble::uuid16(0xffe0);
inline const ble_uuid_any_t miThermUUID = Ble::uuid16(0x181A);
inline const ble_uuid_any_t trackrUUID = Ble::uuid16(0x0F3E);
inline const ble_uuid_any_t dexaUUID = Ble::uuid16(0xFEBC);
inline const ble_uuid_any_t vanmoofUUID = Ble::uuid128(0x6acc5540, 0xe631, 0x4069, 0x944db8ca7598ad50);
inline const ble_uuid_any_t tractiveUUID = Ble::uuid128(0x20130001, 0x0719, 0x4b6e, 0xbe5d158ab92fa5a4);
inline const ble_uuid_any_t espresenseUUID = Ble::uuid128(0xe5ca1ade, 0xf007, 0xba11, 0x0000000000000000);

inline const ble_uuid_any_t nutUUID = Ble::uuid16(0x1803);
inline const ble_uuid_any_t miFloraUUID = Ble::uuid16(0xfe95);

inline const ble_uuid_any_t meaterService = Ble::uuid128(0xa75cc7fc, 0xc956, 0x488f, 0xac2a2dbc08b63a04);

inline const ble_uuid_any_t genericAccessService = Ble::uuid16(0x1800);
inline const ble_uuid_any_t nameChar = Ble::uuid16(0x2A00);
inline const ble_uuid_any_t resolvingKeyChar = Ble::uuid16(0x2B2E);

inline const ble_uuid_any_t deviceInformationService = Ble::uuid16(0x180A);
inline const ble_uuid_any_t modelChar = Ble::uuid16(0x2A24);

inline const ble_uuid_any_t batteryService = Ble::uuid16(0x180F);
inline const ble_uuid_any_t batteryLevelChar = Ble::uuid16(0x2A19);
