# Spektrometry a luxmetry

Projekty s Arduino/ESP32 a sdílená databáze modulů.

## `docs_boards_Available/` – databáze dostupných desek a modulů

Společná pro všechny projekty: rozměry desek a montážních děr pro návrh
krabiček, I2C adresy, odkazy na obchody, datasheety, fotografie a ukázkový
kód, jedna podsložka na modul. Přehled a seznam chybějících údajů:
[`docs_boards_Available/board_dimensions.md`](docs_boards_Available/board_dimensions.md).
Datasheety patří sem, ne do `docs/` jednotlivých projektů.

## Projekty

### `arduino_nano_lightmeterBH1750_spectrometerAS7262/` – AS7262 + BH1750 (starý projekt)

Spektrometr AS7262 (6 kanálů, 450–650 nm) + BH1750 luxmetr + OLED,
Arduino Nano. Sketche a dokumentace přesunuty z kořene repa.
Původní README: `arduino_nano_lightmeterBH1750_spectrometerAS7262/README_legacy.md`.

Poznámka: Arduino IDE vyžaduje, aby `.ino` bylo ve složce stejného jména –
sketche po přesunu otevřít zkopírováním/vytvořením odpovídající složky,
nebo ponechat jako archiv.

### `arduino_nano_spectrophotometerAS7341/` – Arduino Nano + AS7341 (nový projekt)

GY modul spektrálního senzoru AS7341 (potvrzeno registry) na Arduino
Nano, report v Pythonu (uv) – terminál + live graf, volitelný CSV.
Viz `arduino_nano_spectrophotometerAS7341/README.md`.

### `esp32_nes_handheld/` – ESP32 herní konzole (NES emulátor)

ESP32 DevKit + 2,8" SPI displej (ILI9341V) se slotem microSD, kříž
a tlačítka, napájení 18650 + TP4056 + MT3608. Základ emulátoru
Anemoia-ESP32. Stav: kostra, otevřené otázky v
`esp32_nes_handheld/docs/todo.md`.
