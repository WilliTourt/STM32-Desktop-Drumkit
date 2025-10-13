/**
 * @file ui.cpp
 * @brief User interface implementation for drumkit
 * 
 * This file implements the UI class methods for display and user interaction,
 * including menu navigation, page display and status updates.
 * 
 * @author WilliTourt, willitourt@foxmail.com
 * @copyright Copyright (c) 2025 by WilliTourt
 * 
 * @note Comments are mostly written by AI
 */

#include "ui.h"

UI ui; // Global UI instance

/**
 * @brief Callback for Home menu item
 * 
 * Switches to MAIN page display mode
 */


void Callback_HomeMenuItem() {
    ui.setMode(UI::DisplayMode::PAGE);
    ui.setPage(UI::Page::MAIN);
    ui._oled.clear();
}

/**
 * @brief Callback for Pad Test menu item
 * 
 * Switches to PAD_TEST page display mode
 */
void Callback_PadTestMenuItem() {
    ui.setMode(UI::DisplayMode::PAGE);
    ui.setPage(UI::Page::PAD_TEST);
    ui._oled.clear();
}

/**
 * @brief Callback for Pad Setting menu item
 * 
 * Switches to PAD_SETTING page display mode
 */
void Callback_PadSettingMenuItem() {
    ui.setMode(UI::DisplayMode::PAGE);
    ui.setPage(UI::Page::PAD_SETTING);
    ui._oled.clear();
}

/**
 * @brief Callback for Statistics menu item
 * 
 * Switches to STATS page display mode
 */
void Callback_StatsMenuItem() {
    ui.setMode(UI::DisplayMode::PAGE);
    ui.setPage(UI::Page::STATS);
    ui._oled.clear();
}

/**
 * @brief Callback for Power Off menu item
 * 
 * Turns off OLED display and sets power state to off
 */
void Callback_PWROFF() {
    ui._oled.clear();
    ui._oled.power(false);
    for (uint8_t i = 0; i < 10; i++) {
        HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
        HAL_Delay(100);
    }
    ui.setPower(false);
}



// Single click callback - handle menu right move
void Callback_ButtonSingleClick() {
    if (ui.getMode() == UI::DisplayMode::MENU) {
        Menu_MoveRight();
        DBG("Menu Right.\r\n");
    }
}

// Double click callback - handle menu left move
void Callback_ButtonDoubleClick() {
    if (ui.getMode() == UI::DisplayMode::MENU) {
        Menu_MoveLeft();
        DBG("Menu Left.\r\n");
    }
}

// Multi click callback - handle menu go back
void Callback_ButtonMultiClick() {
    if (ui.getMode() == UI::DisplayMode::MENU) {
        if (!Menu_GoBack()) {
            Callback_HomeMenuItem();
        }
        DBG("Menu Go Back.\r\n");
    }
}

// Long press start callback - handle power on/off and menu confirm
void Callback_ButtonLongPressStart() {
    if (!ui.chkPower()) {
        HAL_GPIO_WritePin(PWR_EN_GPIO_Port, PWR_EN_Pin, GPIO_PIN_SET); // Power on
        ui.setPower(true);
        DBG("Power has set.\r\n");
    } else {
        if (ui.getMode() == UI::DisplayMode::PAGE) {
            ui.setMode(UI::DisplayMode::MENU);
            DBG("Page -> Menu.\r\n");
        } else {
            Menu_Confirm();
            DBG("Menu Confirm.\r\n");
        }
    }
}



UI::UI() :
    _isPowerOn(false),
    _midiConnected(false),
    _ledEffectsEnabled(false),
    _buzzerEnabled(true),
    _debugLogEnabled(false),
    _mode(DisplayMode::PAGE),
    _prevMode(DisplayMode::PAGE),
    _page(Page::MAIN),
    _pads(nullptr),
    _totalHitsAll(0),
    _totalDataSent(0),
    _selectedPadID(0) {
    memset(_totalHits, 0, sizeof(_totalHits));
}

/**
 * @brief Update pad hit statistics
 * @param padID Pad identifier
 * @param hits Number of hits to add
 */
void UI::updatePadStats(Pad::PadID padID, uint32_t hits) {
    if (padID < Pad::PAD_NUM) {
        _totalHits[padID] += hits;
        _totalHitsAll += hits;
    }
}

/**
 * @brief Update MIDI statistics
 * 
 * Increments total MIDI data sent counter
 */
void UI::updateMidiStats() {
    _totalDataSent += 5;
}

/**
 * @brief Update MIDI connection status
 * @param connected true if connected, false otherwise
 */
void UI::updateMidiConn(bool connected) {
    _midiConnected = connected;
}

