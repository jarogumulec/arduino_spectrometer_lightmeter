# ESP32 NES handheld

Ruční herní konzole: ESP32 + 2,8" SPI displej se slotem microSD + směrový kříž
a 4 tlačítka, napájení z článku 18650. Cílem je NES emulátor s ROM na microSD.
Stav k 2026-11-12: na nepájivém poli ověřen displej a tlačítka; emulátor zatím nenahrán. Průběh v [docs/todo.md](docs/todo.md).

## Emulátor

Základ: [Anemoia-ESP32](https://github.com/Shim06/Anemoia-ESP32) (GPL-3.0).
Klasické dvoujádrové ESP32 bez PSRAM, displej ST7789/ILI9341 přes TFT_eSPI,
ROM z microSD (FAT32, `.nes` v kořeni), mappery 0, 1, 2, 3, 4, 69.
Build v Arduino IDE, ESP32 core 3.2.1, knihovny SdFat a TFT_eSPI; autor
vyžaduje nahradit `platform.txt` jádra (původní zazálohovat).

Hotový build nelze použít beze změn: displejový modul sdílí SPI sběrnici
s SD kartou, Anemoia počítá se dvěma sběrnicemi. Viz [docs/todo.md](docs/todo.md).

Zamítnuto: [SuperESP32World](https://github.com/SoftwareGuy/SuperESP32World)
není emulátor, jde o nedokončený port s výstupem jen na kompozitní video.

## Díly

Podrobnosti v [`../docs_boards_Available/board_dimensions.md`](../docs_boards_Available/board_dimensions.md).

| Díl | Složka | Role |
|---|---|---|
| ESP32 DevKit CP2102 USB-C | `esp32_devkit_cp2102` | MCU (WROOM, bez PSRAM) |
| TFT 2,8" IPS 240×320 (MSP2834, ILI9341V, FT6336G) | `tft_28_ips_touch` | displej + microSD; dotyk se nepoužije |
| Joystick 5směrný + SET + RST | `joystick_5smer` | kříž (4 mikrospínače, COM) |
| Tlačítka 1×4 malá (alternativa 2×2 matice) | `tlacitka_1x4_maly` / `tlacitka_2x2` | A, B, Start, Select |
| Držák 18650 | `drzak_18650` | článek |
| TP4056 nabíječka USB-C | `tp4056_nabijecka` | nabíjení + ochrana DW01 |
| MT3608 step-up | `mt3608_stepup` | 5,0 V pro ESP32 a displej |

## Struktura

| Cesta | Obsah |
|---|---|
| `docs/wiring.md` | zapojení pinů, návrh |
| `docs/power.md` | napájení a odhad odběru |
| `docs/todo.md` | otevřené otázky a další kroky |
| `firmware/test_display/` | test displeje + `User_Setup.h` pro TFT_eSPI |
| `firmware/test_buttons/` | test 8 tlačítek na displeji |
