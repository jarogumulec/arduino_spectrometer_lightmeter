// Měření intenzity světla BH1750


// připojení potřebných knihoven
#include <Wire.h>
#include <BH1750.h> // Nástroje → Spravovat knihovny (zkratka Ctrl+Shift+I) → Vyhledat „BH1750" (by „Christopher Laws”), „1.3.0”.          
// inicializace senzoru BH1750 z knihovny
BH1750 luxSenzor;

void setup() {
  // komunikace po sériové lince rychlostí 9600 baud
  Serial.begin(9600);

  // Odkomentuj nasledujici radek pro ESP8266, ESP32 ap.
  //Wire.begin(D4, D3);
  
  // zapnutí komunikace se senzorem BH1750
  luxSenzor.begin();
  // pauza před zahájením měření
  delay(200);
}

void loop() {
  // vytvoření proměnné pro uložení naměřených údajů
  // a načtení aktuální intenzity světla
  uint16_t lux = luxSenzor.readLightLevel();
  // vytištění výsledku po sériové lince
  Serial.print("Intenzita svetla: ");
  Serial.print(lux);
  Serial.println(" luxu.");
  // pauza po dobu 1 vteřiny před dalším měřením
  delay(1000);
}
