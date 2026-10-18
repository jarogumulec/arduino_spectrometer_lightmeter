# Spektrometry a luxmetry

Dva projekty:

## `arduino_nano_lightmeter/` – Arduino Nano + AS7262 (starý projekt)

Spektrometr AS7262 (6 kanálů, 450–650 nm) + BH1750 luxmetr + OLED,
Arduino Nano. Sketche a dokumentace přesunuty z kořene repa.
Původní README: `arduino_nano_lightmeter/README_legacy.md`.

Poznámka: Arduino IDE vyžaduje, aby `.ino` bylo ve složce stejného jména –
sketche po přesunu otevřít zkopírováním/vytvořením odpovídající složky,
nebo ponechat jako archiv.

## `nano_spectrometer/` – Arduino Nano + AS7341/AS7343 (nový projekt)

GY modul spektrálního senzoru (typ určí identify sketch přes I²C registry)
na Arduino Nano, report v Pythonu (uv) – terminál + live graf, volitelný CSV.
Viz `nano_spectrometer/README.md`.
