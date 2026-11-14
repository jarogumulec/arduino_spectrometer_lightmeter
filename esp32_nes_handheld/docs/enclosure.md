# Krabička

Tisk Prusa MK3S, parametrický model v OpenSCAD:
[`../enclosure/enclosure.scad`](../enclosure/enclosure.scad). Stav k 2026-11-13:
čelní deska vytištěna; vanička přepracována (článek uvnitř), netištěna.

Fotografie: [stav na nepájivém poli](IMG_0116.HEIC), [vypínač](IMG_0118.HEIC).

## Rozhodnutí

| Otázka | Volba |
|---|---|
| Rozložení | na šířku: kříž vlevo, displej uprostřed naležato, tlačítka vpravo |
| Konstrukce | zadní vanička + čelní deska, dva díly sešroubované M3 |
| Kolíkové lišty | zůstávají na DevKitu, displeji a joysticku, vodiče pájené na kolíky; lišta tlačítek 1×4 se odpájí |
| Článek 18650 | držák uvnitř na dně vaničky pod displejem, otevřenou stranou k displeji; výměna článku po sejmutí čelní desky (změna 2026-11-13, původně zvenku na zádech) |
| Tlačítka | `tlacitka_1x4_maly` svisle vpravo, shora A, B, Start, Select |
| Hmatníky | tištěné kolíčky s límcem procházející čelní deskou |
| Vypínač | posuvný s přírubou v horní stěně, příruba zevnitř, 2× M2 zvenku |
| SD karta | slot na displeji (varianta A v [todo.md](todo.md)), výřez v horní stěně |
| Spoje | M3 přímo do plastu (otvor 2,7 mm); TP4056 a MT3608 v rohových dorazech na dně |
| Přístupné zvenku | USB-C nabíjení (TP4056), microSD, vypínač |
| USB-C ESP32 | zvenku nepřístupné; nahrávání firmwaru po sejmutí čelní desky |
| Zvuk | prstenec a mřížka ve dně pro reproduktor Ø 28 mm (parametr), zesilovač volně |

## Rozměry dílů

M = změřeno 2026-11-12, O = výkres prodejce, P = předpoklad.

| Díl | Parametr | Hodnota |
|---|---|---|
| displej | deska | 86 × 50 mm M (dřívější údaj 82 mm byl chybný) |
| displej | díry | středy 7 mm od hrany s lištou, jinde 3 mm M; rozteč 76 × 44 |
| displej | sklo nad lícem desky | 4,5 mm M |
| displej | viditelná plocha | 58,05 × 43,6 mm, 18,81–76,86 mm od hrany s lištou O |
| displej | slot SD | u delší hrany na straně pinu VCC, 36–53 mm od hrany s lištou, karta 3 mm za hranou desky M |
| DevKit | výška s kolíky | 13,5 mm M |
| joystick | páčka | vrchol 11 mm nad deskou, Ø 8 mm, mechanika cca 10 × 10 mm M |
| joystick | střed páčky | 12,5 mm v kratší ose, 20,5 mm od hrany s lištou M |
| joystick | lišta | v rovině desky směrem UP M; přesah 9 mm P |
| joystick | díry | 3,5 a 8,5 mm od hran P (odvozeno z roztečí 18 × 24, symetricky) |
| tlačítka 1×4 | spínač | výška 6 mm M; hmatník Ø 3,5 mm P |
| tlačítka 1×4 | poloha | střed S1 7,5 mm od hrany M; rozteč 7,62 mm P (měřeno cca 7, z fotografie 7,6) |
| vypínač | páčka | 4 × 5 mm, výška 7 mm, zdvih 9 mm M |
| vypínač | díry v přírubě | rozteč 19 mm M; průměr P (M2) |
| TP4056 | deska | 27,5 mm, s USB-C 28,5 mm M; šířka 17,3 mm O |
| držák 18650 | rozměr | 76 × 21 × 21 mm M |

## Model

Vnější rozměr 157,8 × 64,8 × 34,9 mm. Vnitřní hloubka vaničky 30,5 mm
(výška držáku 21 mm + 9,5 mm pod zadní stranou displeje).

| Díl | STL | Rozměr tisku | Orientace |
|---|---|---|---|
| čelní deska | `enclosure/stl/plate.stl` | 157,8 × 64,8 × 9,7 mm | lícem dolů, bez podpor |
| vanička | `enclosure/stl/tray.stl` | 157,8 × 64,8 × 32,5 mm | dnem dolů, bez podpor (otvor USB a vypínače jsou krátké mosty) |
| vanička, zkušební odlehčená | `enclosure/stl/tray_light.stl` | 157,8 × 64,8 × 31,6 mm | dnem dolů, bez podpor |
| hmatníky 4× | `enclosure/stl/caps.stl` | Ø 7,2 × 6 mm | límcem dolů |

