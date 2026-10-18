# TODO List: Oprava kalibrace a zobrazení spektrometru AS7262

Tento dokument shrnuje provedené změny v kódu pro správné zobrazení absolutní intenzity světla v $\mu	ext{W}/	ext{cm}^2$ na OLED displeji.

## 🛠️ Provedené úpravy v kódu

1. **Implementace matematické normalizace citlivosti**
   * Původní kód vyčítal kalibrované registry, ale při automatické změně zisku a integračního času se hodnoty chovaly jako surové (při polovičním zisku byly poloviční).
   * Do funkce `takeSpectrophotometerMeasurement()` byl přidán přepočet vnitřních stavů registru zisku (`0, 1, 2, 3`) na skutečné násobiče zisku (`1.0x, 3.7x, 16.0x, 64.0x`).
   * Vypočítává se reálný čas integrace v milisekundách (`optimalIntegrationTime * 2.8`).

2. **Korekční faktor (`correctionFactor`)**
   * Vytvořen koeficient vyrovnávající odchylku nastavení oproti fixnímu továrnímu standardu ($16	imes$ zisk a $166	ext{ ms}$ integrační čas).
   * Všechny kanály jsou před odesláním na displej a do grafu vynásobeny tímto faktorem:
     $$	ext{correctionFactor} = rac{16.0}{	ext{aktuální zisk}} 	imes rac{166.0}{	ext{aktuální čas v ms}}$$

3. **Vizuální označení jednotek na OLED**
   * Ve funkci `displayData()` u textového zobrazení tabulky (`screenState == 0`) byla za každou vlnovou délku doplněna textová značka `" uW"`, aby bylo na první pohled zřejmé, že hodnoty jsou znormalizované a vyjádřené v $\mu	ext{W}/	ext{cm}^2$.

## 🚀 Další kroky pro uživatele
- [ ] Otevřít Arduino IDE.
- [ ] Zkopírovat opravený kód ze souboru `spectrometer_fixed.ino`.
- [ ] Ujistit se, že jsou nainstalovány potřebné knihovny (`AS726X`, `BH1750`, `U8glib`).
- [ ] Nahrát kód do Arduina Nano (případně zvolit správný *Old Bootloader* v nastavení procesoru).
- [ ] Otestovat měření v různých světelných podmínkách – hodnoty by nyní měly při změně zisku zůstat stabilní a odpovídat reálné intenzitě světla.
