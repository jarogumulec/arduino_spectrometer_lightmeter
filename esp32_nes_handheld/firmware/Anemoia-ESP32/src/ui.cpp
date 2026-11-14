#include "ui.h"

// HANDHELD: ROM vlozene ve firmwaru, generuje tools/make_roms.sh z ../../roms/*.nes
#if __has_include("roms_data.h")
    #include "roms_data.h"
#else
struct EmbeddedRom
{
    const char* name;
    const uint8_t* data;
    uint32_t size;
};
static const EmbeddedRom EMBEDDED_ROMS[] = { { nullptr, nullptr, 0 } };
static constexpr int EMBEDDED_ROM_COUNT = 0;
#endif
static const char* BUTTON_TEST_ITEM = "[Test tlacitek]";

UI::UI(TFT_eSPI* screen)
{
    this->screen = screen;
}

UI::~UI()
{
}

Cartridge* UI::selectGame()
{
    unsigned int last_input_time = 0;
    constexpr unsigned int delay = 250;
    extern bool demo_mode_active;
    extern unsigned int demo_mode_roms_menu_timeout;
    max_items = (screen->height() - 56) / ITEM_HEIGHT;

    getNesFiles();

    bool show_roms_menu = true;
    if (runtime_config.demo_mode && demo_mode_roms_menu_timeout == 0) { show_roms_menu = false; }
    if (show_roms_menu)
    {
        drawWindowBox(2, 20, screen->width() - 4, screen->height() - 40);
        drawBars();
        drawFileList();
        restoreBrightness();
    }

    const int size = files.size();
    while (true)
    {
        bool game_chosen = false;
        unsigned int now = millis();

        if (now - last_input_time > delay)
        {
            if (isDownPressed(CONTROLLER::Up))
            {
                selected--;
                if (selected < 0)
                {
                    selected = (size - 1);
                    scroll_offset = selected - max_items + 1;
                }
                else if (selected < scroll_offset) scroll_offset = selected;
                if (scroll_offset < 0) scroll_offset = 0;
                if (scroll_offset > size - 1) scroll_offset = size - 1;
                drawFileList();
                last_input_time = now;
            }

            if (isDownPressed(CONTROLLER::Down))
            {
                selected++;
                if (selected > (size - 1))
                {
                    selected = 0;
                    scroll_offset = selected;
                }
                else if (selected >= scroll_offset + max_items)
                    scroll_offset = selected - max_items + 1;
                if (scroll_offset < 0) scroll_offset = 0;
                if (scroll_offset > size - 1) scroll_offset = size - 1;
                drawFileList();
                last_input_time = now;
            }

            if (isDownPressed(CONTROLLER::Left))
            {
                int screen_pos = selected - scroll_offset;
                selected -= max_items;
                if (selected < 0) selected = 0;
                scroll_offset = selected - screen_pos;
                if (scroll_offset < 0) scroll_offset = 0;
                drawFileList();
                last_input_time = now;
            }

            if (isDownPressed(CONTROLLER::Right))
            {
                int screen_pos = selected - scroll_offset;
                selected += max_items;
                if (selected > size - 1) selected = size - 1;
                scroll_offset = selected - screen_pos;
                if (scroll_offset < 0) scroll_offset = 0;
                if (scroll_offset > size - 1) scroll_offset = size - 1;
                drawFileList();
                last_input_time = now;
            }

            // HANDHELD: prepinani backendu (Select) zruseno, ROM jsou vzdy ve flash
        }

        if (isDownPressed(CONTROLLER::A) && (selected >= 0 && selected < size))
        {
            game_chosen = true;
        }

        if (runtime_config.demo_mode)
        {
            if (controllerRead())
            {
                demo_mode_active = false; // user input detected, demo mode disabled
            }
            if (demo_mode_active && (now - last_input_time) >= demo_mode_roms_menu_timeout)
            {
                selected = esp_random() % size;
                game_chosen = true;
            }
        }

        if (game_chosen)
        {
            // HANDHELD: posledni polozka = test tlacitek, pak zpet do menu
            if (selected == EMBEDDED_ROM_COUNT)
            {
                buttonTest();
                screen->fillScreen(BG_COLOR);
                drawWindowBox(2, 20, screen->width() - 4, screen->height() - 40);
                drawBars();
                prev_selected = -1;
                drawFileList();
                last_input_time = millis();
                continue;
            }

            // Turn backlight off
            // Backlight will be turned back on after game is drawn to hide
            // visual glitches
            if (runtime_config.backlight) { ledcWrite(TFT_BACKLIGHT_PIN, 0); }

            const EmbeddedRom& rom = EMBEDDED_ROMS[selected];
            std::vector<std::string>().swap(files);
            screen->fillScreen(TFT_BLACK);
            return new Cartridge(rom.data, rom.size);
        }
    }
}

