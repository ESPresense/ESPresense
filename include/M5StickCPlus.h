#define ST7789_2_DRIVER

#define TFT_WIDTH  135
#define TFT_HEIGHT 240
#define TFT_FONT 2

#define TFT_MOSI 15
#define TFT_SCLK 13
#define TFT_CS   5   // Chip select line for TFT display on Shield
#define TFT_DC   23  // Data/command line for TFT on Shield
#define TFT_RST  18  // Reset line for TFT is handled by seesaw!
#define TOUCH_CS -1

#define LOAD_GLCD   // Font 1. Original Adafruit 8 pixel font needs ~1820 bytes
                    // in FLASH
#define LOAD_FONT2  // Font 2. Small 16 pixel high font, needs ~3534 bytes in
                    // FLASH, 96 characters

#define SPI_FREQUENCY 27000000
#define SPI_READ_FREQUENCY 20000000
#define SPI_TOUCH_FREQUENCY 2500000

