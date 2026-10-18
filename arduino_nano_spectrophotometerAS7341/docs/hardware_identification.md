# Hardwarová identifikace – vyřešeno

Modul je **AS7341** – potvrzeno čtením I²C registrů (WHOAMI `0x92` =
0x24, hodnota 0x24–0x27). Modul byl objednán pod jiným označením, na
desce potisk AS7341; identifikační sketch i alternativní readout byly
po vyřešení smazány (2026-10-18).

Zdroj registrů: WHOAMI `0x92`, horní bity obsahují ID (test
`(read(0x92) & 0xFC) == 0x24`), REVID na `0x91`, AUXID na `0x90`
(ověřeno proti zdrojákům Adafruit knihovny).
