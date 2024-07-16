#include <EEPROM.h>

const int photoResistorPin = A0; // Pin connected to the photoresistor
const int buttonPin = 2;         // Pin connected to the button
const int ledPin = 13;           // Pin connected to the LED (usually the built-in LED on the Arduino Nano)
int sensorValue = 0;             // Variable to store the value from the photoresistor
int address = 0;                 // EEPROM address
bool measurementsStarted = false;

void setup() {
  Serial.begin(9600); // Initialize serial communication at 9600 baud
  pinMode(buttonPin, INPUT_PULLUP); // Set button pin as input with internal pull-up resistor
  pinMode(ledPin, OUTPUT);          // Set LED pin as output
  
  // Read the last used address from EEPROM
  EEPROM.get(0, address);

  // Wait for 5 seconds to check for serial input
  Serial.println("Press 'r' to retrieve data, 'c' to clear data, or press the button to start measurements within 5 seconds...");
  unsigned long startTime = millis();
  while (millis() - startTime < 5000) {
    if (Serial.available() > 0) {
      char input = Serial.read();
      if (input == 'r') {
        retrieveData(); // If 'r' is pressed, retrieve data
        return;
      } else if (input == 'c') {
        clearData(); // If 'c' is pressed, clear data
        return;
      }
    } else if (digitalRead(buttonPin) == LOW) {
      startMeasurements(); // If button is pressed, start measurements
      return;
    }
  }
  
  Serial.println("No input received. Waiting for button press to start measurements...");
}

void loop() {
  if (measurementsStarted) {
    logData(); // Log data
    blinkLED(1, 200); // Blink LED once after each measurement
    delay(5000); // Wait for 5 seconds
  } else if (digitalRead(buttonPin) == LOW) {
    startMeasurements(); // Start measurements if button is pressed
  }
}

void logData() {
  sensorValue = analogRead(photoResistorPin); // Read the analog value from the photoresistor

  // Store the value in EEPROM
  if (address < EEPROM.length() - sizeof(sensorValue)) {
    EEPROM.put(address, sensorValue);
    address += sizeof(sensorValue);
    
    // Store the updated address back to the start of the EEPROM
    EEPROM.put(0, address);
    
    Serial.print("Logged value: ");
    Serial.println(sensorValue);
  } else {
    Serial.println("EEPROM full!");
  }
}

void retrieveData() {
  Serial.println("Retrieving data...");
  for (int i = sizeof(address); i < address; i += sizeof(sensorValue)) {
    int value;
    EEPROM.get(i, value);
    Serial.print("Value at ");
    Serial.print(i);
    Serial.print(": ");
    Serial.println(value);
  }
}

void clearData() {
  Serial.println("Clearing all stored data...");
  for (int i = 0; i < EEPROM.length(); i++) {
    EEPROM.write(i, 0);
  }
  address = 0;
  EEPROM.put(0, address); // Reset the address to 0 in EEPROM
  Serial.println("All data cleared.");
}

void startMeasurements() {
  measurementsStarted = true;
  Serial.println("Starting measurements...");
  blinkLED(3, 100); // Blink LED fast 3 times to indicate start
}

void blinkLED(int times, int duration) {
  for (int i = 0; i < times; i++) {
    digitalWrite(ledPin, HIGH);
    delay(duration);
    digitalWrite(ledPin, LOW);
    delay(duration);
  }
}
