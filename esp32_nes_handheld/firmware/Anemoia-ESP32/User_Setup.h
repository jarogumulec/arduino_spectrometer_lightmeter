// TFT_eSPI konfigurace pro MSP2834 (ILI9341V, 240x320) na ESP32 DevKit.
// Zkopirovat pres Documents/Arduino/libraries/TFT_eSPI/User_Setup.h
// (puvodni soubor predtim zazalohovat jako User_Setup.h.orig).
// Piny podle esp32_nes_handheld/docs/wiring.md.

#define USER_SETUP_INFO "MSP2834 ESP32 DevKit"

#define ILI9341_DRIVER          // ILI9341V podle tabulky prodejce

#define TFT_WIDTH  240
#define TFT_HEIGHT 320

// IPS panel: pokud cervena vychazi azurova (cyan), zmenit na TFT_INVERSION_ON.
// Pokud cervena vychazi modra, zmenit TFT_RGB_ORDER na TFT_BGR.
#define TFT_INVERSION_ON   // overeno 2026-11-12: IPS panel vyzaduje inverzi
#define TFT_RGB_ORDER TFT_BGR   // overeno 2026-11-12

#define TFT_MISO 19
#define TFT_MOSI 23
#define TFT_SCLK 18
#define TFT_CS    5
#define TFT_DC    2
#define TFT_RST   4
// TFT_BL nedefinovan: LED pin modulu je na 3V3, podsviceni trvale zapnute.

#define TOUCH_CS -1             // dotyk se nepouziva

#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4

// Nepajive pole: zacit na 27 MHz, po overeni zkusit 40 MHz (cil Anemoie pro ILI9341).
#define SPI_FREQUENCY  40000000 // Anemoia: ILI9341 max. 40 MHz; pri chybach obrazu 27000000 (overeno)
#define SPI_READ_FREQUENCY  10000000
