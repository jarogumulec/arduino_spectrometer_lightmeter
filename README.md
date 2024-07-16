# Arduino Codes for Light Measurement

## Prerequisites

- **Hardware**:
  - Photodiode
  - 10k Resistor
  - AS7262 Visible Spectrometer
  - Arduino Nano (ATMEGA 328P old bootloader)
  - Push Button

- **Software**:
  - Arduino IDE (version 2.3.2 on macOS)
  - Libraries:
    - SparkFun AS726X
    - Wire (for I2C connection)

## Light Meter

A light meter based on a photoresistor that records light intensity every 5 minutes and stores the data to EEPROM. To start recording, press the button (D2 + GND), which also deletes all previous measurements. Once connected to a PC, you can retrieve data by sending `r` via the Serial Monitor or delete data by sending `c`.

### Connection

- **Button**: D2 to GND (press to start measurement, LED blinks 3 times)
- **Photoresistor**: 
  - One end to 5V
  - Other end to both A0 and through a 10k ohm resistor to GND

### Operation

1. Press the button (D2 to GND) to delete all previous measurements and start new measurements every 5 minutes.
2. The LED will blink 3 times to indicate the start of measurements.
3. Every 5 minutes, the light intensity is measured and stored in EEPROM.
4. After measurements, the LED blinks once.
5. Connect to a PC and open the Serial Monitor:
   - Send `r` to retrieve all stored data.
   - Send `c` to clear all stored data.

## Spectrophotometer

An Arduino module using the SparkFun AS7262 Spectral Sensor Breakout for visible light. It reports the calibrated light intensity at 6 wavelengths (450, 500, 550, 570, 600, 650 nm) in units of µW/cm². The values are displayed when the button (D2 + GND) is pressed.

### Connection

- **AS7262** -> **Arduino Nano**
  - GND     -> GND
  - 3V3     -> 3.3V
  - SDA     -> A4 (Analog pin 4)
  - SCL     -> A5 (Analog pin 5)
- **Button**: D2 to GND (press to take a measurement)

### Operation

1. Press the button (D2 to GND) to take a measurement.
2. The sensor will measure the light intensity at 6 wavelengths.
3. The results will be displayed on the Serial Monitor in the format:
