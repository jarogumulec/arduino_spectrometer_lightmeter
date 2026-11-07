# Kalibrace AS7341 – metoda, vzorce, validace, omezení

Stav k 2026-10-18. Kalibrace běží ve vieweru (`viewer/spectrometer_report.py`),
firmware posílá raw county + `CONF`.

## Vzorec

Ee [µW/cm²] = count × 107,67 / (Re × r_gain × tint / 27,8 ms)

- **Re** – počet countů při Ee = 107,67 µW/cm² warm white 2700 K LED,
  AGAIN 64×, tint 27,8 ms, difuzor ED1-C50 (DS000504 v2-00, Figs 7–16).
  Re není v counts/(µW/cm²); do 2026-10-18 byl vzorec bez faktoru
  107,67 a výsledky byly o tento faktor podhodnocené (CSV logy z té
  doby je nutné vynásobit 107,67).

  | kanál | λp [nm] | FWHM [nm] | Re [counts] | 1 count = (64×, 27,8 ms) |
  |---|---|---|---|---|
  | F1 | 415 | 26 | 55 | 1,96 µW/cm² |
  | F2 | 445 | 30 | 110 | 0,979 µW/cm² |
  | F3 | 480 | 36 | 210 | 0,513 µW/cm² |
  | F4 | 515 | 39 | 390 | 0,276 µW/cm² |
  | F5 | 555 | 39 | 590 | 0,182 µW/cm² |
  | F6 | 590 | 40 | 840 | 0,128 µW/cm² |
  | F7 | 630 | 50 | 1350 | 0,0798 µW/cm² |
  | F8 | 680 | 52 | 1070 | 0,101 µW/cm² |
  | Clear | – | broadband | 1750 | 0,0615 µW/cm² |
  | NIR | ~910 | ~100 | 112 | 0,961 µW/cm² |

- **r_gain** – poměr optického gainu k 64× (Fig. 16): 0,5× = 0,008 …
  16× = 0,25 … 512× = 7,75.
- **tint** = (ATIME+1)·(ASTEP+1)·2,78 µs, reportováno v `CONF`.

Saturace (count = 65535) se do µW/cm² nezapočítává (SAT). Auto-ranging
firmwaru udržuje kanály pod klipem (gain 16×→0,5×, pak půlení ATIME).

## Význam jednotky

Výsledek kanálu je celková irradiance 2700 K LED, která by v daném
kanálu dala stejný počet countů. Není to spektrální irradiance
(µW/cm²/nm) ani irradiance v pásmu kanálu. Pro zdroj se spektrem 2700 K
LED dají všechny kanály stejnou hodnotu; pro jiný zdroj je hodnota
kanálu úměrná poměru zdroj/LED v jeho pásmu. Denní světlo je proti
2700 K LED bohaté na modrou, modré kanály proto vycházejí nadsazené.

## Vnitřní kontrola výpočtu

Rámec „přímé odpolední slunce", Clear 37458 counts při 0,5×/280,8 ms:
37458 / (0,008 × 10,1) = 463 000 countů ekvivalentně při 64×/27,8 ms;
463 000 × 107,67 / 1750 = 28 500 µW/cm². Ruční přepočet = výstup
vieweru (28 523 µW/cm²).

## Validace na reálných podmínkách (2026-10-18)

Externí ověření luxmetrem: indoor 85 lx, venkovní přímé slunce
54 600 lx (odpoledne, říjen). Hodnoty přepočteny opraveným vzorcem.

| podmínky | gain | Clear [µW/cm²] | očekávání fyzikální |
|---|---|---|---|
| přímé slunce (odpoledne, říjen) | 0,5× | 28 500 (285 W/m²) | ~300–500 W/m² = 3–5·10⁴ µW/cm² |
| denní místnost | 16× | 73 (0,73 W/m²) | ~0,3–1 W/m² = 30–100 µW/cm² |

Obě podmínky leží řádově ve shodě s očekáváním. Slunce mírně pod
dolní mezí – Clear nepokrývá celé sluneční spektrum a bez difuzoru
je přijímací kužel omezen (half cone angle 40°). Jde o jednobodové
srovnání, n = 1 měření na podmínku.

## Kanál F2 (445 nm) – pravděpodobně artefakt jednotky

Poměr F2/F3 = 2,26 (slunce) a 2,29 (místnost), konzistentní napříč
podmínkami. Oprava faktoru 107,67 poměr nemění. Pravděpodobné
vysvětlení: Re_F2 leží na úzkém píku modré pumpy LED, Re_F3 v
následujícím minimu (Fig. 15). Denní světlo je v rozsahu 445–480 nm
plošší, v jednotce „ekvivalent 2700 K" proto vychází F2 nadsazený.
Testovatelná predikce z existujících dat: F1 (415 nm, kde LED téměř
nesvítí) musí být v denním světle nadsazen ještě víc, F1/F3 > F2/F3.
Pokud ne, je podezřelý vlastní kanál F2.

## Neověřeno

- Dark offset se neodečítá (datasheet 0–3 counts při 512×/98 ms; při
  běžných gainech pod 1 count, ale nezměřeno na tomto modulu).
- NIR Re = 112 je jednobodová warm-white kalibrace, nejmenší důvěra
  spolu s F1/F2.
- Linearita odezvy na tint (přepočet předpokládá lineární škálování).
- Vliv chybějícího difuzoru: úhlová odezva a nehomogenní osvit 4×4
  pole kanálů.