/**
 * @brief Initialize button with timing parameters
 * @param debounce_ms Debounce time in milliseconds
 * @param singleclick_maxMs Maximum time for single click
 * @param longPress_minMs Minimum time for long press
 * @param multiClick_minMs Minimum time between multi-clicks
 */
void UI::buttonInit(uint16_t debounce_ms, uint16_t singleclick_maxMs, uint16_t longPress_minMs, uint16_t multiClick_minMs) {
    _button.setDebounceMs(debounce_ms);
    _button.setClickMs(singleclick_maxMs);
    _button.setPressMs(longPress_minMs);
    _button.setMultiClickMs(multiClick_minMs);
	
	// Bind menu control callbacks
	_button.attachClick(Callback_ButtonSingleClick);
	_button.attachDoubleClick(Callback_ButtonDoubleClick);
    _button.attachMultiClick(Callback_ButtonMultiClick);
	_button.attachLongPressStart(Callback_ButtonLongPressStart);
}

/**
 * @brief Process button input
 * 
 * Should be called periodically to handle button events
 */
void UI::buttonTick() {
    _button.tick();
}

/**
 * @brief Initialize UI components
 * 
 * Sets up menu system, initializes OLED display and configures default settings
 */
void UI::init() {
    // Initialize stats
    for (uint8_t i = 0; i < Pad::PAD_NUM; i++) {
        _totalHits[i] = 0;
        _padCurves[i] = Pad::CURVE_LINEAR; // Default curve
        _padThresholds[i] = ADC_PAD_HIT_DEFAULT_THRESHOLD; // Default threshold
        _padUpperLimits[i] = ADC_PAD_DEFAULT_UPPER_LIMIT; // Default upper limit
    }

    // Create menu items
    _initMenuPointers();
    _createSettingsMenu();
    _createAboutMenu();
    _createMainMenu();
    currentMenu = _mainMenu;

    // Initialize menu system
    OLEDUI_Init();
}

/**
 * @brief Update display
 * 
 * Handles button input and refreshes display based on current mode
 */
void UI::update() {
    if (!_isPowerOn) return;
    buttonTick();
    _show();
}

void UI::_show() {

    // Clear screen when display mode changes
    if (_mode != _prevMode) {
        _oled.clear();
        _prevMode = _mode;
    }

    // Show different content based on mode
    if (_mode == DisplayMode::MENU) {
        OLEDUI_Update();
		OLEDUI_Move();
		OLEDUI_Show();
    } else if (_mode == DisplayMode::PAGE) {
        static uint32_t lastUpdateTime = 0;
        if (HAL_GetTick() - lastUpdateTime > 500) {
            lastUpdateTime = HAL_GetTick();
            switch (_page) {
                case Page::MAIN:
                    _showMainPage();
                    break;
                case Page::PAD_TEST:
                    _showPadTestPage();
                    break;
                case Page::PAD_SETTING:
                    _showPadSettingPage();
                    break;
                case Page::STATS:
                    _showStatsPage();
                    break;
            }
        }
    }
}


// Below are menu creation and page display helper functions

void UI::_initMenuPointers() {
    _mainMenu = AddMenu("Main Menu", NULL, 0, NULL);
    _settingsMenu = AddMenu("Settings", NULL, 0, _mainMenu);
    _aboutMenu = AddMenu("About", NULL, 0, _mainMenu);
}

void UI::_createMainMenu() {
    AddMenuItem(_mainMenu, "1 Home", Callback_HomeMenuItem, NULL, NONE_CTRL, NULL);
    AddMenuItem(_mainMenu, "2 Settings", FunctionForNextMenu, _settingsMenu, NONE_CTRL, NULL);
    AddMenuItem(_mainMenu, "3 Pad Test", Callback_PadTestMenuItem, NULL, NONE_CTRL, NULL);
    AddMenuItem(_mainMenu, "4 Statistics", Callback_StatsMenuItem, NULL, NONE_CTRL, NULL);
    AddMenuItem(_mainMenu, "5 About", FunctionForNextMenu, _aboutMenu, NONE_CTRL, NULL);
    AddMenuItem(_mainMenu, "> POWER OFF!", Callback_PWROFF, NULL, NONE_CTRL, NULL);
}

void UI::_createSettingsMenu() {
    AddMenuItem(_settingsMenu, "1 Pad Settings", Callback_PadSettingMenuItem, NULL, NONE_CTRL, NULL);
    AddMenuItem(_settingsMenu, "2 LED Effects(NA", FunctionForCtrl, NULL, SWITCH_CTRL, (int*)&_ledEffectsEnabled);
    AddMenuItem(_settingsMenu, "3 Buzzer", FunctionForCtrl, NULL, SWITCH_CTRL, (int*)&_buzzerEnabled);
    AddMenuItem(_settingsMenu, "4 Debug Log", FunctionForCtrl, NULL, SWITCH_CTRL, (int*)&_debugLogEnabled);
}