void UI::getNesFiles()
{
    // HANDHELD: seznam z firmwaru misto korene SD karty
    files.clear();
    for (int i = 0; i < EMBEDDED_ROM_COUNT; i++) files.push_back(EMBEDDED_ROMS[i].name);
    files.push_back(BUTTON_TEST_ITEM);
}

void UI::drawFileList()
{
    if (prev_selected != selected)
        screen->fillRect(10, 32, screen->width() - 20, screen->height() - 64, BG_COLOR);

    const int size = files.size();
    for (int i = 0; i < max_items; i++)
    {
        int item = i + scroll_offset;
        if (item >= size) break;

        std::string file = files[item];
        int maxWidth = screen->width() - 28;
        while (screen->textWidth(file.c_str()) > maxWidth) { file.pop_back(); }
        if (file.size() < files[item].size()) { file.replace(file.size() - 3, 3, "..."); }

        const char* filename = file.c_str();
        int y = i * ITEM_HEIGHT + 32;
        if (item == selected)
        {
            screen->setTextColor(SELECTED_TEXT_COLOR);
            screen->drawString(filename, 14, y, 1);
        }
        else
        {
            screen->setTextColor(TEXT_COLOR);
            screen->drawString(filename, 14, y, 1);
        }
    }

    prev_selected = selected;
}

void UI::drawWindowBox(int x, int y, int w, int h)
{
    screen->drawRect(x, y, w, h, TFT_WHITE);
    screen->drawRect(x + 1, y, w - 2, h, TFT_WHITE);

    screen->drawRect(x + 4, y + 3, w - 8, h - 7, TFT_WHITE);
    screen->drawRect(x + 5, y + 3, w - 10, h - 7, TFT_WHITE);

    const char* text1 = " ANEMOIA.CPP ";
    screen->setTextColor(TEXT_COLOR, BG_COLOR);
    screen->setCursor((int16_t)((screen->width() - screen->textWidth(text1)) / 2), 20);
    screen->print(text1);
}

void UI::drawBars()
{
    // Top bar
    screen->fillRect(0, 0, screen->width(), 16, BAR_COLOR);
    screen->setTextColor(TFT_BLACK, BAR_COLOR);

    const char* text1 = "ANEMOIA-ESP32";
    screen->setCursor((int16_t)((screen->width() - screen->textWidth(text1)) / 2), 4);
    screen->print(text1);

    // Bottom bar
    screen->fillRect(0, screen->height() - 16, screen->width(), 16, BAR_COLOR);
    screen->setTextColor(TFT_BLACK, BAR_COLOR);

    const int16_t y = screen->height() - 12;
    const int16_t x = 4;

    screen->setTextColor(TEXT2_COLOR, BAR_COLOR);
    screen->setCursor(x, y);
    screen->print("Up/Down");

    screen->setTextColor(TFT_BLACK, BAR_COLOR);
    screen->print(" Move   ");

    screen->setTextColor(TEXT2_COLOR, BAR_COLOR);
    screen->print("A");

    screen->setTextColor(TFT_BLACK, BAR_COLOR);
    screen->print(" Select");
}

