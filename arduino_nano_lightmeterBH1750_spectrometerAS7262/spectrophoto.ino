#include <Wire.h>
#include <AS726X.h>

AS726X sensor;

const int buttonPin = 2; // Pin connected to the button

void setup() {
  Serial.begin(9600); // Initialize serial communication
  Wire.begin(); // Initialize I2C communication

  if (sensor.begin()) {
    Serial.println("AS7262 sensor initialized successfully!");
  } else {
    Serial.println("AS7262 sensor initialization failed!");
    while (1);
  }

  pinMode(buttonPin, INPUT_PULLUP); // Set button pin as input with internal pull-up resistor

  sensor.disableIndicator(); // Turn off the sensor's indicator LED
  sensor.setIntegrationTime(100); // Set integration time to 100ms

  // Print instructions
  Serial.println("Press the button to take a measurement.");
}

void loop() {
  if (digitalRead(buttonPin) == LOW) {
    takeMeasurement(); // Take a measurement when the button is pressed
    delay(500); // Debounce delay
  }
}

void takeMeasurement() {
  sensor.takeMeasurements(); // Take measurements

  // Read calibrated values for each channel
  float violet = sensor.getCalibratedViolet();
  float blue = sensor.getCalibratedBlue();
  float green = sensor.getCalibratedGreen();
  float yellow = sensor.getCalibratedYellow();
  float orange = sensor.getCalibratedOrange();
  float red = sensor.getCalibratedRed();

  // Print the table heading
  Serial.println("Wavelength (nm);Intensity (µW/cm²)");

  // Print the table rows with wavelengths and intensity values
  Serial.print("450;");
  Serial.println(violet);

  Serial.print("500;");
  Serial.println(blue);

  Serial.print("550;");
  Serial.println(green);

  Serial.print("570;");
  Serial.println(yellow);

  Serial.print("600;");
  Serial.println(orange);

  Serial.print("650;");
  Serial.println(red);
}
