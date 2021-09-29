#include <Arduino.h>
#include <NimBLEBeacon.h>
#include <NimBLEDevice.h>

#define ENDIAN_CHANGE_U16(x) ((((x)&0xFF00) >> 8) + (((x)&0xFF) << 8))

static BLEUUID eddystoneUUID((uint16_t)0xFEAA);
static BLEUUID tileUUID((uint16_t)0xFEED);
static BLEUUID exposureUUID((uint16_t)0xFD6F);
static BLEUUID sonosUUID((uint16_t)0xFE07);
static BLEUUID itagUUID((uint16_t)0xffe0);

static BLEUUID roomAssistantService(uint32_t(0x5403c8a7), uint16_t(0x5c96), uint16_t(0x47e9), uint64_t(0x9ab859e373d875a7));
static BLEUUID rootAssistantCharacteristic(0x21c46f33, 0xe813, 0x4407, 0x86012ad281030052);

static BLEUUID deviceInformationService(uint16_t(0x180A));
static BLEUUID modelNumberCharacteristic(uint16_t(0x2A24));

static int median_of_3(int a, int b, int c)
{
    int the_max = max(max(a, b), c);
    int the_min = min(min(a, b), c);
    // unnecessarily clever code
    int the_median = the_max ^ the_min ^ a ^ b ^ c;
    return (the_median);
}
