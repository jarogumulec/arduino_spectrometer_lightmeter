# Nano spektrometr (AS7341 / AS7343) – jednoduchý report

GY modul spektrálního senzoru na Arduino Nano V3 (ATmega328P), data po USB
sériové lince, report v Pythonu přes uv – terminál + live graf, volitelně CSV.

```
nano_spectrometer/
  firmware/identify_spectrometer/   identifikace čipu (čistý Wire)
  firmware/readout_as7341/          readout, vždy jen jedna z knihoven
  firmware/readout_as7343/
  viewer/                           Python report (uv)
  docs/                             zapojení, identifikace
```

## Hardwarová identifikace

Modul objednán jako AS7343, na desce potisk AS7341 – typ určí
`identify_spectrometer.ino` po prvním připojení
([docs/hardware_identification.md](docs/hardware_identification.md)).

## Zapojení a spuštění

Viz [docs/wiring.md](docs/wiring.md). Ve zkratce: VIN→5V, GND→GND,
SDA→A4, SCL→A5; identify sketch → podle `ID` řádku readout sketch →

```bash
cd viewer
uv sync
uv run python spectrometer_report.py --port /dev/cu.usbserial-XXXX
```

## Měřené veličiny

Raw county 16bit ADC na kanál, gain 16×, ATIME=100, ASTEP=999 (konstantní,
`CONF` řádek v protokolu). Wavelength 0 = Clear kanál. Kalibrace na µW/cm²
(normalizace na gain a integrační čas jako v arduinovém projektu) až po
ověření raw dat.

## Neověřeno

- Který čip je fyzicky na modulu (určí se prvním připojením).
- Zda se obě Adafruit knihovny vejdou do flash Nana po jedné – každý readout
  sketch je kompilován samostatně, ale velikost binárky nebyla ověřena
  (pravděpodobně OK, ~10–15 KB z 30 KB).
- Nano V3 „asi" – old vs new bootloader se případně přepne v Arduino IDE.
