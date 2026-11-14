// Test microSD na slotu displeje MSP2834 (sdilena sbernice VSPI s LCD).
// Postupne zkousi frekvence SD, pri prvni funkcni vypise typ a velikost
// karty, soubory v korenu a rychlost cteni prvniho .nes. Vystup jen na
// displej (seriova linka na macOS nespolehliva). User_Setup.h stejny jako
// test_display.

#include <TFT_eSPI.h>
#include <SPI.h>
#include <SD.h>

const int SD_CS_PIN = 17;
const uint8_t DISPLAY_ROTATION = 3;  // na sirku, slot SD nahore
const uint32_t FREQS[] = {1000000, 4000000, 10000000, 20000000};

TFT_eSPI tft;
int y = 0;

void line(const char* s, uint16_t col = TFT_WHITE) {
  tft.setTextColor(col, TFT_BLACK);
  tft.drawString(s, 4, y, 2);
  y += 16;
}

bool tryFreq(uint32_t f) {
  SD.end();
  return SD.begin(SD_CS_PIN, SPI, f);
}

void setup() {
  pinMode(SD_CS_PIN, OUTPUT);
  digitalWrite(SD_CS_PIN, HIGH);

  tft.init();
  tft.setRotation(DISPLAY_ROTATION);
  tft.fillScreen(TFT_BLACK);
  char buf[64];

  // Max. funkcni frekvence ze seznamu
  uint32_t okFreq = 0;
  for (uint32_t f : FREQS) {
    bool ok = tryFreq(f);
    snprintf(buf, sizeof(buf), "SD %2lu MHz: %s", f / 1000000, ok ? "OK" : "chyba");
    line(buf, ok ? TFT_GREEN : TFT_RED);
    if (ok) okFreq = f;
  }
  if (!okFreq) {
    line("Karta nenalezena (FAT32? zasunuta?)", TFT_RED);
    return;
  }
  tryFreq(okFreq);

  const char* types[] = {"zadna", "MMC", "SD", "SDHC", "?"};
  uint8_t t = SD.cardType();
  snprintf(buf, sizeof(buf), "Typ %s, %llu MB, pouzito %lu MHz",
           types[t > 4 ? 4 : t], SD.cardSize() / (1024ULL * 1024ULL), okFreq / 1000000);
  line(buf, TFT_CYAN);

  // Soubory v korenu
  String firstNes;
  File root = SD.open("/");
  int n = 0;
  for (File f = root.openNextFile(); f; f = root.openNextFile()) {
    String name = f.name();
    if (n < 8) {
      snprintf(buf, sizeof(buf), "%s %s %lu B", f.isDirectory() ? "[D]" : "   ",
               name.c_str(), (unsigned long)f.size());
      line(buf);
    }
    n++;
    if (firstNes.isEmpty() && !f.isDirectory() && name.endsWith(".nes"))
      firstNes = "/" + name;
    f.close();
  }
  root.close();
  snprintf(buf, sizeof(buf), "Polozek v korenu: %d", n);
  line(buf, TFT_CYAN);

  // Rychlost cteni
  if (!firstNes.isEmpty()) {
    File f = SD.open(firstNes);
    static uint8_t blk[4096];
    uint32_t total = 0, t0 = millis();
    while (int r = f.read(blk, sizeof(blk))) total += r;
    uint32_t dt = millis() - t0;
    f.close();
    snprintf(buf, sizeof(buf), "Cteni %lu B za %lu ms (%lu kB/s)", total, dt,
             dt ? total / dt : 0);
    line(buf, TFT_YELLOW);
  }

  // Displej po pristupu na SD stale funguje (sdilena sbernice)
  tft.fillRect(250, 200, 60, 30, TFT_GREEN);
  line("Zeleny ctverec vpravo dole = LCD OK", TFT_GREEN);
}

void loop() {}