void UI::pauseMenu(Nes* nes)
{
    // Black magic stuff
    // Padding bytes for code alignment for better performance
    __attribute__((used, section(".text"), aligned(64))) static const uint8_t padding[128] = { 0 };

    paused = true;
    int prev_select = 0;
    int select = 0;

    screen->endWrite();

    // Draw bars with text
    drawBars();
    const char* text2 = "Pause";
    int text2_x = screen->width() - screen->textWidth(text2) - 12;
    screen->fillRect(text2_x - 4, 0, screen->textWidth(text2) + 8, 16, SELECTED_BG_COLOR);
    drawText(text2, text2_x, 4);

    // HANDHELD: ukladani pozic vyrazeno (bylo na SD)
    constexpr int section_count[] = { 3, 1 };
    constexpr const char* items[] = { "Resume", "Settings", "Reset", "Quit to menu" };
    enum ItemSelect : uint8_t
    {
        Resume,
        Settings,
        Reset,
        SaveAndQuit
    };
    constexpr int items_y[] = { 28, 40, 52, 72 };
    constexpr int num_items = sizeof(items) / sizeof(items[0]);
    constexpr int num_sections = sizeof(section_count) / sizeof(section_count[0]);
    constexpr int item_height = 12;
    constexpr int text_height = 8;
    constexpr int text_padding = (item_height - text_height) / 2;

    // Draw pause window
    constexpr int window_w = 124;
    constexpr int window_h = 80; // HANDHELD: 4 polozky
    int window_x = screen->width() - window_w;
    constexpr int window_y = 16;
    screen->fillRect(window_x, window_y, window_w, window_h, BAR_COLOR);

    // Draw section borders
    int section_y = window_y + 8;
    for (int s = 0; s < num_sections; s++)
    {
        int w = window_w - 16;
        int h = (section_count[s] * item_height) + 8;
        screen->drawRect(window_x + 8, section_y, w, h, TFT_BLACK);
        screen->drawRect(window_x + 9, section_y, w, h, TFT_BLACK);

        section_y += (h - 1);
    }

    // Draw items
    screen->fillRect(window_x + 10, items_y[0], window_w - 19, item_height, SELECTED_BG_COLOR);
    for (int i = 0; i < num_items; i++)
    {
        int y = items_y[i] + text_padding;
        drawText(items[i], window_x + 12, y);
    }

    constexpr int initial_delay = 500;
    int last_input_time = millis() + initial_delay;
    while (true)
    {
        constexpr int delay = 250;
        int now = millis();
        if (now - last_input_time > delay)
        {
            if (isDownPressed(CONTROLLER::Up))
            {
                select--;
                if (select < 0) select = (num_items - 1);
                last_input_time = now;
            }

            if (isDownPressed(CONTROLLER::Down))
            {
                select++;
                if (select > (num_items - 1)) select = 0;
                last_input_time = now;
            }

            if (isDownPressed(CONTROLLER::A))
            {
                switch (select)
                {
                case Resume:
                    screen->fillScreen(TFT_BLACK);
                    screen->startWrite();
                    paused = false;
                    return;

                case Settings:
                    settingsMenu(nes);

                    // Redraw pause menu
                    screen->fillRect(window_x, window_y, window_w, window_h, BAR_COLOR);
                    section_y = window_y + 8;
                    for (int s = 0; s < num_sections; s++)
                    {
                        int w = window_w - 16;
                        int h = (section_count[s] * item_height) + 8;
                        screen->drawRect(window_x + 8, section_y, w, h, TFT_BLACK);
                        screen->drawRect(window_x + 9, section_y, w, h, TFT_BLACK);

                        section_y += (h - 1);
                    }
                    screen->fillRect(window_x + 10, items_y[select], window_w - 19, item_height,
                                     SELECTED_BG_COLOR);
                    for (int i = 0; i < num_items; i++)
                    {
                        int y = items_y[i] + text_padding;
                        drawText(items[i], window_x + 12, y);
                    }
                    last_input_time = millis() + 500;
                    break;

                case Reset:
                    nes->reset();
                    screen->fillScreen(TFT_BLACK);
                    screen->startWrite();
                    paused = false;
                    return;

                case SaveAndQuit:
                    // turn off audio before restart to prevent speaker popping
                    i2s_driver_uninstall(I2S_NUM_0);
                    ESP.restart();
                    return;
                default: break;
                }
            }
        }

        // Update Selection
        if (prev_select != select)
        {
            int y;
            // Redraw old selection
            screen->fillRect(window_x + 10, items_y[prev_select], window_w - 19, item_height,
                             BAR_COLOR);
            y = items_y[prev_select] + text_padding;
            drawText(items[prev_select], window_x + 12, y);

            // Draw new selection
            screen->fillRect(window_x + 10, items_y[select], window_w - 19, item_height,
                             SELECTED_BG_COLOR);
            y = items_y[select] + text_padding;
            drawText(items[select], window_x + 12, y);
        }

        prev_select = select;
    }
}

