# AS7262
https://www.sparkfun.com/sparkfun-spectral-sensor-breakout-as7262-visible-qwiic.html

The AS7262 detects 450, 500, 550, 570, 600, and 650nm of light each with 40nm of full-width half-max detection. The AS7263 can detect 610, 680, 730, 760, 810, and 860nm of light each with 20nm of full-width half-max detection.

Communication
⚡ Warning: You should only use a 3.3V USB-to-serial converter when connecting to the serial port. Using a 5V USB-to-serial converter may damage the components on the board.

The AS726X is unique as it can communicate by both an I2C interface (through the onboard Qwiic connectors, or pins at the bottom of the board) and serial interface using AT commands (pins at the top of the board).

Serial Ports
While I2C is the default setting (The default I2C address is 0x49 for both AS7262 and AS7263), serial communication via UART can be enabled by removing solder from the jumpers labeled JP1, adding solder to the jumper labeled JP2 (on the back of the board), and using Sparkfun's USB-to-Serial breakout to interface directly with the computer. JP1 also connects the I2C bus to pull-up resistors. If multiple sensors are connected to the bus with the pull-up resistors enabled, the parallel equivalent resistance will create too strong of a pull-up for the bus to operate correctly. As a general rule of thumb, disable all but one pair of pull-up resistors if multiple devices are connected to the bus. If you need to disconnect the pull up resistors they can be removed by removing the solder on jumper JP1 highlighted below.

Tutorial: https://learn.sparkfun.com/tutorials/as726x-nirvi

# BH1750
https://dratek.cz/arduino-platforma/902-mereni-intenzity-svetla-bh1750.html

Specifikace:

Snímač BH1750 Maximální proud 190 µA
Komunikační rozhraní I2C Citlivost 1 až 4 lx
Defaultní I2C adresa 0x23 Rozsah měření 1 až 65 535 lx
Výstupní napětí 3,3 nebo 5 V Rozměry (mm) 21 x 16 x 3,3


Použitý modul: BH1750FVI
Napájecí napětí: 3V - 5V
Balíček obsahuje jeden měřič intenzity napětí BH1750.

navod: 
https://navody.dratek.cz/navody-k-produktum/arduino-mereni-intenzity-svetla.html


# Arduino
https://dratek.cz/arduino-platforma/121745-usb-c-klon-arduino-nano-ch340.html

velikost nevím