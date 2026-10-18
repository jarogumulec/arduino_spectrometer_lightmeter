# Nano spektrometr (AS7341) – jednoduchý report

GY modul spektrálního senzoru na Arduino Nano V3 (ATmega328P), data po USB
sériové lince, report v Pythonu přes uv – terminál + live graf, volitelně CSV.

```
arduino_nano_spectrophotometerAS7341/
  firmware/readout_as7341/          readout AS7341 (potvrzeno na modulu)
  viewer/                           Python report (uv)
  docs/                             zapojení, kalibrace, datasheet
```

## Hardwarová identifikace – vyřešeno

Modul objednán pod jiným označením, na desce potisk AS7341. Registry potvrdily
AS7341 (WHOAMI 0x92). Historie v
[docs/hardware_identification.md](docs/hardware_identification.md).

## Zapojení a spuštění

Viz [docs/wiring.md](docs/wiring.md). Ve zkratce: VIN→5V, GND→GND,
SDA→A4, SCL→A5; nahrát `readout_as7341.ino` →

```bash
cd viewer
uv sync
uv run python spectrometer_report.py --port /dev/cu.usbserial-XXXX
```

## Měřené veličiny

Raw county 16bit ADC na kanál, start 16×, ATIME=100, ASTEP=999 (tint
280,78 ms). **Auto-ranging:** saturace (65535) → krok gainu dolů až po
0,5×, při trvalé saturaci půlení ATIME (100→50→25); tmavý rámec
(max < 400) → krok gainu nahoru. Změna konfigurace se ohlásí novým
`CONF` řádkem, viewer přepočet přizpůsobí automaticky. Adaptace max
jedna úroveň za rámec (~1 s). Wavelength 0 = Clear kanál, 910 = NIR.

Kalibrace na µW/cm² zavedena ve vieweru: Ee = count / (Re × r_gain ×
tint/27,8 ms), Re z [datasheetu](docs/AS7341_DS000504_datasheet.pdf)
DS000504 v2-00 (Figs 7–16), platné pro warm white 2700 K SPD; pro jiné
zdroje jen řádový odhad. Clear se reportuje bokem jako broadband osvit,
NIR vlastní Re (112, warm-white kalibrace). Saturace (count 65535) se
označuje SAT a do µW/cm² ani grafu se nezapočítává.

**Absolutní škála je systematicky ~100–200× podhodnocená** (Re je
definováno s difuzorem, modul ho nemá) – relativní spektra použitelná,
absolutní hodnoty jen jako interní jednotka, dokud neproběhne
empirická korekce proti BH1750. Detaily, validace i podezřelý kanál
F2 (445 nm) v [docs/calibration.md](docs/calibration.md).
Luxy zatím ne – Clear kanál nemá V(λ) tvar, empirický koeficient až po
souběžném měření s BH1750.

## Flicker

Každý 5. rámec firmware spustí hardwarovou detekci (FLICK řádek):
100/120 Hz rozpoznáno, 1 = detekováno neznámé frekvence, 0 = nic /
neznamo. Jiné frekvence než 100/120 Hz čip kategoricky neumí – na to by
byl potřeba buffered mód + FFT na hostiteli (na Nanu nereálné).

## Neověřeno

- Dark offset se neodečítá (při 16× podle datasheetu 0–1 count, ale
  nezměřeno).
- Re platnost pro reálné zdroje v této sestavě (±30 % pro broadband,
  monochromatické zdroje jinak).
- Nano V3 „asi" – old vs new bootloader se případně přepne v Arduino IDE.
