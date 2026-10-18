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

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println(F("=== AS7341 readout (Nano) ==="));

  Wire.begin();
  if (!as7341.begin(0x39, &Wire)) {
    Serial.println(F("ERR,AS7341 init selhal - spust identify_spectrometer.ino"));
    while (1) delay(1000);
  }
  as7341.setATIME(100);
  as7341.setASTEP(999);
  as7341.setGain(AS7341_GAIN_16X);
  Serial.println(F("CONF,16x,atime=100,astep=999"));
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
  Serial.print(F("ENDD,")); Serial.println(seq);
  seq++;
  delay(1000);
}
