/**
 * @file pad.cpp
 * @brief Implementation of Pad class for drum pad input processing
 * 
 * This file implements the Pad class methods for handling drum pad inputs,
 * including hit detection, force measurement, MIDI velocity mapping and controlling output pins.
 * 
 * @author WilliTourt, willitourt@foxmail.com
 * @copyright Copyright (c) 2025 by WilliTourt
 * 
 * @note Comments are mostly written by AI
 */

#include "pad.h"
#include "math.h"

/**
 * @brief Static ADC buffers initialization
 */
uint16_t Pad::adc1_buf[ADC1_PAD_NUMS] = { 0 };
uint16_t Pad::adc2_buf[ADC2_PAD_NUMS] = { 0 };
uint16_t Pad::adc3_buf[ADC3_PAD_NUMS] = { 0 };

/**
 * @brief Convert PadID to string
 * @param id Pad identifier
 * @return const char* String representation of the PadID
 */
const char* Pad::ID2Str(PadID id) {
    switch (id) {
        case OpenHiHat:   return "OpHiHat";
        case CloseHiHat:  return "ClHiHat";
        case Crash:       return "Crash";
        case Ride:        return "Ride";
        case SideStick:   return "SSTK";
        case Kick:        return "Kick";
        case Snare:       return "Snare";
        case MidTom:      return "MidTom";
        case LowTom:      return "LowTom";
        case HighTom:     return "HighTom";
        case PAD_NUM:     return "PAD_NUM";
        default:          return "Unknown";
    }
}

/**
 * @brief Construct a new Pad object
 * @param piezo_adc_group ADC group the pad is connected to
 * @param piezo_adc_index Index of the pad in the ADC group
 * @param out_port GPIO port for output pin
 * @param out_pin GPIO pin number for output
 * @param pad_id Identifier for the pad (Used for MIDI mapping)
 * @param hit_threshold Threshold value for hit detection
 * @param upper_limit Upper limit for ADC readings
 * @param force_curve Force mapping curve type
 */
Pad::Pad(ADCGroup piezo_adc_group, uint8_t piezo_adc_index, GPIO_TypeDef* out_port, uint16_t out_pin,
    PadID pad_id,
    uint16_t hit_threshold, uint16_t upper_limit, ForceMappingCurve force_curve) :
    _piezo_adc_group(piezo_adc_group),
    _piezo_adc_index(piezo_adc_index),
    _out_port(out_port),
    _out_pin(out_pin),
    _force(0),
    _force_curve(force_curve),
    _last_trigger_state(false),
    _pad_id(pad_id),
    _hit_threshold(hit_threshold),
    _upper_limit(upper_limit),
    _peak_val(0),
    _adc_measuring(false),
    _measurement_cplt(false),
    _adc_measuring_start_time(0),
    _last_adc_measuring_state(0) {}

/**
 * @brief Detect hit and start force measurement window (call in main loop)
 * 
 * This method detects new hits and initializes the measurement window.
 * Must be called before isTriggered() for proper operation.
 */
void Pad::detectHit() {
    bool current_hit = _isHit();
    
    if (current_hit && !_adc_measuring) {
        _adc_measuring = true;
        _peak_val = 0;
    }
}

/**
 * @brief Check if the pad is currently triggered
 * @return true if a NEW hit is detected, false otherwise
 * 
 * Compares current hit state with previous state to detect new triggers.
 * Must be called after detectHit() for proper operation.
 */
bool Pad::isTriggered() {
    bool current_hit = _isHit();
    bool triggered = (current_hit && !_last_trigger_state);
    _last_trigger_state = current_hit;
    return triggered;
}

/**
 * @brief Complete force measurement and calculate final value
 * 
 * This method:
 * 1. Tracks peak ADC value during measurement window
 * 2. Maps peak value to force (0-127) when window ends
 * 3. Resets measurement state when done
 * 
 * Should be called after isTriggered() if a new hit was detected.
 */
void Pad::measureForce() {
    uint32_t now = HAL_GetTick();
    
    if (_adc_measuring) {
        // Update timestamp at start of measurement
        if (_last_adc_measuring_state != _adc_measuring) {
            _adc_measuring_start_time = now;
        }

        // // Skip readings in peak area
        // uint16_t val = _getADCVal();
        // if (val < 3800) {
        //     _peak_val = ((_peak_val > val) ? _peak_val : val);
        // }

        uint16_t val = _getADCVal();
        _peak_val = ((_peak_val > val) ? _peak_val : val);

        bool current_hit = _isHit();
        if (!current_hit && ((now - _adc_measuring_start_time) > ADC_MEASURING_WINDOW_MS)) {
            if (_peak_val >= _upper_limit) {
                _force = 127;
            } else {
                _force = _force_map(_peak_val);
            }

            #ifndef PEAK_CHK_DBG
            _peak_val = 0;
            #endif

            _adc_measuring = false;
            _measurement_cplt = true;
        }

        _last_adc_measuring_state = _adc_measuring;
    }
}

/**
 * @brief Set the output pin state
 * @param state GPIO_PIN_SET or GPIO_PIN_RESET
 */
void Pad::setOut(GPIO_PinState state) {
    HAL_GPIO_WritePin(_out_port, _out_pin, state);
}

/**
 * @brief Check if current reading indicates a hit
 * @return true if ADC value exceeds threshold, false otherwise
 */
bool Pad::_isHit() {
    return (_getADCVal() > _hit_threshold);
}

/**
 * @brief Get current ADC value from appropriate buffer
 * @return uint16_t Raw ADC value
 */
uint16_t Pad::_getADCVal() {
    switch (_piezo_adc_group) {
        case ADC_1: return adc1_buf[_piezo_adc_index];
        case ADC_2: return adc2_buf[_piezo_adc_index];
        case ADC_3: return adc3_buf[_piezo_adc_index];
        default: return 0;
    }
}

/**
 * @brief Map raw ADC value to force/velocity (0-127)
 * @param adc_val Raw ADC value
 * @return uint8_t Mapped force value
 * 
 * Applies different mapping curves based on current setting:
 * - LINEAR: y = kx
 * - LOG: y = klog10(1 + 9x) (more sensitive at lower values)
 * - EXP: y = kx^1.5 (more sensitive at higher values)
 * - k: 126/adc_range
 * 
 * Where x is normalized ADC value (0.0 to 1.0)
 * and y is mapped to 1-127 range
 */
uint8_t Pad::_force_map(uint16_t adc_val) {
    if (adc_val <= _hit_threshold) return 1;

    uint16_t orig_delta = _upper_limit - _hit_threshold; // ADC measurement range
    if (orig_delta <= 0) { return 1; }

    float temp = (float)(adc_val - _hit_threshold) / (float)orig_delta; // Normalized value 0.0 to 1.0
    uint8_t mapped_delta = 126; // Output range (1-127)

    switch (_force_curve) {
        case CURVE_LINEAR: // Linear mapping
            temp = (temp * mapped_delta) + 1.0f;
            break;   
        case CURVE_LOG: // Logarithmic mapping (more sensitive at low values)
            temp = (log(1.0f + temp * 9.0f) / log(10.0f) * mapped_delta) + 1.0f;
            break;
        case CURVE_EXP: // Exponential mapping (more sensitive at high values)
            temp = (pow(temp, 1.5f) * mapped_delta) + 1.0f;
            break;
        default: // Fallback to linear
            temp = (temp * mapped_delta) + 1.0f;
            break;
    }

    temp = ((temp >= 126.5) ? 127 : temp);
    return (uint8_t)temp;
}
