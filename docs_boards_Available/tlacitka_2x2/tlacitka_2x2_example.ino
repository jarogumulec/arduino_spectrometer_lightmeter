#include <Keypad.h>

#define RADKY   2   // pocet radku
#define SLOUPCE 2   // pocet sloupcu

/* Zapojeni pinu */
#define PIN_L1  2
#define PIN_L2  3
#define PIN_R1  4
#define PIN_R2  5

char tlacitka[RADKY][SLOUPCE] = {
  {'1','2'},
  {'3','4'}
};
byte pinyRadky[RADKY]     = {PIN_R1,PIN_R2};
byte pinySloupce[SLOUPCE] = {PIN_L1,PIN_L2};

Keypad keypad = Keypad( makeKeymap(tlacitka), pinyRadky, pinySloupce, RADKY, SLOUPCE );

void setup(){
  Serial.begin(9600);
}

void loop(){
  char tlacitko = keypad.getKey();

  if (tlacitko){
    Serial.print("Stisknute tlacitko: "); Serial.println(tlacitko);
  }
}
