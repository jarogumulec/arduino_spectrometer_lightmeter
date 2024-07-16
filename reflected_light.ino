#include <Wire.h>
#include <AS726X.h>
#include <math.h>

AS726X sensor;

const int buttonPin = 2;

// D65 white point
const float Xn = 95.047;
const float Yn = 100.000;
const float Zn = 108.883;

// Known Lab values for ColorChecker patches
const float knownLab[6][3] = {
  {29, 14, -50},   // Blue
  {55, -38, 31},   // Green
  {42, 53, 28},    // Red
  {82, 4, 80},     // Yellow
  {52, 50, -15},   // Magenta
  {51, -29, -29}   // Cyan
};

// Variables to store calibration XYZ values for known patches
float refXYZ[6][3];

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

  sensor.disableIndicator(); // Turn off the sensor's indicator LED by default
  sensor.setIntegrationTime(100); // Set integration time to 100ms

  // Convert known Lab values to XYZ values
  convertLabToXYZ();

  // Print instructions
  Serial.println("Press the button to calibrate with ColorChecker patches.");
  Serial.println("Measure the following patches in this order: Blue, Green, Red, Yellow, Magenta, Cyan.");
  calibrateSensor();
  Serial.println("Calibration complete. Press the button to take a measurement with LED illumination.");
}

void loop() {
  if (digitalRead(buttonPin) == LOW) {
    takeMeasurementWithLED(); // Take a measurement with the internal LED illumination
    delay(500); // Debounce delay
  }
}

void convertLabToXYZ() {
  for (int i = 0; i < 6; i++) {
    float L = knownLab[i][0];
    float a = knownLab[i][1];
    float b = knownLab[i][2];

    float Y = (L + 16.0) / 116.0;
    float X = a / 500.0 + Y;
    float Z = Y - b / 200.0;

    if (pow(Y, 3) > 0.008856) Y = pow(Y, 3);
    else Y = (Y - 16.0 / 116.0) / 7.787;

    if (pow(X, 3) > 0.008856) X = pow(X, 3);
    else X = (X - 16.0 / 116.0) / 7.787;

    if (pow(Z, 3) > 0.008856) Z = pow(Z, 3);
    else Z = (Z - 16.0 / 116.0) / 7.787;

    refXYZ[i][0] = X * Xn;
    refXYZ[i][1] = Y * Yn;
    refXYZ[i][2] = Z * Zn;
  }

  Serial.println("Converted XYZ values for known Lab patches:");
  for (int i = 0; i < 6; i++) {
    Serial.print("Patch ");
    Serial.print(i + 1);
    Serial.print(": X: "); Serial.print(refXYZ[i][0]);
    Serial.print(" Y: "); Serial.print(refXYZ[i][1]);
    Serial.print(" Z: "); Serial.println(refXYZ[i][2]);
  }
}

void calibrateSensor() {
  for (int i = 0; i < 6; i++) {
    Serial.print("Measure patch ");
    Serial.println(i + 1);
    while (digitalRead(buttonPin) != LOW) {
      delay(10);
    }
    sensor.enableBulb(); // Turn on the internal LED
    delay(100); // Wait a short time to ensure the LED is fully on

    sensor.takeMeasurements(); // Take measurements

    sensor.disableBulb(); // Turn off the internal LED

    // Calculate XYZ values
    refXYZ[i][0] = 0.033 * sensor.getCalibratedViolet() + 0.323 * sensor.getCalibratedBlue() + 0.878 * sensor.getCalibratedGreen() + 0.849 * sensor.getCalibratedYellow() + 0.63 * sensor.getCalibratedOrange() + 0.176 * sensor.getCalibratedRed();
    refXYZ[i][1] = 0.001 * sensor.getCalibratedViolet() + 0.725 * sensor.getCalibratedBlue() + 0.92 * sensor.getCalibratedGreen() + 0.994 * sensor.getCalibratedYellow() + 0.618 * sensor.getCalibratedOrange() + 0.04 * sensor.getCalibratedRed();
    refXYZ[i][2] = 0.358 * sensor.getCalibratedViolet() + 0.002 * sensor.getCalibratedBlue() + 0.008 * sensor.getCalibratedGreen() + 0.002 * sensor.getCalibratedYellow() + 0.002 * sensor.getCalibratedOrange();
    
    while (digitalRead(buttonPin) == LOW) {
      delay(10);
    }
    delay(500); // Debounce delay
  }

  Serial.println("Calibration XYZ values:");
  for (int i = 0; i < 6; i++) {
    Serial.print("Patch ");
    Serial.print(i + 1);
    Serial.print(": X: "); Serial.print(refXYZ[i][0]);
    Serial.print(" Y: "); Serial.print(refXYZ[i][1]);
    Serial.print(" Z: "); Serial.println(refXYZ[i][2]);
  }
}

void takeMeasurementWithLED() {
  sensor.enableBulb(); // Turn on the internal LED
  delay(100); // Wait a short time to ensure the LED is fully on

  sensor.takeMeasurements(); // Take measurements

  sensor.disableBulb(); // Turn off the internal LED

  // Read calibrated values for each channel
  float violet = sensor.getCalibratedViolet();
  float blue = sensor.getCalibratedBlue();
  float green = sensor.getCalibratedGreen();
  float yellow = sensor.getCalibratedYellow();
  float orange = sensor.getCalibratedOrange();
  float red = sensor.getCalibratedRed();

  // Print the table rows with wavelengths and intensity values
  Serial.print("450\t\t");
  Serial.println(violet);
  Serial.print("500\t\t");
  Serial.println(blue);
  Serial.print("550\t\t");
  Serial.println(green);
  Serial.print("570\t\t");
  Serial.println(yellow);
  Serial.print("600\t\t");
  Serial.println(orange);
  Serial.print("650\t\t");
  Serial.println(red);

  // Calculate XYZ values
  float X = 0.033 * violet + 0.323 * blue + 0.878 * green + 0.849 * yellow + 0.63 * orange + 0.176 * red;
  float Y = 0.001 * violet + 0.725 * blue + 0.92 * green + 0.994 * yellow + 0.618 * orange + 0.04 * red;
  float Z = 0.358 * violet + 0.002 * blue + 0.008 * green + 0.002 * yellow + 0.002 * orange;

  // Normalize by the calibration patches
  float normX = X / refXYZ[3][0] * Xn; // Using yellow patch (patch 3) for normalization
  float normY = Y / refXYZ[3][1] * Yn; // Using yellow patch (patch 3) for normalization
  float normZ = Z / refXYZ[3][2] * Zn; // Using yellow
