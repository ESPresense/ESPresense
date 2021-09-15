#include <Arduino.h>
#include <NimBLEBeacon.h>
#include <NimBLEDevice.h>

#define ENDIAN_CHANGE_U16(x) ((((x)&0xFF00) >> 8) + (((x)&0xFF) << 8))

#ifdef TX_DEFAULT
static const int defaultTxPower = TX_DEFAULT;
#else
static const int defaultTxPower = -59;
#endif

static const uint16_t beaconUUID = 0xFEAA;
static const uint16_t tileUUID = 0xFEED;
static const uint16_t exposureUUID = 0xFD6F;

static int median_of_3(int a, int b, int c)
{
    int the_max = max(max(a, b), c);
    int the_min = min(min(a, b), c);
    // unnecessarily clever code
    int the_median = the_max ^ the_min ^ a ^ b ^ c;
    return (the_median);
}

static String getProximityUUIDString(BLEBeacon beacon)
{
    std::string serviceData = beacon.getProximityUUID().toString().c_str();
    int serviceDataLength = serviceData.length();
    String returnedString = "";
    int i = serviceDataLength;
    while (i > 0)
    {
        if (serviceData[i - 1] == '-')
        {
            i--;
        }
        char a = serviceData[i - 1];
        char b = serviceData[i - 2];
        returnedString += b;
        returnedString += a;

        i -= 2;
    }

    return returnedString;
}