void UI::_createAboutMenu() {
    AddMenuItem(_aboutMenu, "STM32 DesktopDrumkit", NULL, NULL, NONE_CTRL, NULL);
    AddMenuItem(_aboutMenu, "     - Version 1.0.0", NULL, NULL, NONE_CTRL, NULL);
    AddMenuItem(_aboutMenu, "Author: WilliTourt", NULL, NULL, NONE_CTRL, NULL);
    AddMenuItem(_aboutMenu, "Email: willitourt@", NULL, NULL, NONE_CTRL, NULL);
    AddMenuItem(_aboutMenu, "       foxmail.com", NULL, NULL, NONE_CTRL, NULL);
    AddMenuItem(_aboutMenu, "Credits to:", NULL, NULL, NONE_CTRL, NULL);
    AddMenuItem(_aboutMenu, "- 'lgykl'", NULL, NULL, NONE_CTRL, NULL);
    AddMenuItem(_aboutMenu, "  for oled menu libs", NULL, NULL, NONE_CTRL, NULL);
    AddMenuItem(_aboutMenu, "- 'Matthias Hertel'", NULL, NULL, NONE_CTRL, NULL);
    AddMenuItem(_aboutMenu, "  for button libs", NULL, NULL, NONE_CTRL, NULL);
    AddMenuItem(_aboutMenu, "- 'Victor2805'", NULL, NULL, NONE_CTRL, NULL);
    AddMenuItem(_aboutMenu, "  for inspiration ;)", NULL, NULL, NONE_CTRL, NULL);
    AddMenuItem(_aboutMenu, "- 'WoodBreeze'", NULL, NULL, NONE_CTRL, NULL);
    AddMenuItem(_aboutMenu, "  for proto. advice", NULL, NULL, NONE_CTRL, NULL);  
    AddMenuItem(_aboutMenu, "- STMicroelectronics", NULL, NULL, NONE_CTRL, NULL);
    AddMenuItem(_aboutMenu, "  for platform sup.", NULL, NULL, NONE_CTRL, NULL);
    AddMenuItem(_aboutMenu, "- And all related", NULL, NULL, NONE_CTRL, NULL);
    AddMenuItem(_aboutMenu, "  communities", NULL, NULL, NONE_CTRL, NULL);
}

// void UI::_createMainMenu() {
//     ItemTypedef mainMenuItems[] = {
//         { (char*)"Home", 4,
//             Callback_HomeMenuItem,
//             NULL,
//             NULL
//         },
//         { (char*)"Pad Test", 8,
//             Callback_PadTestMenuItem,
//             NULL,
//             NULL
//         },
//         { (char*)"Settings", 8,
//             NULL,
//             _settingsMenu,
//             NULL
//         },
//         { (char*)"Statistics", 10,
//             Callback_StatsMenuItem,
//             NULL,
//             NULL
//         },
//         { (char*)"About", 5,
//             NULL,
//             _aboutMenu,
//             NULL
//         }
//     };

//     _mainMenu = AddMenu("Main Menu", mainMenuItems, sizeof(mainMenuItems)/sizeof(ItemTypedef), NULL);
// }

// void UI::_createSettingsMenu() {
//     ControlTypedef ledEffects = { (int*)&_ledEffectsEnabled, SWITCH_CTRL };
//     ControlTypedef buzzerCtrl = { (int*)&_buzzerEnabled, SWITCH_CTRL };
//     ControlTypedef debugLogCtrl = { (int*)&_debugLogEnabled, SWITCH_CTRL };

//     ItemTypedef settingsMenuItems[] = {
//         { (char*)"Pad Settings", 12,
//             Callback_PadSettingMenuItem,
//             NULL,
//             NULL
//         },
//         { (char*)"LED Effects", 11,
//             NULL,
//             NULL,
//             &ledEffects
//         },
//         { (char*)"Buzzer", 6,
//             NULL,
//             NULL,
//             &buzzerCtrl
//         },
//         { (char*)"Debug Log", 9,
//             NULL,
//             NULL,
//             &debugLogCtrl
//         }
//     };

//     _settingsMenu = AddMenu("Settings", settingsMenuItems, sizeof(settingsMenuItems)/sizeof(ItemTypedef), _mainMenu);
// }

