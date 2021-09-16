#include <Arduino.h>
#include <NimBLEBeacon.h>
#include <NimBLEDevice.h>

#define ENDIAN_CHANGE_U16(x) ((((x)&0xFF00) >> 8) + (((x)&0xFF) << 8))

#ifdef TX_DEFAULT
static const int defaultTxPower = TX_DEFAULT;
#else
static const int defaultTxPower = -59;
#endif

static BLEUUID eddystoneUUID((uint16_t)0xFEAA);
static BLEUUID tileUUID((uint16_t)0xFEED);
static BLEUUID exposureUUID((uint16_t)0xFD6F);
static BLEUUID sonosUUID((uint16_t)0xFE07);

static int median_of_3(int a, int b, int c)
{
    int the_max = max(max(a, b), c);
    int the_min = min(min(a, b), c);
    // unnecessarily clever code
    int the_median = the_max ^ the_min ^ a ^ b ^ c;
    return (the_median);
}