void UI::settingsMenu(Nes* nes)
{
    // Draw settings window

    int prev_select = 0;
    int select = 0;

    constexpr int window_w = 124;
    constexpr int window_h = 104;
    int window_x = screen->width() - window_w;
    constexpr int window_y = 16;
    screen->fillRect(window_x, window_y, window_w, window_h, BAR_COLOR);

    const char* text2 = "Settings";
    int text2_x = screen->width() - screen->textWidth(text2) - 12;
    screen->fillRect(text2_x - 4, 0, screen->textWidth(text2) + 8, 16, SELECTED_BG_COLOR);
    drawText(text2, text2_x, 4);

    static char volume_text[15];
    static char palette_text[20];
    static char brightness_text[20];
    static char save_return_text[] = "Save & Return";
    const char* palette_names[] = { "NTSC 565", "PAL 565", "NTSC 222", "PAL 222" };

    snprintf(volume_text, sizeof(volume_text), "Volume: %d%%", settings.volume);
    snprintf(palette_text, sizeof(palette_text), "Palette: %s", palette_names[settings.palette]);
    snprintf(brightness_text, sizeof(brightness_text), "Brightness: %d%%", settings.brightness);
    char* items[] = { volume_text, brightness_text, palette_text, save_return_text };
    enum ItemSelect
    {
        Volume,
        Brightness,
        Palette,
        Back
    };
    constexpr int16_t items_y[] = { 30, 42, 54, 66 };
    constexpr int num_items = sizeof(items) / sizeof(items[0]);
    constexpr int item_height = 12;
    constexpr int text_height = 8;
    constexpr int text_padding = (item_height - text_height) / 2;

    screen->drawRect(window_x + 8, window_y + 8, window_w - 16, window_h - 16, TFT_BLACK);
    screen->drawRect(window_x + 9, window_y + 8, window_w - 16, window_h - 16, TFT_BLACK);

    screen->fillRect(window_x + 10, items_y[0], window_w - 19, item_height, SELECTED_BG_COLOR);
    for (int i = 0; i < num_items; i++)
    {
        int16_t y = items_y[i] + text_padding;
        if (i == Brightness)
        {
            if (runtime_config.backlight) { drawText(items[i], window_x + 12, y); }
            else
            {
                screen->setCursor(window_x + 12, y);
                screen->setTextColor(TFT_DARKGREY);
                screen->print(items[i]);
            }
        }
        else drawText(items[i], window_x + 12, y);
    }

    constexpr int initial_delay = 500;
    unsigned long last_input_time = millis() + initial_delay;
    while (true)
    {
        constexpr int delay = 250;
        unsigned long now = millis();
        if (now - last_input_time > delay)
        {
            if (isDownPressed(CONTROLLER::Up))
            {
                select--;
                if (select < 0) select = (num_items - 1);
                if (!runtime_config.backlight && select == Brightness)
                {
                    select--;
                    if (select < 0) select = (num_items - 1);
                }
                last_input_time = now;
            }

            if (isDownPressed(CONTROLLER::Down))
            {
                select++;
                if (select > (num_items - 1)) select = 0;
                if (!runtime_config.backlight && select == Brightness)
                {
                    select++;
                    if (select > (num_items - 1)) select = 0;
                }
                last_input_time = now;
            }

            if (isDownPressed(CONTROLLER::Left))
            {
                switch (select)
                {
                case Volume:
                    if (settings.volume >= 5) settings.volume -= 5;
                    snprintf(volume_text, sizeof(volume_text), "Volume: %d%%", settings.volume);
                    screen->fillRect(window_x + 10, items_y[Volume], window_w - 19, item_height,
                                     SELECTED_BG_COLOR);
                    drawText(items[Volume], window_x + 12, items_y[Volume] + text_padding);
                    break;
                case Brightness:
                    if (settings.brightness >= 10) settings.brightness -= 5;
                    snprintf(brightness_text, sizeof(brightness_text), "Brightness: %d%%",
                             settings.brightness);
                    screen->fillRect(window_x + 10, items_y[Brightness], window_w - 19, item_height,
                                     SELECTED_BG_COLOR);
                    drawText(items[Brightness], window_x + 12, items_y[Brightness] + text_padding);
                    setBrightness(settings.brightness);
                    break;
                case Palette:
                    if (settings.palette == 0)
                        settings.palette = Ppu2C02::Palette::PaletteCount - 1;
                    else settings.palette--;
                    snprintf(palette_text, sizeof(palette_text), "Palette: %s",
                             palette_names[settings.palette]);
                    screen->fillRect(window_x + 10, items_y[Palette], window_w - 19, item_height,
                                     SELECTED_BG_COLOR);
                    drawText(items[Palette], window_x + 12, items_y[Palette] + text_padding);
                    break;
                default: break;
                }
                last_input_time = now;
            }

            if (isDownPressed(CONTROLLER::Right))
            {
                switch (select)
                {
                case Volume:
                    if (settings.volume <= 95) settings.volume += 5;
                    snprintf(volume_text, sizeof(volume_text), "Volume: %d%%", settings.volume);
                    screen->fillRect(window_x + 10, items_y[Volume], window_w - 19, item_height,
                                     SELECTED_BG_COLOR);
                    drawText(items[Volume], window_x + 12, items_y[Volume] + text_padding);
                    break;
                case Brightness:
                    if (settings.brightness <= 95) settings.brightness += 5;
                    snprintf(brightness_text, sizeof(brightness_text), "Brightness: %d%%",
                             settings.brightness);
                    screen->fillRect(window_x + 10, items_y[Brightness], window_w - 19, item_height,
                                     SELECTED_BG_COLOR);
                    drawText(items[Brightness], window_x + 12, items_y[Brightness] + text_padding);
                    setBrightness(settings.brightness);
                    break;
                case Palette:
                    settings.palette = (settings.palette + 1) % Ppu2C02::Palette::PaletteCount;
                    snprintf(palette_text, sizeof(palette_text), "Palette: %s",
                             palette_names[settings.palette]);
                    screen->fillRect(window_x + 10, items_y[Palette], window_w - 19, item_height,
                                     SELECTED_BG_COLOR);
                    drawText(items[Palette], window_x + 12, items_y[Palette] + text_padding);
                    break;
                default: break;
                }
                last_input_time = now;
            }

            if (isDownPressed(CONTROLLER::A))
            {
                switch (select)
                {
                case Back:
                    loadEmulatorSettings(nes);
                    saveSettings(&settings);
                    return;
                default: break;
                }
            }
        }

        // Update Selection
        if (prev_select != select)
        {
            int y;
            // Redraw old selection
            screen->fillRect(window_x + 10, items_y[prev_select], window_w - 19, item_height,
                             BAR_COLOR);
            y = items_y[prev_select] + text_padding;
            drawText(items[prev_select], window_x + 12, y);

            // Draw new selection
            screen->fillRect(window_x + 10, items_y[select], window_w - 19, item_height,
                             SELECTED_BG_COLOR);
            y = items_y[select] + text_padding;
            drawText(items[select], window_x + 12, y);
        }

        prev_select = select;
    }
}

