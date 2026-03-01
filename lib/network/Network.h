#ifdef ESP8266
  #include <ESP8266WiFi.h>
#else // ESP32
  #include <WiFi.h>
  #include <ETH.h>
  // Ensure W5500 support is defined for SPI Ethernet
  #ifndef CONFIG_ETH_SPI_ETHERNET_W5500
    #define CONFIG_ETH_SPI_ETHERNET_W5500 1
  #endif
  // Define ETH_PHY_W5500 if not already defined
  #ifndef ETH_PHY_W5500
    // Based on ESP32 Arduino ETH.h enum values
    #define ETH_PHY_W5500 7
  #endif
#endif

#ifndef Network_h
#define Network_h

class NetworkManager
{
public:
  IPAddress localIP();
  IPAddress dnsIP();
  static const char *getHostname();
  static void setHostname(const char *hostname);
  static void hostname(const String &hostname);
  bool isOnline();
  bool connect(int ethernetType, int wait_seconds, const char *hostName);

private:
  bool initEthernet(int ethernetType);
};

#define CONFIG_NUM_ETH_TYPES        15

#define CONFIG_ETH_NONE             0
#define CONFIG_ETH_WT32_ETH01       1
#define CONFIG_ETH_ESP32_POE        2
#define CONFIG_ETH_WESP32           3
#define CONFIG_ETH_QUINLED          4
#define CONFIG_ETH_TWILIGHTLORD     5
#define CONFIG_ETH_ESP32DEUX        6
#define CONFIG_ETH_ETHERNET_KIT_VE  7
#define CONFIG_ETH_LILYGO           8
#define CONFIG_ETH_GLINET_S10_V21   9
#define CONFIG_ETH_EST_POE_32       10
#define CONFIG_ETH_LILYGO_LITE_RTL  11
#define CONFIG_ETH_ESP32_POE_A1     12
#define CONFIG_ETH_WESP32_RTL8201   13
#define CONFIG_ETH_WAVESHARE_S3     14

// For ESP32, the remaining five pins are at least somewhat configurable.
// eth_address  is in range [0..31], indicates which PHY (MAC?) address should be allocated to the interface
// eth_power    is an output GPIO pin used to enable/disable the ethernet port (and/or external oscillator)
// eth_mdc      is an output GPIO pin used to provide the clock for the management data
// eth_mdio     is an input/output GPIO pin used to transfer management data
// eth_type     is the physical ethernet module's type (ETH_PHY_LAN8720, ETH_PHY_TLK110)
// eth_clk_mode defines the GPIO pin and GPIO mode for the clock signal
//              However, there are really only four configurable options on ESP32:
//              ETH_CLOCK_GPIO0_IN    == External oscillator, clock input  via GPIO0
//              ETH_CLOCK_GPIO0_OUT   == ESP32 provides 50MHz clock output via GPIO0
//              ETH_CLOCK_GPIO16_OUT  == ESP32 provides 50MHz clock output via GPIO16
//              ETH_CLOCK_GPIO17_OUT  == ESP32 provides 50MHz clock output via GPIO17
typedef struct EthernetSettings {
  uint8_t        eth_address;
  int            eth_power;
  int            eth_mdc;
  int            eth_mdio;
  eth_phy_type_t eth_type;
  eth_clock_mode_t eth_clk_mode;
  bool           use_spi;      // true for SPI Ethernet (W5500), false for RMII
  int            spi_miso;     // SPI MISO pin
  int            spi_mosi;     // SPI MOSI pin
  int            spi_sclk;     // SPI SCLK pin
  int            spi_cs;       // SPI CS pin
  int            spi_int;      // SPI INT pin (optional, -1 if not used)
} ethernet_settings;

