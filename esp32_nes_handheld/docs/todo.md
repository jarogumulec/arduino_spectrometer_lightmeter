# TODO a otevřené otázky

## K rozhodnutí: SD karta

Displejový modul má jednu sadu datových vodičů (SCK, MOSI, MISO), po které
mluví displej i SD karta; která z nich poslouchá, určuje vlastní CS
(LCD_CS, SD_CS). Anemoia počítá se dvěma sadami vodičů: displej na jedné
sběrnici (VSPI), SD na druhé (HSPI). Během hry čte emulátor průběžně části
ROM z SD a současně posílá snímky na displej; při sdílené sběrnici by se
obojí střídalo na jednom vedení.

Možnosti:

| Varianta | Co obnáší | Riziko |
|---|---|---|
| A. upravit Anemoiu na sdílenou sběrnici | změna inicializace SD v kódu, ROM kopírovat do flash (režim `nesrom`), SD pak jen v menu | zásah do kódu; převodník úrovní na modulu omezí rychlost SD (odhad 20–40 MHz místo 80) |
| B. samostatná čtečka microSD na HSPI | dokoupit modul microSD (SPI); slot na displeji nevyužit; Anemoia v tomto bodě beze změn | další díl a 6 vodičů; čtečka musí mít 3,3 V logiku nebo převodník |
| C. jiný emulátor se sdílenou sběrnicí | dohledat projekt, který podporuje displej + SD na jednom SPI bez PSRAM | zatím nenalezen; retro-go nemá target pro tento hardware |

Doporučení zatím žádné definitivní; B je nejméně práce na kódu, A nejméně
dílů.

## Tlačítka

Na nepájivém poli `tlacitka_1x4_maly`, všech 8 vstupů ověřeno. Konečná volba
mezi 1×4 a 2×2 (ergonomie) při stavbě krabičky. Select přepojit z GPIO12 na
GPIO16 ([wiring.md](wiring.md)).

## Hotovo

| Datum | Krok | Výsledek |
|---|---|---|
| 2026-11-12 | prostředí | `arduino-cli` 1.6.0 (Homebrew), ESP32 core 3.0.2, TFT_eSPI 2.5.43; `User_Setup.h` knihovny nahrazen, původní jako `User_Setup.h.orig` |
| 2026-11-12 | test displeje (`firmware/test_display`) | ILI9341 funguje při 27 MHz; nutné `TFT_INVERSION_ON` + `TFT_BGR` |
| 2026-11-12 | test tlačítek (`firmware/test_buttons`) | 8/8 vstupů, A + B současně OK; Select dočasně GPIO12 |

## Další kroky

1. Rozhodnout variantu SD (výše), test SD: výpis kořenového adresáře.
2. Test displeje na 40 MHz (cíl Anemoie pro ILI9341).
3. Fork Anemoie do `firmware/`: `User_Setup.h` (ILI9341, inverze, BGR),
   `config.h` (piny tlačítek, Select → 16), SPI/SD podle varianty.
   Anemoia vyžaduje ESP32 core 3.2.1 a vlastní `platform.txt` – upgrade jádra
   jen po souhlasu.
4. Napájení z článku, měření odběru.
5. Krabička (rozměry v `board_dimensions.md`; u displeje přeměřit díry).

## Neověřeno

- SPI nad 27 MHz přes převodník úrovní na modulu.
- SD karta na sdílené sběrnici.
- Napájení displeje 3,3 V místo 5 V.
- Sériová linka: čtení z macOS (`arduino-cli monitor`) vrací nesmyslná data;
  příčina neznámá, výstup přes Serial Monitor v IDE nevyzkoušen.
