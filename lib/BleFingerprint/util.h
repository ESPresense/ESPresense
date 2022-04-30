#include <Arduino.h>
#include <NimBLEBeacon.h>
#include <NimBLEDevice.h>

#define ENDIAN_CHANGE_U16(x) ((((x)&0xFF00) >> 8) + (((x)&0xFF) << 8))

static BLEUUID eddystoneUUID((uint16_t)0xFEAA);
static BLEUUID tileUUID((uint16_t)0xFEED);
static BLEUUID exposureUUID((uint16_t)0xFD6F);
static BLEUUID smartTagUUID((uint16_t)0xFD5A);
static BLEUUID sonosUUID((uint16_t)0xFE07);
static BLEUUID itagUUID((uint16_t)0xffe0);
static BLEUUID miThermUUID(uint16_t(0x181A));
static BLEUUID trackrUUID((uint16_t)0x0F3E);
static BLEUUID vanmoofUUID(0x6acc5540, 0xe631, 0x4069, 0x944db8ca7598ad50);
static BLEUUID tractiveUUID(0x20130001, 0x0719, 0x4b6e, 0xbe5d158ab92fa5a4);

static BLEUUID fitbitUUID(0xadabfb00, 0x6e7d, 0x4601, 0xbda2bffaa68956ba);

static BLEUUID roomAssistantService(0x5403c8a7, 0x5c96, 0x47e9, 0x9ab859e373d875a7);
static BLEUUID rootAssistantCharacteristic(0x21c46f33, 0xe813, 0x4407, 0x86012ad281030052);

static BLEUUID meaterService(0xa75cc7fc, 0xc956, 0x488f, 0xac2a2dbc08b63a04);
static BLEUUID meaterCharacteristic(0x7edda774, 0x045e, 0x4bbf, 0x909b45d1991a2876);

static BLEUUID genericAccessService(uint16_t(0x1800));
static BLEUUID nameChar(uint16_t(0x2A00));

static BLEUUID deviceInformationService(uint16_t(0x180A));
static BLEUUID modelChar(uint16_t(0x2A24));
static BLEUUID fwRevChar(uint16_t(0x2A26));
static BLEUUID hwRevChar(uint16_t(0x2A27));
static BLEUUID manufChar(uint16_t(0x2A29));

static int median_of_3(int a, int b, int c)
{
    int the_max = max(max(a, b), c);
    int the_min = min(min(a, b), c);
    // unnecessarily clever code
    int the_median = the_max ^ the_min ^ a ^ b ^ c;
    return (the_median);
}