const ethernet_settings ethernetBoards[] = {
  // None
  {
  },

  // WT32-EHT01
  {
    1,                 // eth_address,
    16,                // eth_power,
    23,                // eth_mdc,
    18,                // eth_mdio,
    ETH_PHY_LAN8720,   // eth_type,
    ETH_CLOCK_GPIO0_IN,// eth_clk_mode
    false              // use_spi
  },

  // ESP32-POE
  {
    0,                  // eth_address,
    12,                  // eth_power,
    23,                  // eth_mdc,
    18,                  // eth_mdio,
    ETH_PHY_LAN8720,     // eth_type,
    ETH_CLOCK_GPIO17_OUT,// eth_clk_mode
    false                // use_spi
  },

   // WESP32
  {
    0,			              // eth_address,
    -1,			              // eth_power,
    16,			              // eth_mdc,
    17,			              // eth_mdio,
    ETH_PHY_LAN8720,      // eth_type,
    ETH_CLOCK_GPIO0_IN,	  // eth_clk_mode
    false                 // use_spi
  },

  // QuinLed-ESP32-Ethernet
  {
    0,			              // eth_address,
    5,			              // eth_power,
    23,			              // eth_mdc,
    18,			              // eth_mdio,
    ETH_PHY_LAN8720,      // eth_type,
    ETH_CLOCK_GPIO17_OUT,	// eth_clk_mode
    false                 // use_spi
  },

  // TwilightLord-ESP32 Ethernet Shield
  {
    0,			              // eth_address,
    5,			              // eth_power,
    23,			              // eth_mdc,
    18,			              // eth_mdio,
    ETH_PHY_LAN8720,      // eth_type,
    ETH_CLOCK_GPIO17_OUT,	// eth_clk_mode
    false                 // use_spi
  },

  // ESP3DEUXQuattro
  {
    1,                    // eth_address,
    -1,                   // eth_power,
    23,                   // eth_mdc,
    18,                   // eth_mdio,
    ETH_PHY_LAN8720,      // eth_type,
    ETH_CLOCK_GPIO17_OUT, // eth_clk_mode
    false                 // use_spi
  },

  // ESP32-ETHERNET-KIT-VE
  {
    0,                    // eth_address,
    5,                    // eth_power,
    23,                   // eth_mdc,
    18,                   // eth_mdio,
    ETH_PHY_IP101,        // eth_type,
    ETH_CLOCK_GPIO0_IN,   // eth_clk_mode
    false                 // use_spi
  },

  // LilyGO-T-ETH-POE
  {
    0,                    // eth_address,
    -1,                   // eth_power,
    23,                   // eth_mdc,
    18,                   // eth_mdio,
    ETH_PHY_LAN8720,      // eth_type,
    ETH_CLOCK_GPIO17_OUT, // eth_clk_mode
    false                 // use_spi
  },
  
  // GL-inet GL-S10 v2.1 Ethernet
  {
    1,                    // eth_address,
    5,                    // eth_power,
    23,                   // eth_mdc,
    18,                   // eth_mdio,
    ETH_PHY_IP101,        // eth_type,
    ETH_CLOCK_GPIO0_IN,   // eth_clk_mode
    false                 // use_spi
  },

  // EST-PoE-32
  {
    0,                  // eth_address,
    12,                  // eth_power,
    23,                  // eth_mdc,
    18,                  // eth_mdio,
    ETH_PHY_LAN8720,     // eth_type,
    ETH_CLOCK_GPIO17_OUT,// eth_clk_mode
    false                // use_spi
  },

  // LilyGO-T-ETH-Lite
  {
    0,                    // eth_address,
    12,                   // eth_power,
    23,                   // eth_mdc,
    18,                   // eth_mdio,
    ETH_PHY_RTL8201,      // eth_type,
    ETH_CLOCK_GPIO0_IN,   // eth_clk_mode
    false                 // use_spi
  },

  // ESP32-POE_A1
  {
    1,                  // eth_address,
   12,                  // eth_power,
   23,                  // eth_mdc,
   18,                  // eth_mdio,
   ETH_PHY_LAN8720,     // eth_type,
   ETH_CLOCK_GPIO17_OUT,// eth_clk_mode
   false                // use_spi
  },

  // WESP32 Rev7+ (RTL8201)
  {
    0,                  // eth_address,
    -1,                 // eth_power,
    16,                 // eth_mdc,
    17,                 // eth_mdio,
    ETH_PHY_RTL8201,    // eth_type,
    ETH_CLOCK_GPIO0_IN, // eth_clk_mode
    false               // use_spi
  },

  // Waveshare ESP32-S3-ETH (W5500 SPI)
  {
    0,                  // eth_address (not used for SPI)
    9,                  // eth_power (RST pin)
    -1,                 // eth_mdc (not used for SPI)
    -1,                 // eth_mdio (not used for SPI)
    ETH_PHY_W5500,      // eth_type
    ETH_CLOCK_GPIO0_IN, // eth_clk_mode (not used for SPI)
    true,               // use_spi
    12,                 // spi_miso
    11,                 // spi_mosi
    13,                 // spi_sclk
    14,                 // spi_cs
    10                  // spi_int
  }
};

extern NetworkManager Network;

#endif