void UI::initializeSettings()
{
    // HANDHELD: nastaveni jen v RAM (bez SD), po restartu vychozi hodnoty
    settings = Settings();
    // do not call setBrightness() here to allow for better control of the
    // backlight in the main code to hide glitchy visuals when drawing the screen
}

void UI::loadEmulatorSettings(Nes* nes)
{
    nes->setPalette(settings.palette);
    nes->setVolume(settings.volume);
}

void UI::restoreBrightness()
{
    if (runtime_config.backlight) setBrightness(settings.brightness);
}

void UI::setBrightness(int value)
{
    uint8_t pwm = ((value * 255) + 50) / 100;
    ledcWrite(TFT_BACKLIGHT_PIN, pwm);
}

void UI::saveSettings(const Settings* s)
{
    // HANDHELD: bez SD se neuklada
    (void)s;
}

void UI::loadSettings(Settings* s)
{
    *s = Settings();
}

void UI::drawText(const char* text, const int16_t x, const int16_t y)
{
    screen->setTextColor(TFT_BLACK);
    screen->setCursor(x, y);
    screen->print(text);
    screen->setCursor(x, y);
    screen->setTextColor(TEXT2_COLOR);
    screen->print(text[0]);
}

void UI::drawRomMode()
{
    const int16_t y = screen->height() - 12;
    const char* selectText = "Select";
    const char* mode1 = " RAM mode";
    const char* mode2 = " Flash mode";
    const char* currentMode;
    switch (settings.rom_backend)
    {
    case 0: currentMode = mode1; break;
    case 1: currentMode = mode2; break;
    default: currentMode = mode1; break;
    }

    const int16_t select_w = screen->textWidth(selectText);
    const int16_t mode_w = screen->textWidth(currentMode);
    const int16_t total_w = mode_w + select_w;
    const int16_t mode_x = screen->width() - total_w - 8;
    screen->setCursor(mode_x, y);

    screen->setTextColor(TEXT2_COLOR, BAR_COLOR);
    screen->print(selectText);

    screen->setTextColor(TFT_BLACK, BAR_COLOR);
    screen->print(currentMode);
}

