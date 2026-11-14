# Napájení

18650 → TP4056 (B+/B−) → OUT+/OUT− → MT3608 (5,0 V) → VIN DevKitu + VCC displeje.

- Zátěž připojit na OUT+/OUT− TP4056, ne na B+, jinak neplatí ochrana DW01
  (odpojení pod 2,4 V, nadproud, zkrat).
- MT3608 nastavit voltmetrem na 5,0 V před připojením zátěže; trimr nejdřív
  naplno doleva (výstup ~24 V), pak snižovat.
- Vypínač zařadit mezi OUT+ a vstup MT3608.

## Odhad odběru (neměřeno)

| Spotřebič | Odhad |
|---|---|
| podsvícení displeje | 80 mA z 5 V (údaj výrobce, 0,4 W) |
| ESP32, WiFi vypnutá | 100–150 mA z 3,3 V (přes LDO DevKitu z 5 V) |
| celkem z článku | 350–400 mA při účinnosti step-upu ~90 % |

Článek 2500 mAh → řádově 6 h. Ověřit měřením (INA228 mezi TP4056 a MT3608).

## Programování přes USB

Ověřeno 2026-11-14: konzole běží z článku (TP4056 → vypínač → MT3608).

Při připojení DevKitu k PC přes USB: vypínač vypnout, článek může zůstat
v držáku. Vypnutý vypínač odpojí vstup MT3608, takže na VIN je jen napětí
z USB (přes diodu DevKitu), které napájí ESP32 i displej. Zpětně do MT3608
proud neteče, brání tomu jeho výstupní dioda. TP4056 a článek jsou oddělené
a nabíjení přes USB-C nabíječky funguje dál.

Se zapnutým vypínačem by se na VIN potkaly 5,0 V z MT3608 a cca 4,7 V
z USB. Ochranu PC před zpětným proudem by pak zajišťovala jen dioda mezi
USB a VIN na DevKitu; schéma tohoto klonu DevKitu neověřeno.
