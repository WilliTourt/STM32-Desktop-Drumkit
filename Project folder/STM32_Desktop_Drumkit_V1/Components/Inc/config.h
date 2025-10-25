#pragma once

/**
 * @file config.h
 * @brief Centralized configuration for STM32 Desktop Drumkit
 * 
 * This file contains all configurable parameters for the drumkit system.
 * Modify these values to adjust system behavior without changing source code.
 * 
 * @author WilliTourt, willitourt@foxmail.com
 * @copyright Copyright (c) 2025 by WilliTourt
 * 
 */

// =============================================
// Pad Hit Threshold Settings
// =============================================

/**
 * @brief Global offset applied to all pad hit thresholds
 * @note Value range: 0-4095 (12-bit ADC range)
 */
#define _CFG_HIT_THRESHOLD_OFFSET 150

/**
 * @brief Individual pad hit thresholds (ADC values)
 * @note Values should be above noise level but below max playing force
 * @note Typical range: 300-2000 (12-bit ADC)
 */
#define _CFG_OPHIHAT_HIT_THRESHOLD 1013  // Open Hi-Hat
#define _CFG_CLHIHAT_HIT_THRESHOLD 602   // Closed Hi-Hat 
#define _CFG_CRASH_HIT_THRESHOLD   432   // Crash Cymbal
#define _CFG_RIDE_HIT_THRESHOLD	   365   // Ride Cymbal
#define _CFG_SSTK_HIT_THRESHOLD    1650  // Side Stick
#define _CFG_KICK_HIT_THRESHOLD	   1658  // Kick Drum
#define _CFG_SNARE_HIT_THRESHOLD   1587  // Snare Drum
#define _CFG_MIDTOM_HIT_THRESHOLD  959   // Mid Tom
#define _CFG_LOWTOM_HIT_THRESHOLD  1287  // Low Tom
#define _CFG_HIGHTOM_HIT_THRESHOLD 1378  // High Tom

// =============================================
// Pad Upper Limit Settings
// =============================================

/**
 * @brief Maximum force values for each pad (ADC values)
 * @note Used for velocity/force mapping to MIDI (0-127)
 * @note Should be set to typical maximum playing force
 */
#define _CFG_OPHIHAT_MAXF 1889  // Open Hi-Hat
#define _CFG_CLHIHAT_MAXF 1649  // Closed Hi-Hat
#define _CFG_CRASH_H_MAXF 2155  // Crash Cymbal
#define _CFG_RIDE_MAXF    792   // Ride Cymbal
#define _CFG_SSTK_MAXF    4095  // Side Stick
#define _CFG_KICK_MAXF    2140  // Kick Drum
#define _CFG_SNARE_MAXF   2174  // Snare Drum
#define _CFG_MIDTOM_MAXF  3254  // Mid Tom
#define _CFG_LOWTOM_MAXF  2647  // Low Tom
#define _CFG_HIGHTOM_MAXF 2791  // High Tom

// =============================================
// Force Mapping Curve Settings
// =============================================

/**
 * @brief Force mapping curve types for each pad
 * @note 0: Linear (direct mapping)
 * @note 1: Logarithmic (softer response)
 * @note 2: Exponential (stronger response)
 */
#define _CFG_OPHIHAT_FORCE_CURVE_TYPE 0  // Open Hi-Hat
#define _CFG_CLHIHAT_FORCE_CURVE_TYPE 0  // Closed Hi-Hat
#define _CFG_CRASH_FORCE_CURVE_TYPE   0  // Crash Cymbal
#define _CFG_RIDE_FORCE_CURVE_TYPE    0  // Ride Cymbal
#define _CFG_SSTK_FORCE_CURVE_TYPE    0  // Side Stick
#define _CFG_KICK_FORCE_CURVE_TYPE    2  // Kick Drum
#define _CFG_SNARE_FORCE_CURVE_TYPE   2  // Snare Drum
#define _CFG_MIDTOM_FORCE_CURVE_TYPE  0  // Mid Tom
#define _CFG_LOWTOM_FORCE_CURVE_TYPE  0  // Low Tom
#define _CFG_HIGHTOM_FORCE_CURVE_TYPE 0  // High Tom

// =============================================
// General Pad Settings
// =============================================

/**
 * @brief Time window for force measurement in milliseconds
 * @note Longer windows capture more of the hit but increase latency
 */
#define _CFG_PAD_MEASURING_WINDOW_MS 18

/**
 * @brief When true, set the velocity to _CFG_LOW_VELOCITY_THRESHOLD 
 *        if uint8_t Pad::_force_map(uint16_t adc_val) tend to return
 *        a value below _CFG_LOW_VELOCITY_THRESHOLD.
 */
#define _CFG_FORBID_LOW_VELOCITY true

#define _CFG_LOW_VELOCITY_THRESHOLD 50

// =============================================
// Button Settings
// =============================================

/**
 * @brief Button debounce time in milliseconds
 * @note Prevents false triggers from contact bounce
 */
#define _CFG_BUTTON_DEBOUNCE_TIME_MS       12

/**
 * @brief Maximum time between clicks for single click detection (ms)
 */
#define _CFG_BUTTON_SINGLECLICK_MAXTIME_MS 400

/**
 * @brief Minimum press duration for long press detection (ms)
 */
#define _CFG_BUTTON_LONGPRESS_MINTIME_MS   600

/**
 * @brief Maximum time between clicks for multi-click detection (ms)
 */
#define _CFG_BUTTON_MULTICLICK_MAXTIME_MS  900

// =============================================
// General MIDI Settings
// =============================================

/**
 * @brief MIDI note off signal delay in milliseconds
 */
#define _CFG_NOTEOFF_DELAY_MS     20    

/**
 * @brief MIDI channel ID for drumkit output
 * @note 10 is percussion channel
 */
#define _CFG_MIDI_CHANNEL_ID      10

/**
 * @brief MIDI send timeout in milliseconds
 * @note If a MIDI message is not acknowledged within this time,
 *       it is skiped and midi status will become "Disconnected"
 */
#define _CFG_MIDI_SEND_TIMEOUT_MS 100 

// =============================================
// Other Settings
// =============================================

#define _CFG_BUZZER_DEFAULT_FREQ 1000  // Default buzzer frequency (Hz)
