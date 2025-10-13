/**
 * @file pad.h
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

#pragma once

#include "cpp_main.h"

#define ADC_PAD_HIT_DEFAULT_THRESHOLD 1000 // Default threshold for pad hit detection
#define ADC_PAD_DEFAULT_UPPER_LIMIT 4095   // Default upper limit for ADC readings
#define ADC_MEASURING_WINDOW_MS 18         // Time window for ADC measuring in milliseconds

#define ADC1_PAD_NUMS 4                    // Number of pads connected to ADC1
#define ADC2_PAD_NUMS 3                    // Number of pads connected to ADC2
#define ADC3_PAD_NUMS 3                    // Number of pads connected to ADC3

/**
 * @brief Class for handling drum pad inputs
 * 
 * The Pad class processes inputs from piezo sensors connected to ADC channels,
 * detects hits, measures force/velocity, and controls output pins.
 */
class Pad {
    public:
        /**
         * @brief Enum for ADC group selection
         */
        enum ADCGroup {
            ADC_1, // ADC1
            ADC_2, // ADC2
            ADC_3  // ADC3
        };

        /**
         * @brief ADC buffer for pads connected to ADC1
         * @warning Should not be accessed directly
         */
        static uint16_t adc1_buf[ADC1_PAD_NUMS];
        
        /**
         * @brief ADC buffer for pads connected to ADC2
         * @warning Should not be accessed directly
         */
        static uint16_t adc2_buf[ADC2_PAD_NUMS];
        
        /**
         * @brief ADC buffer for pads connected to ADC3
         * @warning Should not be accessed directly
         */
        static uint16_t adc3_buf[ADC3_PAD_NUMS];

        /**
         * @brief Enum for pad identification
         * 
         * Identifies the type of drum pad for MIDI mapping and processing
         */
        enum PadID {
            OpenHiHat,
            CloseHiHat,
            Crash,
            Ride,
            SideStick,
            Kick,
            Snare,
            MidTom,
            LowTom,
            HighTom,
            PAD_NUM // Number of pads
        };

        /**
         * @brief Enum for force mapping curve types
         * 
         * Defines different velocity/force response curves
         */
        enum ForceMappingCurve {
            CURVE_LINEAR, // Linear mapping curve
            CURVE_LOG,    // Logarithmic mapping curve
            CURVE_EXP     // Exponential mapping curve
        };

        /**
         * @brief Construct a new drumpad object
         * @param piezo_adc_group ADC group the pad is connected to
         * @param piezo_adc_index Index of the pad in the ADC group
         * @param out_port GPIO port for output pin
         * @param out_pin GPIO pin number for output
         * @param pad_id Identifier for the pad (Used for MIDI mapping)
         * @param hit_threshold Threshold value for hit detection (default: ADC_PAD_HIT_DEFAULT_THRESHOLD)
         * @param upper_limit Upper limit for ADC readings (default: ADC_PAD_DEFAULT_UPPER_LIMIT)
         * @param force_curve Force mapping curve type (default: CURVE_LINEAR)
         */
        Pad(ADCGroup piezo_adc_group, uint8_t piezo_adc_index, GPIO_TypeDef *out_port, uint16_t out_pin,
            PadID pad_id,
            uint16_t hit_threshold = ADC_PAD_HIT_DEFAULT_THRESHOLD,
            uint16_t upper_limit = ADC_PAD_DEFAULT_UPPER_LIMIT,
            ForceMappingCurve force_curve = CURVE_LINEAR);

        /**
         * @brief Get the current force value
         * @return uint8_t Force value (0-127)
         */
        inline uint8_t getForce() { return _force; };

        /**
         * @brief Detect hit and start force measurement window
         * 
         * Must be called before isTriggered() for proper operation.
         */
        void detectHit();

        /**
         * @brief Check if the pad is currently triggered
         * @return true if a NEW hit is detected, false otherwise
         * 
         * Must be called after detectHit() for proper operation.
         */
        bool isTriggered();

        /**
         * @brief Complete force measurement and calculate final value
         * 
         * Should be called after isTriggered() if a new hit was detected.
         */
        void measureForce();

        /**
         * @brief Check if force measurement is complete
         * @return true if measurement window has ended, false otherwise
         */
        inline bool isMeasurementCplt() { return _measurement_cplt; }

        /**
         * @brief Reset the measurement complete flag
         */
        inline void resetMeasurementCplt() { _measurement_cplt = false; }

        /**
         * @brief Get the velocity value (same as force)
         * @return uint8_t Velocity value (0-127)
         */
        uint8_t getVelocity() { return getForce(); };

        /**
         * @brief Get the pad ID
         * @return PadID The pad identifier
         */
        inline PadID getID() { return _pad_id; }

        /**
         * @brief Set the force mapping curve type
         * @param curve Force mapping curve type
         * 
         * Changes how ADC values are mapped to MIDI velocity:
         * - LINEAR: Direct proportional mapping
         * - LOG: Logarithmic response (softer hits have more sensitivity)
         * - EXP: Exponential response (harder hits have more sensitivity)
         */
        inline void setForceCurve(ForceMappingCurve curve) { _force_curve = curve; }

        /**
         * @brief Set the output pin state
         * @param state GPIO_PinState to set (GPIO_PIN_SET or GPIO_PIN_RESET)
         */
        void setOut(GPIO_PinState state);

        /**
         * @brief Get raw ADC value (for debugging)
         * @return uint16_t Raw ADC value
         */
        uint16_t getADCVal_DBG() { return _getADCVal(); };

        /**
         * @brief Get peak ADC value during measuring window (for debugging)
         * @return uint16_t Peak ADC value
         */
        inline uint16_t getPeak_DBG() { 

            #define PEAK_CHK_DBG

            uint16_t val = _peak_val;
            _peak_val = 0;
            return val;
        };

        /**
         * @brief Convert PadID to string for debugging
         * @param id Pad identifier
         * @return const char* String representation of the PadID
         */
        static const char* ID2Str(PadID id);

    private:
        ADCGroup _piezo_adc_group;              // ADC group this pad belongs to
        uint8_t _piezo_adc_index;               // Index of this pad in its ADC group
        GPIO_TypeDef *_out_port;                // GPIO port for output pin
        uint16_t _out_pin;                      // GPIO pin number for output

        uint8_t _force;                         // Current force value (0-127)
        ForceMappingCurve _force_curve;         // Force mapping curve type
        bool _last_trigger_state;               // Previous trigger state

        PadID _pad_id;                          // Identifier for this pad (used for MIDI mapping)

        // For solving INTERFERENCE
        uint16_t _hit_threshold;                // Threshold value for hit detection
        uint16_t _upper_limit;                  // Upper limit for ADC readings

        // For adc measuring window
        bool _isHit();                          // Internal method to check if pad was hit
        uint16_t _getADCVal();                  // Internal method to get current ADC value
        uint16_t _peak_val;                     // Peak ADC value during measuring window
        bool _adc_measuring;                    // Flag indicating ADC measuring is in progress
        bool _measurement_cplt;                 // Flag indicating measurement window completed
        uint32_t _adc_measuring_start_time;     // Timestamp when ADC measuring started
        uint32_t _last_adc_measuring_state;     // Last state of ADC measuring (for timing)

        /**
         * @brief Map raw ADC value to force value (0-127)
         * @param val Raw ADC value
         * @return uint8_t Mapped force value
         */
        uint8_t _force_map(uint16_t val);

};
