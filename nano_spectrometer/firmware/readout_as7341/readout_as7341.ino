/**
 * Spektrometr AS7341 - readout, Arduino Nano
 *
 * Vyzaduje knihovnu Adafruit AS7341 (+ Adafruit BusIO, Adafruit GFX ne).
 * Nahraje se az po identifikaci pres identify_spectrometer.ino.
 *
 * Zapojeni:
 *   VIN -> 5V, GND -> GND, SDA -> A4, SCL -> A5
 *
 * Serial protokol 115200:
 *   DATA,seq,wavelength_nm,count   (Clear = 0, NIR = 910)
 *   ENDD,seq
 */

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_AS7341.h>

Adafruit_AS7341 as7341;
uint32_t seq = 0;

// Auto-ranging: gain zezdola nahoru, index 0 = 0.5x ... 5 = 16x
const uint8_t GAIN_CODES[6] = {AS7341_GAIN_0_5X, AS7341_GAIN_1X, AS7341_GAIN_2X,
                               AS7341_GAIN_4X, AS7341_GAIN_8X, AS7341_GAIN_16X};
const char* const GAIN_LABELS[6] = {"0.5x", "1x", "2x", "4x", "8x", "16x"};
int gain_idx = 5;      // start 16x
uint8_t atime = 100;   // pri trvale saturaci na 0.5x se puluje (100->50->25)

void applyConfig() {
  as7341.setATIME(atime);
  as7341.setASTEP(999);
  as7341.setGain((as7341_gain_t)GAIN_CODES[gain_idx]);
}

void emitConf() {
  Serial.print(F("CONF,"));
  Serial.print(GAIN_LABELS[gain_idx]);
  Serial.print(F(",atime=")); Serial.print(atime);
  Serial.println(F(",astep=999"));
}

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println(F("=== AS7341 readout (Nano) ==="));

  Wire.begin();
  if (!as7341.begin(0x39, &Wire)) {
    Serial.println(F("ERR,AS7341 init selhal - spust identify_spectrometer.ino"));
    while (1) delay(1000);
  }
  applyConfig();
  Serial.println(F("ID,AS7341"));
  emitConf();
}

void loop() {
  // 12 hodnot: [0..5]=F1,F2,F3,F4,Clear,NIR ; [6..11]=F5,F6,F7,F8,Clear,NIR
  uint16_t r[12];
  if (!as7341.readAllChannels(r)) {
    Serial.println(F("ERR,readAllChannels selhal"));
    delay(1000);
    return;
  }

  Serial.print(F("DATA,")); Serial.print(seq); Serial.print(F(",415,")); Serial.println(r[0]);
  Serial.print(F("DATA,")); Serial.print(seq); Serial.print(F(",445,")); Serial.println(r[1]);
  Serial.print(F("DATA,")); Serial.print(seq); Serial.print(F(",480,")); Serial.println(r[2]);
  Serial.print(F("DATA,")); Serial.print(seq); Serial.print(F(",515,")); Serial.println(r[3]);
  Serial.print(F("DATA,")); Serial.print(seq); Serial.print(F(",555,")); Serial.println(r[6]);
  Serial.print(F("DATA,")); Serial.print(seq); Serial.print(F(",590,")); Serial.println(r[7]);
  Serial.print(F("DATA,")); Serial.print(seq); Serial.print(F(",630,")); Serial.println(r[8]);
  Serial.print(F("DATA,")); Serial.print(seq); Serial.print(F(",680,")); Serial.println(r[9]);
  Serial.print(F("DATA,")); Serial.print(seq); Serial.print(F(",0,"));    Serial.println(r[10]);  // Clear
  Serial.print(F("DATA,")); Serial.print(seq); Serial.print(F(",910,")); Serial.println(r[11]);  // NIR

  // Flicker detekce kazdy 5. ramec (vlastni 500 ms sekvenci ve cipu).
  // Navratove hodnoty: 0 = zadny/neznamy flicker, 1 = neznama frekvence,
  // 100 / 120 Hz = rozpoznano. readAllChannels si SMUX obnovi samo.
  if (seq % 5 == 0) {
    uint16_t hz = as7341.detectFlickerHz();
    Serial.print(F("FLICK,")); Serial.print(seq); Serial.print(F(","));
    Serial.println(hz);
    // detectFlickerHz vypina spectralni mereni - obnovit konfiguraci
    as7341.setATIME(atime);
    as7341.setASTEP(999);
    as7341.setGain((as7341_gain_t)GAIN_CODES[gain_idx]);
  }

  Serial.print(F("ENDD,")); Serial.println(seq);
  seq++;

  // Auto-ranging (CONF se posila po ENDD, plati od nasledujiciho ramce):
  // saturace -> gain krok dolu; stale saturovano na 0.5x -> pulovani ATIME.
  // Tmavy ramec (max < 400) -> gain krok nahoru. Hystereza brani oscilaci.
  uint16_t maxc = 0;
  for (uint8_t i = 0; i < 12; i++)
    if (r[i] > maxc) maxc = r[i];
  bool changed = false;
  if (maxc >= 65535) {
    if (gain_idx > 0) { gain_idx--; changed = true; }
    else if (atime > 25) { atime /= 2; changed = true; }
  } else if (maxc < 400 && gain_idx < 5) {
    gain_idx++; changed = true;
  }
  if (changed) {
    applyConfig();
    emitConf();
  }

  delay(1000);
}
