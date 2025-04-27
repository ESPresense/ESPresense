#include <Arduino.h>
#include <NimBLEBeacon.h>
#include <NimBLEDevice.h>
#include <sys/time.h>

#define ENDIAN_CHANGE_U16(x) ((((x)&0xFF00) >> 8) + (((x)&0xFF) << 8))

const BLEUUID eddystoneUUID((uint16_t)0xFEAA);
const BLEUUID tileUUID((uint16_t)0xFEED);
const BLEUUID exposureUUID((uint16_t)0xFD6F);
const BLEUUID smartTagUUID((uint16_t)0xFD5A);
const BLEUUID sonosUUID((uint16_t)0xFE07);
const BLEUUID itagUUID((uint16_t)0xffe0);
const BLEUUID miThermUUID(uint16_t(0x181A));
const BLEUUID trackrUUID((uint16_t)0x0F3E);
const BLEUUID vanmoofUUID(0x6acc5540, 0xe631, 0x4069, 0x944db8ca7598ad50);
const BLEUUID tractiveUUID(0x20130001, 0x0719, 0x4b6e, 0xbe5d158ab92fa5a4);
const BLEUUID espresenseUUID(0xe5ca1ade, 0xf007, 0xba11, 0x0000000000000000);

const BLEUUID nutUUID((uint16_t)0x1803);
const BLEUUID miFloraUUID((uint16_t)0xfe95);

const BLEUUID fitbitUUID(0xadabfb00, 0x6e7d, 0x4601, 0xbda2bffaa68956ba);

const BLEUUID roomAssistantService(0x5403c8a7, 0x5c96, 0x47e9, 0x9ab859e373d875a7);
const BLEUUID rootAssistantCharacteristic(0x21c46f33, 0xe813, 0x4407, 0x86012ad281030052);

const BLEUUID meaterService(0xa75cc7fc, 0xc956, 0x488f, 0xac2a2dbc08b63a04);
const BLEUUID meaterCharacteristic(0x7edda774, 0x045e, 0x4bbf, 0x909b45d1991a2876);

const BLEUUID genericAccessService(uint16_t(0x1800));
const BLEUUID nameChar(uint16_t(0x2A00));

const BLEUUID deviceInformationService(uint16_t(0x180A));
const BLEUUID modelChar(uint16_t(0x2A24));
const BLEUUID fwRevChar(uint16_t(0x2A26));
const BLEUUID hwRevChar(uint16_t(0x2A27));
const BLEUUID manufChar(uint16_t(0x2A29));

uint64_t getNowMs();
