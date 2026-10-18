# Identifikace hardware – AS7341 vs AS7343

## Situace

Modul objednán jako GY-AS7343 (14 kanálů), na desce je však potištěno AS7341.
Označení čipu („AM TLL") není čitelné. Popis modulu: level převod, napájení
2–5 V, I²C adresa 0x39. Jediný spolehlivý způsob rozlišení je přečtení
I²C registrů – k tomu slouží `firmware/identify_spectrometer/`.

## Kanály

| čip | kanály | rozsah |
|---|---|---|
| AS7341 | 8× VIS (F1–F8: 415, 445, 480, 515, 555, 590, 630, 680 nm), Clear, NIR (~910 nm), flicker | 350–1000 nm |
| AS7343 | 12× VIS rovnoměrně (405–690 nm), NIR (855 nm), Clear, XYZ kanály, flicker | 380–1000 nm |

## Registry pro rozlišení (ověřeno proti zdrojákům Adafruit knihoven)

Oba čipy na I²C adrese 0x39:

- **AS7341**: WHOAMI registr `0x92`, hodnota ID 0x09 v horních bitech –
  test `(read(0x92) & 0xFC) == (0x09 << 2)`, tj. 0x24–0x27
  (`Adafruit_AS7341::_init()`). REVID na 0x91, AUXID na 0x90.
- **AS7343**: ID registr `0x5A` = 0x81, ale jen v **bance 1** registrů.
  Banka se přepíná bitem 4 CFG0 (`0xBF`); po přečtení nutno CFG0 obnovit.
  REVID na 0x59, AUXID na 0x58.

## Proč tři sketche

ATmega328P má 32 KB flash a 2 KB RAM; obě Adafruit knihovny s BusIO
současně se do jednoho sketchi nevejdou. Identifikační sketch proto používá
jen holý Wire, readout sketche jsou oddělené a každý obsahuje jen svou
knihovnu.

## Očekávané výstupy identify sketche

```
# WHOAMI(0x92) = 0x24        <- AS7341 (0x24–0x27)
ID,AS7341
```
nebo
```
# WHOAMI(0x92) = 0x00
# AS7343 banka1: ID(0x5A)=0x81 ...
ID,AS7343
```

Není-li na sběrnici 0x39 nic nebo registry nesedí, sketch vypíše
`ID,UNKNOWN,...` spolu s hex hodnotami – ty poslouží k dalšímu určení.
