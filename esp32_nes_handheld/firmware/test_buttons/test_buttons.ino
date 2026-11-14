// Test tlacitek: 8 digitalnich vstupu (INPUT_PULLUP, stisk = LOW).
// Displej na sirku (DISPLAY_ROTATION): kriz vlevo, A/B vpravo, Sel/Start dole;
// stisknute pole zezelena.
// Pod kazdym polem pocet stisku; zakmity spinace se projevi skokem o vic nez 1.
// Dole bitova maska v poradi Anemoie (gpioRead): A B Sel Start U D L R.
// Piny podle esp32_nes_handheld/docs/wiring.md (premapovano 2026-11-14 podle
// rozlozeni v krabicce); SEL docasne GPIO12, cilove GPIO16.
// Pouziva TFT_eSPI s User_Setup.h
// z firmware/test_display.

#include <TFT_eSPI.h>

// 3 = na sirku, slot SD nahore, A/B vpravo (overeno 2026-11-14; 1 je vzhuru nohama).
const uint8_t DISPLAY_ROTATION = 3;

struct Btn { const char* name; int pin; int x, y; };

Btn btns[] = {
  {"A",      14, 265,  90},   // fyzicky drive "B"
  {"B",      13, 205, 120},   // fyzicky drive "START"
  {"SEL",    12, 150, 180},
  {"START",  27, 210, 180},   // fyzicky drive "A"
  {"UP",     32,  60,  70},
  {"DOWN",   33,  60, 150},
  {"LEFT",   25,   5, 110},
  {"RIGHT",  26, 115, 110},
};
const int N = sizeof(btns) / sizeof(btns[0]);
const int W = 50, H = 34;

TFT_eSPI tft;
bool     state[N];
uint32_t count[N];

void drawBtn(int i) {
  uint16_t bg = state[i] ? TFT_GREEN : TFT_DARKGREY;
  uint16_t fg = state[i] ? TFT_BLACK : TFT_WHITE;
  tft.fillRoundRect(btns[i].x, btns[i].y, W, H, 4, bg);
  tft.setTextColor(fg, bg);
  tft.setTextDatum(MC_DATUM);
  tft.drawString(btns[i].name, btns[i].x + W / 2, btns[i].y + H / 2, 2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextDatum(TC_DATUM);
  tft.setTextPadding(W);
  tft.drawNumber(count[i], btns[i].x + W / 2, btns[i].y + H + 2, 1);
  tft.setTextPadding(0);
}

void drawMask() {
  uint8_t m = 0;
  for (int i = 0; i < N; i++) if (state[i]) m |= 1 << i;
  char buf[32];
  snprintf(buf, sizeof(buf), "mask %c%c%c%c%c%c%c%c  0x%02X",
           m & 1 ? '1' : '0', m & 2 ? '1' : '0', m & 4 ? '1' : '0', m & 8 ? '1' : '0',
           m & 16 ? '1' : '0', m & 32 ? '1' : '0', m & 64 ? '1' : '0', m & 128 ? '1' : '0', m);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.setTextDatum(TL_DATUM);
  tft.drawString(buf, 10, 215, 2);
}

void setup() {
  pinMode(17, OUTPUT);           // SD_CS v HIGH, SD neposloucha
  digitalWrite(17, HIGH);
  Serial.begin(115200);

  for (int i = 0; i < N; i++) pinMode(btns[i].pin, INPUT_PULLUP);

  tft.init();
  tft.setRotation(DISPLAY_ROTATION);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString("TEST TLACITEK", 10, 5, 4);
  tft.drawString("A+B soucasne musi svitit obe", 10, 35, 2);
  for (int i = 0; i < N; i++) drawBtn(i);
  drawMask();
}

void loop() {
  bool changed = false;
  for (int i = 0; i < N; i++) {
    bool s = digitalRead(btns[i].pin) == LOW;
    if (s != state[i]) {
      state[i] = s;
      if (s) count[i]++;
      drawBtn(i);
      changed = true;
      Serial.printf("%s %s (GPIO%d) n=%lu\n", btns[i].name, s ? "stisk" : "uvolneno",
                    btns[i].pin, count[i]);
    }
  }
  if (changed) drawMask();
  delay(1);   // bez debounce zamerne: zakmity maji byt videt v poctu
}
