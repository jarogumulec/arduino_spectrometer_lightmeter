# Nano spektrometr (AS7341) – jednoduchý report

GY modul spektrálního senzoru na Arduino Nano V3 (ATmega328P), data po USB
sériové lince, report v Pythonu přes uv – terminál + live graf, volitelně CSV.

```
arduino_nano_spectrophotometerAS7341/
  firmware/readout_as7341/          readout AS7341 (potvrzeno na modulu)
  viewer/                           Python report (uv)
  docs/                             zapojení, kalibrace (datasheet: ../docs_boards_Available/as7341/)
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

Kalibrace na µW/cm² zavedena ve vieweru: Ee = count × 107,67 / (Re ×
r_gain × tint/27,8 ms), Re z [datasheetu](../docs_boards_Available/as7341/as7341_datasheet_DS000504.pdf)
DS000504 v2-00 (Figs 7–16) = county při 107,67 µW/cm² warm white 2700 K
LED. Výsledek je ekvivalent irradiance 2700 K LED, ne spektrální
irradiance; pro denní světlo jsou modré kanály nadsazené. Clear se
reportuje bokem jako broadband osvit, NIR vlastní Re (112, warm-white
kalibrace). Saturace (count 65535) se označuje SAT a do µW/cm² ani
grafu se nezapočítává.

Do 2026-10-18 chyběl ve vzorci faktor 107,67 (starší CSV logy jsou o
něj podhodnocené). Po opravě Clear souhlasí řádově s luxmetrem na
slunci i v místnosti. Detaily, validace a kanál F2 (445 nm) v
[docs/calibration.md](docs/calibration.md).
Luxy zatím ne – Clear kanál nemá V(λ) tvar, empirický koeficient až po
souběžném měření s BH1750.

Srovnání zdrojů (halogen, LED, denní světlo; AS7341 vs. AS7262 vs.
luxmetr): [tests.md](tests.md). Digitalizovaná spektrální citlivost
(Fig. 18) a spektra testovacích LED (Fig. 15) pro budoucí úpravu přepočtu:
[../docs_boards_Available/as7341/](../docs_boards_Available/as7341/),
popis v [databázi modulů](../docs_boards_Available/board_dimensions.md#as7341-spektrální-senzor-gy).

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
