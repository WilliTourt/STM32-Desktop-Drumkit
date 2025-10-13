/**
 * @file midi.h
 * @brief MIDI communication interface for drumkit
 * 
 * This file defines the Midi class for handling MIDI communication,
 * including note on/off messages and channel state management.
 * 
 * @author WilliTourt, willitourt@foxmail.com
 * @copyright Copyright (c) 2025 by WilliTourt
 * 
 * @note Comments are mostly written by AI
 */

#pragma once

#include "cpp_main.h"
#include "pad.h"


/* MIDI NOTE CODES ------------------------------------------
    General MIDI (GM) Percussion Key Map
    https://musescore.org/sites/musescore.org/files/General%20MIDI%20Standard%20Percussion%20Set%20Key%20Map.pdf

    35 - Acoustic Bass Drum
    36 - Bass Drum 1
    37 - Side Stick
    38 - Acoustic Snare
    39 - Hand Clap
    40 - Electric Snare
    41 - Low Floor Tom
    42 - Closed Hi-Hat
    43 - High Floor Tom
    44 - Pedal Hi-Hat
    45 - Low Tom
    46 - Open Hi-Hat
    47 - Low-Mid Tom
    48 - Hi-Mid Tom
    49 - Crash Cymbal 1
    50 - High Tom
    51 - Ride Cymbal 1
    52 - Chinese Cymbal
    53 - Ride Bell
    54 - Tambourine
    55 - Splash Cymbal
    56 - Cowbell
    57 - Crash Cymbal 2
    58 - Vibraslap
    59 - Ride Cymbal 2
    60 - Hi Bongo
    61 - Low Bongo
    62 - Mute Hi Conga
    63 - Open Hi Conga
    64 - Low Conga
    65 - High Timbale
    66 - Low Timbale
    67 - High Agogo
    68 - Low Agogo
    69 - Cabasa
    70 - Maracas
    71 - Short Whistle
    72 - Long Whistle
    73 - Short Guiro
    74 - Long Guiro
    75 - Claves
    76 - Hi Wood Block
    77 - Low Wood Block
    78 - Mute Cuica
    79 - Open Cuica
    80 - Mute Triangle
    81 - Open Triangle
    82 - Shaker
---------------------------------------------------------- */

// Notes used in the drumkit
// DAW mapping should align with these values (GM standard)
#define ACOUSTIC_BASS_DRUM 35  // MIDI note number for acoustic bass drum
#define ACOUSTIC_SNARE     38  // MIDI note number for acoustic snare
#define LOW_TOM            45  // MIDI note number for low tom
#define HIGH_MID_TOM       48  // MIDI note number for hi-mid tom
#define HIGH_TOM           50  // MIDI note number for high tom
#define CLOSED_HI_HAT      42  // MIDI note number for closed hi-hat
#define OPEN_HI_HAT        46  // MIDI note number for open hi-hat
#define CRASH_CYMBAL_1     49  // MIDI note number for crash cymbal 1
#define RIDE_CYMBAL_1      51  // MIDI note number for ride cymbal 1
#define SIDESTICK          37  // MIDI note number for side stick


#define MIDI_CHANNELS_NUM Pad::PAD_NUM  // Number of MIDI channels (matches number of pads)
#define NOTEOFF_DELAY_MS 20             // Delay before sending note off message in milliseconds
#define MIDI_CHANNEL_ID 10              // Default MIDI channel ID for drumkit (channel 10 is percussion)
#define MIDI_SEND_TIMEOUT_MS 100        // Timeout for MIDI send operations (ms)


/**
 * @class Midi
 * @brief Handles MIDI communication for the drumkit
 * 
 * This class manages sending MIDI messages and handling note on/off events.
 * It supports automatic note off timing and channel state tracking.
 */
class Midi {
    public:
        /**
         * @brief Structure for tracking channel states
         */
        struct ChnState {
            volatile bool noteOn_sent;  // Flag indicating if note on was sent (volatile for memory visibility)
            uint32_t noteOn_timestamp;  // Timestamp of note on event
            uint8_t note;               // MIDI note number
            uint8_t channel;            // MIDI channel
        };

        /**
         * @brief Construct a new Midi object
         */
        Midi();

        /**
         * @brief Send a MIDI Note On message
         * @param padID Pad identifier
         * @param velocity MIDI velocity (0-127)
         * @param channel MIDI channel (defaults to 10)
         */
        bool sendNoteOn(Pad::PadID padID, uint8_t velocity, uint8_t channel = MIDI_CHANNEL_ID);

        /**
         * @brief Send a MIDI Note Off message
         * @param note MIDI note number
         * @param channel MIDI channel (defaults to 10)
         */
        void sendNoteOff(uint8_t note, uint8_t channel = MIDI_CHANNEL_ID);

        /**
         * @brief Send a MIDI Note Off message for a pad
         * @param padID Pad identifier
         */
        void sendNoteOff(Pad::PadID padID);

        /**
         * @brief Handle automatic note off events
         * 
         * Should be called periodically to send note off messages after the delay period
         */
        void autoNoteOff();

        /**
         * @brief Check if MIDI interface is connected
         * @return true if connected, false otherwise
         */
        bool isConnected();

    private:
        volatile ChnState _channel_states[MIDI_CHANNELS_NUM]; // Array of channel states (volatile for memory visibility)

        volatile bool _ack;       // Acknowledge flag (updated in interrupt)
        volatile bool _connected; // Connection status flag (updated in interrupt)

        /**
         * @brief Send a single MIDI byte
         * @param byte MIDI byte to send
         */
        bool _sendByte(uint8_t byte);

        /**
         * @brief MIDI note mapping for each pad
         * 
         * Maps Pad::PadID to corresponding MIDI note numbers
         */
        static constexpr uint8_t _PAD_MIDI_NOTE_MAP[MIDI_CHANNELS_NUM] = {
            OPEN_HI_HAT,        // OpenHiHat
            CLOSED_HI_HAT,      // CloseHiHat
            CRASH_CYMBAL_1,     // Crash
            RIDE_CYMBAL_1,      // Ride
            SIDESTICK,          // SideStick
            ACOUSTIC_BASS_DRUM, // Kick
            ACOUSTIC_SNARE,     // Snare
            HIGH_MID_TOM,       // MidTom
            LOW_TOM,            // LowTom
            HIGH_TOM            // HighTom
        };

        friend void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin); // Friend function for interrupt handling
};