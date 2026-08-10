# Arduino Codes for Light Measurement

## Prerequisites

- **Hardware**:
  - AS7262 Visible Spectrometer
  - BH1750 Light Sensor
  - Arduino Nano (ATMEGA 328P old bootloader)
  - OLED Display (128x64, SPI)
  - Push Buttons
  - Key Matrix 2x2 Buttons
  - (Deprecated) Photodiode
  - (Deprecated) 10k Resistor

- **Software**:
  - Arduino IDE (version 2.3.2 on macOS)
  - Libraries:
    - SparkFun AS726X (integrated)
    - Wire (for I2C connection)
    - BH1750 (library uploaded as `bh1750.zip`)
    - U8glib (library uploaded as `u8glib_oled.zip`)

## Integrated Spectrometer and Lightmeter

An integrated system combining the AS7262 spectrometer and BH1750 light sensor. This setup includes an OLED display for immediate data visualization. The system uses a key matrix with three buttons to trigger different functionalities. The BH1750 library is required for the light sensor, and the U8glib library is needed for the OLED display.

### Connection

- **AS7262** -> **Arduino Nano**
  - GND     -> GND
  - 3V3     -> 3.3V
  - SDA     -> A4 (Analog pin 4)
  - SCL     -> A5 (Analog pin 5)
- **BH1750** -> **Arduino Nano**
  - VCC     -> 3.3V or 5V
  - GND     -> GND
  - SCL     -> A5 (Analog pin 5)
  - SDA     -> A4 (Analog pin 4)
- **OLED Display** -> **Arduino Nano**
  - GND     -> GND
  - VCC     -> 5V
  - D0      -> D13 (SCK)
  - D1      -> D11 (MOSI)
  - RES     -> D10 (RESET)
  - DC      -> D9 (DC)
  - CS      -> D8 (CS)
- **Key Matrix** -> **Arduino Nano**
  - L1      -> D2
  - L2      -> D3
  - R1      -> D4
  - R2      -> D5

### Operation

- **Home Menu**:
Lighmeter 24-07-21
  - 1 - spectral analysis
  - 2 - luxmeter
  - 3 - PAR and K


- **Button 1**: Triggers the AS7262 spectrometer to measure light intensity at 6 wavelengths (450, 500, 550, 570, 600, 650 nm). The program optimizes the gain and integration time settings before taking measurements to avoid saturation. Results are displayed on the OLED and printed to the Serial Monitor.
- The OLED cycles through three screens: 
  1. **Chart** of the measurements.
  2. **Table** of the measurements.
  3. **Optimized Settings** used for the measurements (gain and integration time).

- **Button 2**: Triggers the BH1750 light sensor to measure ambient light intensity in lux. Results are displayed on the OLED and printed to the Serial Monitor.

- **Button 3**: Attempts to calculate PAR (Photosynthetically Active Radiation) and CCT (Correlated Color Temperature). The PAR calculation is based on the sum of calibrated spectral values. The CCT calculation is an approximation and may not provide accurate results. Both PAR and CCT values are displayed on the OLED and printed to the Serial Monitor.

### Optimization Process

- The program optimizes the gain and integration time settings for the AS7262 spectrometer by iterating through a predefined set of settings:
1. Integration time: 50ms, Gain: 3.7x
2. Integration time: 50ms, Gain: 16x
3. Integration time: 100ms, Gain: 16x
4. Integration time: 200ms, Gain: 16x
5. Integration time: 200ms, Gain: 64x
- The optimization process stops if any raw spectral value exceeds 50,000, indicating potential saturation. The last valid settings are used for the final measurement.

## Light Meter (Deprecated)

A light meter based on a photoresistor that records light intensity every 5 minutes and stores the data to EEPROM. This version is now deprecated in favor of the integrated spectrometer and lightmeter setup.

### Filename

- `lightmeter.ino`

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

## Simple Light Meter (Deprecated)

