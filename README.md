# Spektrometry a luxmetry

Dva projekty:

## `arduino_nano_lightmeterBH1750_spectrometerAS7262/` – AS7262 + BH1750 (starý projekt)

Spektrometr AS7262 (6 kanálů, 450–650 nm) + BH1750 luxmetr + OLED,
Arduino Nano. Sketche a dokumentace přesunuty z kořene repa.
Původní README: `arduino_nano_lightmeterBH1750_spectrometerAS7262/README_legacy.md`.

Poznámka: Arduino IDE vyžaduje, aby `.ino` bylo ve složce stejného jména –
sketche po přesunu otevřít zkopírováním/vytvořením odpovídající složky,
nebo ponechat jako archiv.

## `arduino_nano_spectrophotometerAS7341/` – Arduino Nano + AS7341 (nový projekt)

GY modul spektrálního senzoru AS7341 (potvrzeno registry) na Arduino
Nano, report v Pythonu (uv) – terminál + live graf, volitelný CSV.
Viz `arduino_nano_spectrophotometerAS7341/README.md`.