Zkušební vanička `tray_light` (2026-11-13): stejný vnější obrys, rohové
sloupky, dorazy a otvory jako plná verze, pasuje na stejnou čelní desku.
Stěna 1,6 mm, dno 1,1 mm s okny vyplněnými mřížkou (žebra 1,2 mm, rastr 8 mm,
výška 0,5 mm), spodní a boční stěny jen jako lem 6 mm (plná výška u rohů
a v horní stěně).

G-code (PrusaSlicer 2.9.3, MK3S+, Generic PLA, netištěno):

| Soubor | Obsah | Nastavení | Odhad |
|---|---|---|---|
| `enclosure/gcode/tray_light_caps_0.30mm_fast_PLA_MK3S.gcode` | zkušební vanička + 4 hmatníky (`stl/tray_light_caps.stl`) | 0.30mm DRAFT, perimetry 70/45 mm/s, první vrstva 30 mm/s, zrychlení 1000–1250, výplň 0 %, 3 horní vrstvy | 1 h 42 min, 31,8 g |
| `enclosure/gcode/caps_0.15mm_PLA_MK3S.gcode` | jen 4 hmatníky, přesnější | 0.15mm QUALITY beze změn | 11 min, 0,8 g |

Stejná sada s výchozím profilem DRAFT: 2 h 05 min. Slicer u hmatníků hlásí
nízkou přilnavost k podložce (malá základna Ø 7,2 mm); límec nepřidán.

Náhledy: `enclosure/preview/` (`front`, `iso_exploded`, `tray_layout`,
`tray_print`, `tray_light`, `light_assembly`, `plate_print`, `top_wall`, `back`).

Čelní deska byla vytištěna z verze 2026-11-12 a od té doby se nezměnila
(kontrola 2026-11-13: nový export má shodnou množinu vrcholů, 3 192).

Na čelní desce (zevnitř): displej na 4 sloupcích 4,7 mm, joystick na
4 sloupcích 4,5 mm (páčka vyčnívá 4,1 mm), tlačítka na 4 sloupcích 7,3 mm.
Ve vaničce: DevKit na 4 sloupcích 10,5 mm kolíky dolů, TP4056 vlevo nahoře
(USB-C horní stěnou), držák článku dole uprostřed v rohových dorazech,
MT3608 nastojato vpravo, vypínač v horní stěně vpravo, reproduktor vlevo
dole.

Šrouby: 4× M3×10 čelní deska–vanička, 4× M3×6 displej, 4× M3×6 joystick,
4× M3×8 tlačítka, 4× M3×6 DevKit, 2× M2 vypínač. Držák článku drží v dorazech, shora ho
jistí zadní strana displeje (vůle 1 mm).

Kontrola: oba hlavní díly jsou podle OpenSCAD uzavřená tělesa (Simple: yes);
rozměry STL odpovídají parametrům. Kolize desek kontrolovány jen výpočtem
a náhledem s maketami, ne tiskem.

## Neověřeno, rizika

- Hodnoty P v tabulce výše; před tiskem celku přeměřit díry joysticku
  a rozteč spínačů.
- Hmatníky: vůle otvoru 0,25 mm na stranu, rozteč 7,62 mm nechává mezi
  límci 0,4 mm. Vytisknout nejdřív hmatníky a zkušební proužek čelní desky.
- Karta microSD je 3 mm za hranou desky, tedy cca 6,5 mm pod vnějším
  povrchem stěny. Výřez 18 × 11 mm je otevřený k čelní desce; vytažení
  karty nehtem nebo pinzetou neověřeno.
- Průměr děr DevKitu (3 mm) pro M3 a průměr sloupků 5,5 mm vůči kolíkovým
  lištám.
- Držák článku: vodiče musí vycházet v podélné ose, pod držákem na dně
  není místo. Vůle mezi článkem a zadní stranou displeje 1 mm (výpočet).
- Vypínač: závit v přírubě neověřen (M2 zvenku, případně s maticí).
- Vůle DevKit–zadní strana displeje 6,5 mm (výpočet).

## Tisk

Materiál a tryska nerozhodnuty. Předpoklad modelu: tryska 0,4 mm, vrstva
0,2 mm, vůle 0,3 mm kolem desek.
