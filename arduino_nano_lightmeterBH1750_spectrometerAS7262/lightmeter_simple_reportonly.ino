const int photoResistorPin = A0; // Pin connected to the photoresistor
int sensorValue = 0;             // Variable to store the value from the photoresistor
float luxValue = 0.0;            // Variable to store the calculated lux value

void setup() {
  Serial.begin(9600); // Initialize serial communication at 9600 baud
}

void loop() {
  sensorValue = analogRead(photoResistorPin); // Read the analog value from the photoresistor
  luxValue = 36.93 * sensorValue + 225.7; // Calculate the lux value using the given equation

  Serial.print("Photoresistor value: ");
  Serial.print(sensorValue); // Print the raw value to the serial monitor
  Serial.print("\tCalibrated lux value: ");
  Serial.println(luxValue); // Print the calibrated lux value to the serial monitor

  delay(1000); // Wait for 1 second
}