// HANDHELD: test tlacitek pres controllerRead(), tedy s mapovanim z config.h.
// Ukonceni: Start + Select drzet 1 s.
void UI::buttonTest()
{
    struct Item
    {
        const char* name;
        CONTROLLER bit;
        int16_t x, y;
    };
    static const Item items[] = {
        { "UP", CONTROLLER::Up, 60, 70 },        { "DOWN", CONTROLLER::Down, 60, 150 },
        { "LEFT", CONTROLLER::Left, 5, 110 },    { "RIGHT", CONTROLLER::Right, 115, 110 },
        { "SEL", CONTROLLER::Select, 150, 180 }, { "START", CONTROLLER::Start, 210, 180 },
        { "B", CONTROLLER::B, 205, 120 },        { "A", CONTROLLER::A, 265, 90 },
    };
    constexpr int n = sizeof(items) / sizeof(items[0]);
    constexpr int w = 50, h = 28;
    uint16_t counts[n] = { 0 };

    screen->fillScreen(TFT_BLACK);
    screen->setTextDatum(TL_DATUM);
    screen->setTextColor(TFT_WHITE, TFT_BLACK);
    screen->drawString("Test tlacitek", 4, 4, 2);
    screen->drawString("Start + Select 1 s = zpet", 4, 22, 2);
    restoreBrightness();

    // Pockat na uvolneni A z vyberu v menu
    while (controllerRead()) delay(10);

    uint8_t prev = 0xFF;
    unsigned long combo_since = 0;
    while (true)
    {
        uint8_t s = controllerRead();
        if (s != prev)
        {
            for (int i = 0; i < n; i++)
            {
                bool on = s & (uint8_t)items[i].bit;
                bool was = (prev != 0xFF) && (prev & (uint8_t)items[i].bit);
                if (on && !was) counts[i]++;
                uint16_t bg = on ? TFT_GREEN : TFT_DARKGREY;
                uint16_t fg = on ? TFT_BLACK : TFT_WHITE;
                screen->fillRoundRect(items[i].x, items[i].y, w, h, 4, bg);
                screen->setTextColor(fg, bg);
                screen->setTextDatum(MC_DATUM);
                screen->drawString(items[i].name, items[i].x + w / 2, items[i].y + 9, 2);
                char c[8];
                snprintf(c, sizeof(c), "%u", counts[i]);
                screen->drawString(c, items[i].x + w / 2, items[i].y + 21, 1);
            }
            char mask[24];
            snprintf(mask, sizeof(mask), "stav 0x%02X   ", s);
            screen->setTextDatum(TL_DATUM);
            screen->setTextColor(TFT_WHITE, TFT_BLACK);
            screen->drawString(mask, 10, 215, 2);
            prev = s;
        }

        const uint8_t combo = (uint8_t)CONTROLLER::Start | (uint8_t)CONTROLLER::Select;
        if ((s & combo) == combo)
        {
            if (!combo_since) combo_since = millis();
            else if (millis() - combo_since > 1000) break;
        }
        else combo_since = 0;
        delay(10);
    }

    while (controllerRead()) delay(10);
    screen->setTextDatum(TL_DATUM);
}
