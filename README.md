# arduino_codes for light

## prerequesitites: 

photodiode, 10k resistor, AS7262 vis spectrometer, arduino nano (ATMEGA 3285 old boodloader is mine) and software Arduino IDE (mine is 2.3.2 on mac), needed also libraries Sparkfun AS726X and wire (for I2C connection of module)

# lightmeter
Lightmeter based on photoresustor which records ligh every 5 minutes and stores to Eprom. To start recotding, need to click the button (D2+GND) which also deletes all prev measurements. Then, once connected to PC you can retrieve data by hitting r o delete hitting c.

## conection

D2+gnd start measurement (led blinks 3x) then measures every 5 s. oafter measurements blinks. Connection: photoresistor to 5V > other end to (A0 and (node) 
through 10k ohm resistor to GND)

## operation

now the button D2+ground deletes all measurements, and start the measurements every 5 mins. after connecting to PC by clicking r all data is viewed.

# spectrophotometer

Another arduino module using SparkFun Spectral Sensor Breakout - AS7262 vor visible light, which reports the calibrated light intensity in 6 wavelengths 450, 500, 550, 570, 600 and reports the intensity in calibrated untis: Intensity (µW/cm²). the value is shown once the button (D2+gnd) is pressed.

## connection: 

AS7262        Arduino Nano
---------------------------
GND     ->    GND
3V3     ->    3.3V
SDA     ->    A4 (Analog pin 4)
SCL     ->    A5 (Analog pin 5)
Button  ->    D2 (Digital pin 2) to GND (button is connected not on AS unit, its cust vable which is connected to gnd for measurement)

## operation 
hit the button D2+gnd to view last measurements
