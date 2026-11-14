# Zapojení (návrh, neověřeno)

Varianta se sdílenou sběrnicí VSPI pro displej i SD. Platí jen, pokud se
v [todo.md](todo.md) zvolí úprava kódu Anemoie; při samostatné čtečce SD
se SD přesune na HSPI (13/12/14) podle výchozí konfigurace Anemoie.

## Displej MSP2834 (14 pinů)

Podle tabulky prodejce: řadič ILI9341V (4vodičové SPI), IPS 240 × 320,
napájení 5 V, podsvícení 80 mA, převodník úrovní na desce, dotyk FT6336G
po I2C. Pro TFT_eSPI z toho plyne `ILI9341_DRIVER`, `TFT_WIDTH 240`,
`TFT_HEIGHT 320`. Ověřeno 2026-11-12 (`firmware/test_display`, 27 MHz):
displej funguje s `TFT_INVERSION_ON` a `TFT_RGB_ORDER TFT_BGR`. Výchozí
nastavení Anemoie je `TFT_INVERSION_OFF`, při portu změnit.

| Pin modulu | GPIO | Poznámka |
|---|---|---|
| VCC | 5 V | z MT3608; modul má vlastní regulátor a převodník úrovní |
| GND | GND | |
| LCD_CS | 5 | |
| LCD_RST | 4 | |
| LCD_RS (DC) | 2 | |
| SDI (MOSI) | 23 | společné s SD |
| SCK | 18 | společné s SD |
| LED | 3,3 V | vstup řízení podsvícení, ne indikační LED; viz poznámka pod tabulkou |
| SDO (MISO) | 19 | společné s SD |
| CTP_SCL / CTP_RST / CTP_SDA / CTP_INT | – | nezapojeno, dotyk se nepoužívá |
| SD_CS | 17 | volný pin, není strapping |

Pozorování 2026-11-14: podsvícení svítí i při odpojeném pinu LED. Pin LED
je vstup spínacího tranzistoru podsvícení (na desce Q1–Q3), který má na
modulu zřejmě vlastní pull-up; nezapojený pin = podsvícení zapnuté.
Schéma modulu nemám, výklad je odvozený z chování. Doporučeno nechat na
3,3 V (definovaný stav, odolnost proti rušení), případně později na GPIO
(21 nebo 22) s PWM pro řízení jasu a úsporu baterie.

## Ovládání

| Vstup | GPIO | Poznámka |
|---|---|---|
| UP / DWN / LFT / RHT | 32 / 33 / 25 / 26 | joystick, COM → GND, INPUT_PULLUP |
| A / B / Start | 14 / 13 / 27 | `tlacitka_1x4_maly`, GND → GND, INPUT_PULLUP; přemapováno 2026-11-14 |
| Select | 12 nyní, 16 cílově | viz níže |

Přemapování 2026-11-14 (po montáži do krabičky, jen v softwaru, vodiče beze
změny): tlačítko dříve „B" (GPIO14) je nyní A, dříve „Start" (GPIO13) je B,
dříve „A" (GPIO27) je Start, Select beze změny. Displej na šířku
(`setRotation(3)`, ověřeno 2026-11-14): slot SD nahoře, A/B vpravo.

Ověřeno 2026-11-12 (`firmware/test_buttons`): všech 8 vstupů funguje,
deska s tlačítkem na GPIO12 bootuje.

Select je na nepájivém poli dočasně na GPIO12 (kabel k RX2 nedosáhne).
V další fázi (fork Anemoie, pájená verze) přepojit na RX2 = GPIO16 a změnit
pin v kódu. Důvod: GPIO12 je strapping pin (HIGH při startu = 1,8 V flash,
deska nenabootuje). Dočasně bezpečné, protože tlačítko spíná jen na GND,
modul nemá pull-up k VCC a při startu drží pin vnitřní pull-down. Na GPIO12
nepřidávat externí pull-up. Kolize: GPIO12 je HSPI MISO, varianta B
(samostatná čtečka SD) by ho potřebovala.

Kříž + 1×4 malá = 8 nezávislých digitálních vstupů, což přesně odpovídá
režimu `CT_GPIO` Anemoie (`gpioRead()` v `src/controller.cpp`); čtení
tlačítek se nemusí upravovat, jen piny v `config.h`.

Alternativa `tlacitka_2x2` (matice L1, L2, R1, R2) zabere stejné 4 piny,
ale vyžaduje skenování matice, tedy úpravu `gpioRead()`. Dva současné stisky
matice 2×2 rozliší, duchy vznikají až od tří. Oproti 1×4 malým nepřináší nic
kromě rozložení 2×2.

Zamítnuto: barevná 4tlačítka (`tlacitka_1x4_analog`) jsou podle výkresu DPS
sériový odporový žebřík, současný stisk A + B by ADC nerozlišil (odvozeno
z výkresu, neměřeno).

## Rezerva

Volné GPIO po cílovém zapojení (Select na 16): 12, 15, 21, 22, 34, 35, 36, 39
(34/35/36/39 jen vstup; 12 a 15 strapping). GPIO 25 nebo 26 je DAC pro budoucí zvuk – při zapojení reproduktoru
přesunout příslušný směr kříže.
