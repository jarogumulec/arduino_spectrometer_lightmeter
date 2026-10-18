#include <EEPROM.h>

const int photoResistorPin = A0; // Pin connected to the photoresistor
const int buttonPin = 2;         // Pin connected to the button
const int ledPin = 13;           // Pin connected to the LED (usually the built-in LED on the Arduino Nano)
int sensorValue = 0;             // Variable to store the value from the photoresistor
int address = 0;                 // EEPROM address
bool measurementsStarted = false;
unsigned long startTime = 0;     // Time when measurements start
unsigned long lastMeasurementTime = 0; // Time when the last measurement was logged

void setup() {
  Serial.begin(9600); // Initialize serial communication at 9600 baud
  pinMode(buttonPin, INPUT_PULLUP); // Set button pin as input with internal pull-up resistor
  pinMode(ledPin, OUTPUT);          // Set LED pin as output
  
  // Read the last used address from EEPROM
  EEPROM.get(0, address);

  // Wait for 5 seconds to check for serial input
  Serial.println("Press 'r' to retrieve data, or press the button to start measurements and clear data within 5 seconds...");
  unsigned long waitStartTime = millis();
  while (millis() - waitStartTime < 5000) {
    if (Serial.available() > 0) {
      char input = Serial.read();
      if (input == 'r') {
        retrieveData(); // If 'r' is pressed, retrieve data
        return;
      }
    } else if (digitalRead(buttonPin) == LOW) {
      clearData(); // Clear data when the button is pressed
      startMeasurements(); // Start measurements when the button is pressed
      return;
    }
  }
  
  Serial.println("No input received. Waiting for button press to start measurements...");
}

void loop() {
  if (measurementsStarted) {
    // Check if 5 minutes have passed since the last measurement
    if (millis() - lastMeasurementTime >= 300000) {
      recordAverageMeasurement();
      blinkLED(1, 200); // Blink LED once after each measurement
      lastMeasurementTime = millis();
    }
  } else if (digitalRead(buttonPin) == LOW) {
    clearData(); // Clear data when the button is pressed
    startMeasurements(); // Start measurements when the button is pressed
  }
}

void logData(int timeCode, int averageValue) {
  if (address < EEPROM.length() - sizeof(int) * 2) {
    EEPROM.put(address, timeCode);
    address += sizeof(int);
    EEPROM.put(address, averageValue);
    address += sizeof(int);
    
    // Store the updated address back to the start of the EEPROM
    EEPROM.put(0, address);
    
    Serial.print("Logged value: ");
    Serial.print("Timecode: ");
    Serial.print(timeCode);
    Serial.print(", Average value: ");
    Serial.println(averageValue);
  } else {
    Serial.println("EEPROM full!");
  }
}

void recordAverageMeasurement() {
  int sum = 0;
  int count = 0;
  unsigned long measurementStartTime = millis();
  
  while (millis() - measurementStartTime < 300000) { // 5 minutes
    sensorValue = analogRead(photoResistorPin);
    sum += sensorValue;
    count++;
    delay(1000); // Take a reading every second
  }
  
  int averageValue = sum / count;
  int timeCode = (millis() - startTime) / 60000; // Time in minutes since start
  logData(timeCode, averageValue);
}

void retrieveData() {
  Serial.println("Retrieving data...");
  for (int i = sizeof(address); i < address; i += sizeof(int) * 2) {
    int timeCode;
    int value;
    EEPROM.get(i, timeCode);
    EEPROM.get(i + sizeof(int), value);
    Serial.print("Timecode: ");
    Serial.print(timeCode);
    Serial.print(", Value: ");
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
  startTime = millis(); // Record the start time
  lastMeasurementTime = millis(); // Initialize the last measurement time
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
