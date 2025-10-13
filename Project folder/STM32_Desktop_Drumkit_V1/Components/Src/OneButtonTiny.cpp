/**
 * @file OneButtoTiny.cpp
 *
 * @brief Library for detecting button clicks, doubleclicks and long press
 * pattern on a single button.
 *
 * @author Matthias Hertel, https://www.mathertel.de
 * STM32 HAL adaptation by WilliTourt, willitourt@foxmail.com
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 *                          Ihor Nehrutsa, Ihor.Nehrutsa@gmail.com
 *
 * This work is licensed under a BSD style license. See
 * http://www.mathertel.de/License.aspx
 *
 * More information on: https://www.mathertel.de/Arduino/OneButtonLibrary.aspx
 *
 * Changelog: see OneButtonTiny.h
 */

#include "OneButtonTiny.h"

// ----- Initialization and Default Values -----

/**
 * Initialize the OneButton library.
 * @param pin The pin to be used for input from a momentary button.
 * @param activeLow Set to true when the input level is RESET when the button is pressed, Default is true.
 * @param pullupActive Activate the internal pullup when available. Default is true.
 */
OneButtonTiny::OneButtonTiny(GPIO_TypeDef* GPIOx, const uint16_t GPIO_Pin, const bool activeLow) {
	_gpio_port = GPIOx;
	_gpio_pin = GPIO_Pin;

	if (activeLow) {
		// the button connects the input pin to GND when pressed.
		_buttonPressed = GPIO_PIN_RESET;

	} else {
		// the button connects the input pin to VCC when pressed.
		_buttonPressed = GPIO_PIN_SET;
	}
}  // OneButton


// explicitly set the number of millisec that have to pass by before a click is assumed stable.
void OneButtonTiny::setDebounceMs(const unsigned int ms) {
	_debounce_ms = ms;
}  // setDebounceMs


// explicitly set the number of millisec that have to pass by before a click is detected.
void OneButtonTiny::setClickMs(const unsigned int ms) {
	_click_ms = ms;
}  // setClickMs


void OneButtonTiny::setPressMs(const unsigned int ms) {
	_press_ms = ms;
}  // setPressMs

// explicitly set the number of millisec that have to pass by before a multi click is detected.
void OneButtonTiny::setMultiClickMs(const unsigned int ms) {
	_multiClick_ms = ms;
}  // setMultiClickMs


// save function for click event
void OneButtonTiny::attachClick(callbackFunction newFunction) {
	_clickFunc = newFunction;
}  // attachClick


// save function for doubleClick event
void OneButtonTiny::attachDoubleClick(callbackFunction newFunction) {
	_doubleClickFunc = newFunction;
}  // attachDoubleClick


void OneButtonTiny::attachMultiClick(callbackFunction newFunction) {
	_multiClickFunc = newFunction;
}  // attachMultiClick

// save function for longPressStart event
void OneButtonTiny::attachLongPressStart(callbackFunction newFunction) {
	_longPressStartFunc = newFunction;
}  // attachLongPressStart


void OneButtonTiny::reset(void) {
	_state = OneButtonTiny::OCS_INIT;
	_nClicks = 0;
	_startTime = 0;
}


/**
 * @brief Debounce input pin level for use in SpecialInput.
 */
int OneButtonTiny::debounce(const int value) {
	now = HAL_GetTick();  // current (relative) time in msecs.
	if (_lastDebouncePinLevel == value) {
		if (now - _lastDebounceTime >= _debounce_ms)
		debouncedPinLevel = value;
	} else {
		_lastDebounceTime = now;
		_lastDebouncePinLevel = value;
	}
	return debouncedPinLevel;
};


/**
 * @brief Check input of the configured pin,
 * debounce input pin level and then
 * advance the finite state machine (FSM).
 */
void OneButtonTiny::tick(void) {
	if (_gpio_pin >= 0) {
		_fsm(debounce(HAL_GPIO_ReadPin(_gpio_port, _gpio_pin)) == _buttonPressed);
	}
}  // tick()


void OneButtonTiny::tick(bool activeLevel) {
	_fsm(debounce(activeLevel));
}


/**
 *  @brief Advance to a new state and save the last one to come back in cas of bouncing detection.
 */
void OneButtonTiny::_newState(stateMachine_t nextState) {
	_state = nextState;
}  // _newState()


/**
 * @brief Run the finite state machine (FSM) using the given level.
 */
void OneButtonTiny::_fsm(bool activeLevel) {
	unsigned long waitTime = (now - _startTime);

	// Implementation of the state machine
	switch (_state) {
		case OneButtonTiny::OCS_INIT:
		// waiting for level to become active.
		if (activeLevel) {
			_newState(OneButtonTiny::OCS_DOWN);
			_startTime = now;  // remember starting time
			_nClicks = 0;
		}  // if
		break;

		case OneButtonTiny::OCS_DOWN:
		// waiting for level to become inactive.

		if (!activeLevel) {
			_newState(OneButtonTiny::OCS_UP);
			_startTime = now;  // remember starting time

		} else if ((activeLevel) && (waitTime > _press_ms)) {
			if (_longPressStartFunc) _longPressStartFunc();
			_newState(OneButtonTiny::OCS_PRESS);
		}  // if
		break;

		case OneButtonTiny::OCS_UP:
		// level is inactive

		// count as a short button down
		_nClicks++;
		_newState(OneButtonTiny::OCS_COUNT);
		break;

		case OneButtonTiny::OCS_COUNT:
		// debounce time is over, count clicks

		if (activeLevel) {
			// button is down again
			_newState(OneButtonTiny::OCS_DOWN);
			_startTime = now;  // remember starting time

		} else if ((_nClicks > 2 && waitTime >= _multiClick_ms) || (_nClicks <= 2 && waitTime >= _click_ms)) {
			// now we know how many clicks have been made.

			if (_nClicks == 1) {
				// 单击
				if (_clickFunc) _clickFunc();
			} else if (_nClicks == 2) {
				// 双击
				if (_doubleClickFunc) _doubleClickFunc();
			} else if (_nClicks > 2) {
				// 多击
				if (_multiClickFunc) _multiClickFunc();
			}
			reset();
		}
		break;

		case OneButtonTiny::OCS_PRESS:
		// waiting for pin being release after long press.

		if (!activeLevel) {
			_newState(OneButtonTiny::OCS_PRESSEND);
			_startTime = now;
		}  // if
		break;

		case OneButtonTiny::OCS_PRESSEND:
		// button was released.
		reset();
		break;

		default:
		// unknown state detected -> reset state machine
		_newState(OneButtonTiny::OCS_INIT);
		break;
	}  // if

}  // OneButton.tick()


// end.
