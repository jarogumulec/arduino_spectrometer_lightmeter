# Zapojení – Arduino Nano + AS7341

## Desky

- GY-AS7341 breakout (registry potvrdily AS7341)
- Arduino Nano V3.0 (ATmega328P); bootloader old i new podle desky

## Zapojení

```
GY-AS7341              Arduino Nano
---------              ------------
VIN            ->     5V
GND            ->     GND
SDA            ->     A4
SCL            ->     A5
```

Poznámky:

- Modul má level převod a napájení 2–5 V – 5V z Nana je v pořádku a je
  bezpečnější než 3V3 pin (Nano regulátor 3V3 maximálně ~50 mA, modul ale
  odbírá ~1 mA; obě varianty funkční).
- I²C adresa 0x39; další piny modulu (INT, GPIO) nechat volné.
- Nano se připojí USB; port v macOS `/dev/cu.usbserial-*` (FTDI) nebo
  `/dev/cu.wchusbserial*` (klon s CH340).

## Postup

1. Zapojit dle tabulky, USB do počítače.
2. Nahrát firmware – Arduino IDE (`firmware/readout_as7341/readout_as7341.ino`,
   knihovna `Adafruit AS7341` z Library Manageru) nebo z terminálu (board
   má old bootloader – při potížích se synchronizací držet RESET a pustit
   1–2 s po startu avrdude):

   ```bash
   "/Applications/Arduino IDE.app/Contents/Resources/app/lib/backend/resources/arduino-cli" \
     upload -p /dev/cu.usbserial-110 \
     --fqbn arduino:avr:nano:cpu=atmega328old \
     nano_spectrometer/firmware/readout_as7341
   ```
3. Report:

   ```bash
   cd viewer
   uv sync
   uv run python spectrometer_report.py --list
   uv run python spectrometer_report.py --port /dev/cu.usbserial-XXXX
   ```

## Sériový protokol (115200)

| řádek | význam |
|---|---|
| `# text` | diagnostika |
| `ID,AS7341` | identifikace čipu (setup) |
| `CONF,...` | konfigurace měření |
| `DATA,seq,wavelength_nm,count` | hodnota kanálu |
| `FLICK,seq,hz` | flicker detekce (každý 5. rámec): 100/120 Hz, 1 = neznámá frekvence, 0 = nic |
| `ENDD,seq` | konec rámce |
| `ERR,message` | chyba |

Wavelength 0 = Clear kanál, 910 = NIR. Rámec po ~1 s.
