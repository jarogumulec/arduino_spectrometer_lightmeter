# Kalibrace AS7341 – metoda, vzorce, validace, omezení

Stav k 2026-10-18. Kalibrace běží ve vieweru (`viewer/spectrometer_report.py`),
firmware posílá raw county + `CONF`.

## Vzorec

Ee [µW/cm²] = count / (Re × r_gain × tint / 27,8 ms)

- **Re** – irradiance responsivity [counts/(µW/cm²)] z datasheetu
  DS000504 v2-00, Figs 7–16, podmínky AGAIN 64×, tint 27,8 ms,
  kalibrační LED warm white 2700 K:

  | kanál | λp [nm] | FWHM [nm] | Re | 1 count = |
  |---|---|---|---|---|
  | F1 | 415 | 26 | 55 | 18,2 nW/cm² |
  | F2 | 445 | 30 | 110 | 9,1 nW/cm² |
  | F3 | 480 | 36 | 210 | 4,8 nW/cm² |
  | F4 | 515 | 39 | 390 | 2,6 nW/cm² |
  | F5 | 555 | 39 | 590 | 1,7 nW/cm² |
  | F6 | 590 | 40 | 840 | 1,2 nW/cm² |
  | F7 | 630 | 50 | 1350 | 0,74 nW/cm² |
  | F8 | 680 | 52 | 1070 | 0,93 nW/cm² |
  | Clear | – | broadband | 1750 | 0,57 nW/cm² |
  | NIR | ~910 | ~100 | 112 | 8,9 nW/cm² |

- **r_gain** – poměr optického gainu k 64× (Fig. 16): 0,5× = 0,008 …
  16× = 0,25 … 512× = 7,75.
- **tint** = (ATIME+1)·(ASTEP+1)·2,78 µs, reportováno v `CONF`.

Saturace (count = 65535) se do µW/cm² nezapočítává (SAT). Auto-ranging
firmwaru udržuje kanály pod klipem (gain 16×→0,5×, pak půlení ATIME).

## Vnitřní kontrola výpočtu

Rámec „přímé odpolední slunce", Clear 37458 counts při 0,5×/280,8 ms:
37458 / (0,008 × 10,1) = 463 000 countů ekvivalentně při 64×/27,8 ms;
463 000 / 1750 = 265 µW/cm². Ruční přepočet = výstup vieweru.

## Validace na reálných podmínkách (2026-10-18)

Externí ověření luxmetrem: indoor 85 lx, venkovní přímé slunce
54 600 lx (odpoledne, říjen).

| podmíní | gain | Clear [µW/cm²] | očekávání fyzikální |
|---|---|---|---|
| přímé slunce (odpoledne, říjen) | 0,5× | 265 | ~300–500 W/m² = 3–5·10⁴ µW/cm² |
| denní místnost | 16× | 0,68 | ~0,3–1 W/m² = 100–300 µW/cm² |

Obě podmínky chybí o faktor ~100–200 ve stejném směru. Závěry:

1. **Relativní spektra** (tvary, poměry kanálů) – použitelné.
2. **Absolutní µW/cm²** – systematicky podhodnocené; Re je definováno s
   difuzorem ED1-C50 na skle s difuzním světlem, modul difuzor nemá a
   apertura balíčku mění účinný příčný průřez. Hodnoty brát jako
   interní jednotku „senscounts", ne metrologicky.
3. **Empirická korekce**: souběžné měření Clear vs BH1750 (jiný projekt
   v repu), lineární fit, konstanta do vieweru. Teprve pak lze mluvit o
   µW/cm².

## Známá podezřelá kanál F2 (445 nm)

Poměr F2/F3 v µW/cm² = 2,26 (slunce) a 2,29 (místnost) – konzistentní
napříč podmínkami, zatímco sluneční SPD dává 445/480 ≈ 1 (odpoledne
spíš < 1). Pravděpodobná příčina: Re_F2 = 110 counts je kalibrováno na
warm white LED, kde modrá má minimum signálu – nejistota F1/F2 Re je
největší v tabulce. Rozlišovací experiment: LED ~450 nm proti
kalibrovanému luxmetru.

## Neověřeno

- Dark offset se neodečítá (datasheet 0–3 counts při 512×/98 ms; při
  běžných gainech pod 1 count, ale nezměřeno na tomto modulu).
- NIR Re = 112 je jednobodová warm-white kalibrace, nejmenší důvěra
  spolu s F1/F2.
- Linearita odezvy na tint (přepočet předpokládá lineární škálování;
  u dlouhých tint sublineární saturování ADC).