A simple light meter with the same connection as the previous version. It does not store data but outputs data in lux based on internal calibration. Tested on a linear range of 200 - 10000 lux, where it works quite well.

### Filename

- `lightmeter_simple_reportonly.ino`


## 2026-07-12 update - show calibrated data

> spectrometer_lightmeter_lightmeter_show-calibrated.ino


Tento dokument shrnuje provedené změny v kódu pro správné zobrazení absolutní intenzity světla v $\mu	ext{W}/	ext{cm}^2$ na OLED displeji.

### 🛠️ Provedené úpravy v kódu

1. **Implementace matematické normalizace citlivosti**
   * Původní kód vyčítal kalibrované registry, ale při automatické změně zisku a integračního času se hodnoty chovaly jako surové (při polovičním zisku byly poloviční).
   * Do funkce `takeSpectrophotometerMeasurement()` byl přidán přepočet vnitřních stavů registru zisku (`0, 1, 2, 3`) na skutečné násobiče zisku (`1.0x, 3.7x, 16.0x, 64.0x`).
   * Vypočítává se reálný čas integrace v milisekundách (`optimalIntegrationTime * 2.8`).

2. **Korekční faktor (`correctionFactor`)**
   * Vytvořen koeficient vyrovnávající odchylku nastavení oproti fixnímu továrnímu standardu ($16	imes$ zisk a $166	ext{ ms}$ integrační čas).
   * Všechny kanály jsou před odesláním na displej a do grafu vynásobeny tímto faktorem:
     $$	ext{correctionFactor} = rac{16.0}{	ext{aktuální zisk}} 	imes rac{166.0}{	ext{aktuální čas v ms}}$$

3. **Vizuální označení jednotek na OLED**
   * Ve funkci `displayData()` u textového zobrazení tabulky (`screenState == 0`) byla za každou vlnovou délku doplněna textová značka `" uW"`, aby bylo na první pohled zřejmé, že hodnoty jsou znormalizované a vyjádřené v $\mu	ext{W}/	ext{cm}^2$.

### 🚀 Další kroky pro uživatele
- [ ] Otevřít Arduino IDE.
- [ ] Zkopírovat opravený kód ze souboru `spectrometer_fixed.ino`.
- [ ] Ujistit se, že jsou nainstalovány potřebné knihovny (`AS726X`, `BH1750`, `U8glib`).
- [ ] Nahrát kód do Arduina Nano (případně zvolit správný *Old Bootloader* v nastavení procesoru).
- [ ] Otestovat měření v různých světelných podmínkách – hodnoty by nyní měly při změně zisku zůstat stabilní a odpovídat reálné intenzitě světla.


## Spectrophotometer (Deprecated)

An Arduino module using the SparkFun AS7262 Spectral Sensor Breakout for visible light. It reports the calibrated light intensity at 6 wavelengths (450, 500, 550, 570, 600, 650 nm) in units of µW/cm². This version is now deprecated in favor of the integrated spectrometer and lightmeter setup.

### Filename

- `spectrophoto.ino`

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
3. The results will be displayed on the Serial Monitor.

## Reflected Light Spectrophotometer (Deprecated)

Works similarly to the spectrophotometer but uses an additional diode (5700K white LED with an irradiance of ~600μW/cm²) to measure reflected light. Additionally, it reports the value for Lab coordinates, though calibration is still a work in progress. This version is now deprecated in favor of the integrated spectrometer and lightmeter setup.

### Filename

- `reflected_light.ino`

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
3. The results, including the Lab coordinates, will be displayed on the Serial Monitor.

## Notes

- The integrated spectrometer and light meter setup is now the primary and recommended configuration.
- The previous versions of the light meter, simple light meter, spectrophotometer, and reflected light spectrophotometer are now deprecated.
- The integrated system includes a key matrix with three buttons for different functionalities, with clear instructions on how to use each button.
- The CCT calculation is still a work in progress and may not provide accurate results at this stage.