// void UI::_createAboutMenu() {
//     ItemTypedef aboutMenuItems[] = {
//         { (char*)"STM32 DesktopDrumkit", 20,
//             NULL,
//             NULL,
//             NULL
//         },
//         { (char*)"Version 1.0", 11,
//             NULL,
//             NULL,
//             NULL
//         },
//         { (char*)"Author: WilliTourt", 18,
//             NULL,
//             NULL,
//             NULL
//         },
//         { (char*)"willitourt@", 11,
//             NULL,
//             NULL,
//             NULL
//         },
//         { (char*)"foxmail.com", 11,
//             NULL,
//             NULL,
//             NULL
//         },
//         { (char*)"Credits to:", 11,
//             NULL,
//             NULL,
//             NULL
//         }
//     };

//     _aboutMenu = AddMenu("About", aboutMenuItems, sizeof(aboutMenuItems)/sizeof(ItemTypedef), _mainMenu);
// }

void UI::welcome() {
    _oled.printText(0, 0, "STM32 Desktop Drumkit", 8);
    _oled.printText(0, 1, "> Initializing...", 8);
    _oled.printText(36, 2, "WELCOME!", 16);
    // _oled.printImage(0, 0, 40, 32, _oled.ST);
    HAL_Delay(2000);
    _oled.clear();
}

void UI::_showMainPage() {
    static uint8_t currentInfo = 4;
    static uint32_t lastSwitchTime = 0;
    static bool shouldUpdate = true;
    
    // 每2秒切换到下一个信息
    if (HAL_GetTick() - lastSwitchTime > 3000) {
        currentInfo = (currentInfo + 1) % 5;
        lastSwitchTime = HAL_GetTick();
        shouldUpdate = true;
    }

    _oled.printText(0, 0, "> Home               ", 8);
    _oled.printText(0, 1, "MIDI: ", 8);
    _oled.printText(36, 1, _midiConnected ? "READY!        " : "Disconnected  ", 8);

    if (!shouldUpdate) return;

    switch (currentInfo) {
        case 0:
            _oled.printText(0, 2, "Short press:         ", 8);
            _oled.printText(0, 3, "Next item in menu    ", 8);
            break;
        case 1:
            _oled.printText(0, 2, "Double press:        ", 8);
            _oled.printText(0, 3, "Previous item in menu", 8);
            break;
        case 2:
            _oled.printText(0, 2, "Triple press:        ", 8);
            _oled.printText(0, 3, "Go back in menu      ", 8);
            break;
        case 3:
            _oled.printText(0, 2, "Long press 1 sec:    ", 8);
            _oled.printText(0, 3, "Go to menu or Confirm", 8);
            break;
        case 4:
            _oled.printText(0, 2, "Long press 3 sec:    ", 8);
            _oled.printText(0, 3, "Power on/off         ", 8);
    }

    shouldUpdate = false;
}


void UI::_showPadTestPage() {
    _oled.printText(0, 0, "> Pad Test           ", 8);
    _oled.printText(0, 1, "                     ", 8);
    _oled.printText(0, 2, "WillBeAddedSoon", 16);
}

void UI::_showPadSettingPage() {
    _oled.printText(0, 0, "> Pad Settings       ", 8);
    _oled.printText(0, 1, "                     ", 8);
    _oled.printText(0, 2, "WillBeAddedSoon", 16);
    // char buf[32];
    // snprintf(buf, sizeof(buf), "Pad %d", _selectedPadID);
    // _oled.printText(0, 1, buf, 8);
}

void UI::_showStatsPage() {
    static uint8_t currentPad = 0;
    static uint32_t lastSwitchTime = 0;

    _oled.printText(0, 0, "> Statistics         ", 8);
    _oled.printText(0, 1, "Total Hits:          ", 8);
    _oled.printText(0, 2, "MIDI Data:          B", 8);
    
    if (HAL_GetTick() - lastSwitchTime > 2000) {
        _oled.clearPart(0, 3, 127, 4);
        currentPad = (currentPad + 1) % Pad::PAD_NUM;
        lastSwitchTime = HAL_GetTick();
    }
    char buf[24];
    snprintf(buf, sizeof(buf), "%s Hits:", Pad::ID2Str((Pad::PadID)currentPad));
    _oled.printText(0, 3, buf, 8);

    _oled.printVar(90, 1, _totalHitsAll, "int", 4, false);
    _oled.printVar(84, 2, _totalDataSent, "int", 5, false);
    _oled.printVar(90, 3, _totalHits[currentPad], "int", 4, false);

    // snprintf(buf, sizeof(buf), "%s Force: %d", 
    //         Pad::ID2Str((Pad::PadID)currentPad),
    //         _avgForce[currentPad]);
    // _oled.printText(0, 3, buf, 8);
}
