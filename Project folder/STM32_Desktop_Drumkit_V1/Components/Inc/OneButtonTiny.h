// -----
// OneButtonTiny.h - Library for detecting button clicks, doubleclicks and long
// press pattern on a single button.
// Adapted for STM32 HAL/CubeMX environment.
// User must configure the button GPIO as input mode externally.
// Copyright (c) by Matthias Hertel, https://www.mathertel.de/
// STM32 HAL adaptation by WilliTourt, willitourt@foxmail.com
// -----
// Original Author's website:
// http://www.mathertel.de This work is licensed under a BSD style license. See
// http://www.mathertel.de/License.aspx More information on:
// http://www.mathertel.de/Arduino
// -----
// Changelog:
// 01.12.2023 - Created from OneButtonTiny to support tiny environments.
// 09.22.2025 - Modified for STM32 HAL Driver by WilliTourt.
// 10.12.2025 - Added multi click support by WilliTourt.
// -----

#pragma once

#include "main.h"

// ----- Callback function types -----

extern "C" {
    typedef void (*callbackFunction)(void);
}


class OneButtonTiny {
	public:
		// ----- Constructor -----

		/**
		 * Initialize the OneButtonTiny library.
		 * @param pin The pin to be used for input from a momentary button.
		 * @param activeLow Set to true when the input level is LOW when the button is pressed, Default is true.
		 */
		explicit OneButtonTiny(GPIO_TypeDef* GPIOx, const uint16_t GPIO_Pin, const bool activeLow = true);

		/**
		 * set # millisec after safe click is assumed.
		 */
		void setDebounceMs(const unsigned int ms);

		/**
		 * set # millisec after single click is assumed.
		 */
		void setClickMs(const unsigned int ms);

		/**
		 * set # millisec after press is assumed.
		 */
		void setPressMs(const unsigned int ms);

		/**
		 * set # millisec after multi click is assumed.
		 */
		void setMultiClickMs(const unsigned int ms);

		// ----- Attach events functions -----

		/**
		 * Attach an event to be called when a single click is detected.
		 * @param newFunction This function will be called when the event has been detected.
		 */
		void attachClick(callbackFunction newFunction);

		/**
		 * Attach an event to be called after a double click is detected.
		 * @param newFunction This function will be called when the event has been detected.
		 */
		void attachDoubleClick(callbackFunction newFunction);

		/**
		 * Attach an event to be called after a multi click is detected.
		 * @param newFunction This function will be called when the event has been detected.
		 */
		void attachMultiClick(callbackFunction newFunction);

		/**
		 * Attach an event to fire when the button is pressed and held down.
		 * @param newFunction
		 */
		void attachLongPressStart(callbackFunction newFunction);


		// ----- State machine functions -----

		/**
		 * @brief Call this function every some milliseconds for checking the input
		 * level at the initialized digital pin.
		 */
		void tick(void);

		/**
		 * @brief Call this function every time the input level has changed.
		 * Using this function no digital input pin is checked because the current
		 * level is given by the parameter.
		 * Run the finite state machine (FSM) using the given level.
		 */
		void tick(bool level);


		/**
		 * Reset the button state machine.
		 */
		void reset(void);


		/**
		 * @return true if we are currently handling button press flow
		 * (This allows power sensitive applications to know when it is safe to power down the main CPU)
		 */
		bool isIdle() const {
			return _state == OCS_INIT;
		}


	private:
		GPIO_TypeDef* _gpio_port = nullptr; 	// hardware port.
		uint16_t _gpio_pin = -1;            	// hardware pin number.
		uint16_t _debounce_ms = 50;  			// number of msecs for debounce times.
		uint16_t _click_ms = 400;    			// number of msecs before a click is detected.
	uint16_t _press_ms = 800;    			// number of msecs before a long button press is detected
	uint16_t _multiClick_ms = 600;       // number of msecs before a multi click is detected

		GPIO_PinState _buttonPressed = GPIO_PIN_RESET;  
		// this is the level of the input pin when the button is pressed.
		// RESET if the button connects the input pin to GND when pressed.
		// SET if the button connects the input pin to VCC when pressed.

		// These variables will hold functions acting as event source.
		callbackFunction _clickFunc = NULL;
		callbackFunction _doubleClickFunc = NULL;
		callbackFunction _multiClickFunc = NULL;
		callbackFunction _longPressStartFunc = NULL;

		// These variables that hold information across the upcoming tick calls.
		// They are initialized once on program start and are updated every time the
		// tick function is called.

		// define FiniteStateMachine
		enum stateMachine_t : int {
			OCS_INIT = 0,
			OCS_DOWN = 1,  // button is down
			OCS_UP = 2,    // button is up
			OCS_COUNT = 3,
			OCS_PRESS = 6,  // button is hold down
			OCS_PRESSEND = 7,
		};

		/**
		 * Run the finite state machine (FSM) using the given level.
		 */
		void _fsm(bool activeLevel);

		/**
		 *  Advance to a new state.
		 */
		void _newState(stateMachine_t nextState);

		stateMachine_t _state = OCS_INIT;

		int debouncedPinLevel = -1;
		int _lastDebouncePinLevel = -1;     // used for pin debouncing
		uint32_t _lastDebounceTime = 0;  	// HAL_GetTick()
		uint32_t now = 0;                	// HAL_GetTick()

		uint32_t _startTime = 0;  			// start of current input change to checking debouncing
		int _nClicks = 0;              		// count the number of clicks with this variable

	public:
		int pin() const {
			return _gpio_pin;
		};
		stateMachine_t state() const {
			return _state;
		};
		int debounce(const int value);
		int debouncedValue() const {
			return debouncedPinLevel;
		};
};
