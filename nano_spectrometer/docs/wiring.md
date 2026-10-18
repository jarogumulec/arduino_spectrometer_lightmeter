# Zapojení – Arduino Nano + AS7341 / AS7343

## Desky

- GY-AS7341/GY-AS7343 breakout (objednán jako AS7343, na modulu potisk
  AS7341 – typ určí `identify_spectrometer.ino`)
- Arduino Nano V3.0 (ATmega328P); bootloader old i new podle desky

## Zapojení

```
GY-AS7341/43          Arduino Nano
------------          ------------
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
2. Arduino IDE, nahrát `firmware/identify_spectrometer/identify_spectrometer.ino`
   (deska Arduino Nano; nefunguje-li upload, zkusit Tools > Processor >
   ATmega328P (Old Bootloader)).
3. Serial Monitor 115200 → řádek `ID,AS7341` nebo `ID,AS7343`.
4. Podle výsledku nahrát `readout_as7341.ino` nebo `readout_as7343.ino`
   – v Arduino IDE nainstalovat Library Managerem `Adafruit AS7341`
   (příp. `Adafruit AS7343`); BusIO se dotáhne sám.
5. Report:

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
| `ID,...` | identifikace čipu (jen identify sketch) |
| `CONF,...` | konfigurace měření |
| `DATA,seq,wavelength_nm,count` | hodnota kanálu |
| `ENDD,seq` | konec rámce |
| `ERR,message` | chyba |

Wavelength 0 = Clear kanál. Rámec po ~1 s.
