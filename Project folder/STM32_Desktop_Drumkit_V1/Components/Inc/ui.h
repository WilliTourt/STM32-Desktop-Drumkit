
/**
 * @file ui.h
 * @brief User interface management for drumkit
 * 
 * This file defines the UI class for handling display and user interaction,
 * including menu navigation, page display and status updates.
 * 
 * @author WilliTourt, willitourt@foxmail.com
 * @copyright Copyright (c) 2025 by WilliTourt
 * 
 * @note Comments are mostly written by AI
 */

#pragma once

#include "cpp_main.h"
#include "pad.h"
#include "midi.h"
#include "OneButtonTiny.h"

#include "oled.h"
extern "C" {
#include "oled_menu.h"
}

/**
 * @brief UI management class for drumkit
 * 
 * Handles display output and user input through:
 * - Menu navigation system
 * - Page display management
 * - Button input handling
 * - Status monitoring and updates
 */
class UI {
    public:
        /**
         * @brief Display operation modes
         */
        enum class DisplayMode {
            PAGE,   ///< Real-time page display mode
            MENU    ///< Menu navigation mode (pads disabled)
        };

        /**
         * @brief Available display pages
         */
        enum class Page {
            MAIN,         ///< Main status page
            PAD_TEST,     ///< Pad test page
            PAD_SETTING,  ///< Pad configuration page
            STATS         ///< Statistics page
        };

        /**
         * @brief Construct a new UI object
         */
        UI();

        /**
         * @brief Check power status
         * @return true if powered on, false otherwise
         */
        inline bool chkPower() { return _isPowerOn; }

        /**
         * @brief Set power status
         * @param isOn true to power on, false to power off
         */
        inline void setPower(bool isOn) { _isPowerOn = isOn; }

        /**
         * @brief Initialize button with timing parameters
         * @param debounce_ms Debounce time in milliseconds
         * @param singleclick_maxMs Maximum time for single click
         * @param longPress_minMs Minimum time for long press
         * @param multiClick_minMs Minimum time between multi-clicks
         */
        void buttonInit(uint16_t debounce_ms, uint16_t singleclick_maxMs, 
                      uint16_t longPress_minMs, uint16_t multiClick_minMs);

        /**
         * @brief Process button input
         * Should be called periodically
         */
        void buttonTick();

        /**
         * @brief Get current display mode
         * @return DisplayMode Current display mode
         */
        inline DisplayMode getMode() { return _mode; }

        /**
         * @brief Set display mode
         * @param mode New display mode
         */
        inline void setMode(DisplayMode mode) { _mode = mode; }

        /**
         * @brief Set current display page
         * @param page Page to display
         */
        inline void setPage(Page page) { _page = page; }

        /**
         * @brief Initialize UI components
         */
        void init();

        /**
         * @brief Update display
         * Should be called periodically
         */
        void update();

        /**
         * @brief Show welcome screen
         */
        void welcome();

        /**
         * @brief Update pad hit statistics
         * @param padID Pad identifier
         * @param hits Number of hits to add
         */
        void updatePadStats(Pad::PadID padID, uint32_t hits);

        /**
         * @brief Update MIDI statistics
         */
        void updateMidiStats();

        /**
         * @brief Update MIDI connection status
         * @param connected true if connected, false otherwise
         */
        void updateMidiConn(bool connected);

        // Pointers to menus
        // Menutypedef* _currentMenu; // This is managed by oled-menu internally
        Menutypedef* _mainMenu;
        Menutypedef* _settingsMenu;
        Menutypedef* _statsMenu;
        Menutypedef* _aboutMenu;

    private:
        OneButtonTiny _button = OneButtonTiny(KEY_PRESS_GPIO_Port, KEY_PRESS_Pin, true);
        OLED _oled = OLED(&hi2c1, 32); // OLED instance to draw page content

        volatile bool _isPowerOn;

        bool _midiConnected;
        bool _ledEffectsEnabled;
        bool _buzzerEnabled;
        bool _debugLogEnabled;

        DisplayMode _mode;
        DisplayMode _prevMode;
        Page _page;

        // Pad数据
        const Pad** _pads;

        // Stats
        uint32_t _totalHits[Pad::PAD_NUM];
        uint32_t _totalHitsAll;
        // uint32_t _sumForce[Pad::PAD_NUM];
        // uint8_t _avgForce[Pad::PAD_NUM];
        uint32_t _totalDataSent;

        // Pad Settings (not implemented yet)
        uint8_t _selectedPadID;
        Pad::ForceMappingCurve _padCurves[Pad::PAD_NUM];
        uint16_t _padThresholds[Pad::PAD_NUM];
        uint16_t _padUpperLimits[Pad::PAD_NUM];

        void _show();

        void _initMenuPointers();
        void _createMainMenu();
        void _createSettingsMenu();
        void _createAboutMenu();

        void _showMainPage();
        void _showPadTestPage();
        void _showPadSettingPage();
        void _showStatsPage();

        friend void Callback_HomeMenuItem();
        friend void Callback_PadTestMenuItem();
        friend void Callback_PadSettingMenuItem();
        friend void Callback_StatsMenuItem();
        friend void Callback_PWROFF();

};

extern UI ui;
