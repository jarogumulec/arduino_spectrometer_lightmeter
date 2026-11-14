// Test displeje MSP2834 (ILI9341V) pres TFT_eSPI.
// Ocekavany obraz (na vysku): pruhy shora CERVENA, ZELENA, MODRA, BILA,
// pod kazdym nazev barvy. Nesouhlasi-li barva s napisem, viz User_Setup.h.
// Seriova linka 115200: ID radice (0xD3) a doba vykresleni celeho snimku.
// SD karta pri testu nema byt ve slotu; SD_CS (GPIO17) drzen v HIGH.

#include <TFT_eSPI.h>

const int SD_CS_PIN = 17;

TFT_eSPI tft;

void drawTest();

void band(int y, int h, uint16_t col, const char* label, uint16_t txt) {
  tft.fillRect(0, y, tft.width(), h, col);
  tft.setTextColor(txt, col);
  tft.drawString(label, 10, y + 10, 4);
}

void setup() {
  pinMode(SD_CS_PIN, OUTPUT);
  digitalWrite(SD_CS_PIN, HIGH);   // SD neposloucha na sdilene sbernici

  Serial.begin(115200);
  delay(300);

  tft.init();
  tft.setRotation(0);

  // ID radice: ILI9341 vraci 0x93 0x41 v bytech 2 a 3. Pres prevodnik urovni
  // na modulu muze cteni selhat (0x00/0xFF) i pri funkcnim displeji.
  Serial.printf("RDID4 (0xD3): %02X %02X %02X\n",
                tft.readcommand8(0xD3, 1), tft.readcommand8(0xD3, 2),
                tft.readcommand8(0xD3, 3));

  drawTest();
}

void drawTest() {
  int h = tft.height() / 5;
  band(0 * h, h, TFT_RED,   "CERVENA", TFT_WHITE);
  band(1 * h, h, TFT_GREEN, "ZELENA",  TFT_BLACK);
  band(2 * h, h, TFT_BLUE,  "MODRA",   TFT_WHITE);
  band(3 * h, h, TFT_WHITE, "BILA",    TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString("240x320 ILI9341", 10, 4 * h + 10, 2);
  tft.drawRect(0, 0, tft.width(), tft.height(), TFT_YELLOW);  // okraj: oriznuti?
}

void loop() {
  // Rychlost: cely snimek 240x320 jednou barvou; mereno pro SPI_FREQUENCY.
  static uint32_t n = 0;
  if (n++ < 3) {
    delay(2000);
    uint32_t t0 = micros();
    tft.fillScreen(TFT_BLACK);
    uint32_t dt = micros() - t0;
    Serial.printf("fillScreen: %lu us (%.1f snimku/s)\n", dt, 1e6 / dt);
    drawTest();  // obnovit testovaci obraz
  }
}
