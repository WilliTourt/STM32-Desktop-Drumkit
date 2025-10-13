/**
 * @file midi.cpp
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

#include "midi.h"
// #include "stdio.h" // For debugging

// Definition of static [pad - midi-note] map
constexpr uint8_t Midi::_PAD_MIDI_NOTE_MAP[MIDI_CHANNELS_NUM];

static Midi* _midi_inst = nullptr; // Global instance pointer for interrupt handling

/**
 * @brief Construct a new Midi object
 * 
 * Initializes MIDI interface with:
 * - ACK flag set to true (ready for first byte)
 * - Connection status false
 * - Channel states initialized with default values
 */
Midi::Midi() : _ack(true), _connected(false) {
    _midi_inst = this; // Set global instance for interrupt callbacks
    for (uint8_t i = 0; i < MIDI_CHANNELS_NUM; i++) {
        _channel_states[i].noteOn_sent = false;
        _channel_states[i].noteOn_timestamp = 0;
        _channel_states[i].note = _PAD_MIDI_NOTE_MAP[i];
        _channel_states[i].channel = MIDI_CHANNEL_ID;
    }
}

/**
 * @brief Send a MIDI Note On message with error handling
 * @param padID Pad identifier
 * @param velocity MIDI velocity (0-127)
 * @param channel MIDI channel (1-16)
 * 
 * Constructs and sends a 3-byte MIDI Note On message with timeout and error handling
 * @return true if sent successful, false if failed
 */
bool Midi::sendNoteOn(Pad::PadID padID, uint8_t velocity, uint8_t channel) {
    // sprintf(dbg_buf, "sendNoteOn called for pad %d (current noteOn_sent=%d)\r\n", 
    //        padID, _channel_states[padID].noteOn_sent);
    // DBG(dbg_buf);
    if (!_connected) return false;
    
    uint8_t note = _PAD_MIDI_NOTE_MAP[padID];
    
    uint8_t midi_msg[3];
    midi_msg[0] = 0x90 | ((channel - 1) & 0x0F); // Note On status + channel
    midi_msg[1] = note & 0x7F;                   // Note number
    midi_msg[2] = velocity & 0x7F;               // Velocity
    
    for (uint8_t i = 0; i < 3; i++) {
        if (!_sendByte(midi_msg[i])) {
            _connected = false;
            return false;
        }
    }

    // Update channel state
    _channel_states[padID].noteOn_sent = true;
    _channel_states[padID].noteOn_timestamp = HAL_GetTick();
    _channel_states[padID].note = note;
    _channel_states[padID].channel = channel;
    return true;
}

/**
 * @brief Send a MIDI Note Off message
 * @param note MIDI note number
 * @param channel MIDI channel (1-16)
 * 
 * Constructs and sends a 3-byte MIDI Note Off message:
 * 1. Status byte (0x80 + channel)
 * 2. Note number
 * 3. Zero velocity
 */
void Midi::sendNoteOff(uint8_t note, uint8_t channel) {
    // sprintf(dbg_buf, "Alternate Midi::sendNoteOff(uint8_t note, uint8_t channel) called (note=%d, channel=%d)\r\n", note, channel);
    // DBG(dbg_buf);
    
    uint8_t midi_msg[3];
    midi_msg[0] = 0x80 | ((channel - 1) & 0x0F); // Note Off status + channel
    midi_msg[1] = note & 0x7F;                   // Note number
    midi_msg[2] = 0x00;                         // Zero velocity
    
    for (uint8_t i = 0; i < 3; i++) {
        _sendByte(midi_msg[i]);
    }
}

/**
 * @brief Send a MIDI Note Off message for a pad
 * @param padID Pad identifier
 * 
 * Sends Note Off using the pad's stored note and channel values
 * Only sends if a Note On was previously sent for this pad
 */
void Midi::sendNoteOff(Pad::PadID padID) {
    // sprintf(dbg_buf, "Entering Midi::sendNoteOff(Pad::PadID padID) for pad %d\r\n", padID);
    // DBG(dbg_buf);

    if (padID >= MIDI_CHANNELS_NUM || !_channel_states[padID].noteOn_sent)  { return; }
    
    uint8_t midi_msg[3];
    midi_msg[0] = 0x80 | ((_channel_states[padID].channel - 1) & 0x0F); // Note Off status + channel
    midi_msg[1] = _channel_states[padID].note & 0x7F;                   // Note number
    midi_msg[2] = 0x00;                                                 // Zero velocity
    
    for (uint8_t i = 0; i < 3; i++) {
        // sprintf(dbg_buf, "Sending MIDI byte %d: 0x%02X\r\n", i, midi_msg[i]);
        // DBG(dbg_buf);
        _sendByte(midi_msg[i]);
    }

    // sprintf(dbg_buf, "Resetting noteOn_sent for pad %d\r\n", padID);
    // DBG(dbg_buf);
    _channel_states[padID].noteOn_sent = false;
    
    // Verify the flag was actually set
    // if (_channel_states[padID].noteOn_sent) {
    //     sprintf(dbg_buf, "ERROR: Failed to reset noteOn_sent for pad %d\r\n", padID);
    //     DBG(dbg_buf);
    // } else {
    //     sprintf(dbg_buf, "Confirmed noteOn_sent reset for pad %d\r\n", padID);
    //     DBG(dbg_buf);
    // }
    
    // sprintf(dbg_buf, "Exiting Midi::sendNoteOff(Pad::PadID padID) (success) for pad %d\r\n", padID);
    // DBG(dbg_buf);
}

/**
 * @brief Handle automatic note off events
 * 
 * Checks all channels and sends Note Off for any notes that have exceeded
 * the NOTEOFF_DELAY_MS duration since their Note On
 */
void Midi::autoNoteOff() {
    uint32_t now = HAL_GetTick();

    for (uint8_t id = 0; id < MIDI_CHANNELS_NUM; id++) {
        if (_channel_states[id].noteOn_sent) {
            uint32_t elapsed = now - _channel_states[id].noteOn_timestamp;
            // sprintf(dbg_buf, "Pad %d: elapsed=%lu, threshold=%d\r\n", id, elapsed, NOTEOFF_DELAY_MS);
            // DBG(dbg_buf);
            if (elapsed > NOTEOFF_DELAY_MS) {
                // sprintf(dbg_buf, "Calling sendNoteOff for pad %d (elapsed: %lu)\r\n", id, elapsed);
                // DBG(dbg_buf);
                
                sendNoteOff(static_cast<Pad::PadID>(id)); // static_cast is NECESSARY here

                // sprintf(dbg_buf, "Auto Note Off sent for pad %d at %lu (elapsed: %lu)\r\n", id, now, elapsed);
                // DBG(dbg_buf);
                // sprintf(dbg_buf, "Post-call check: pad %d noteOn_sent=%d\r\n", 
                //        id, _channel_states[id].noteOn_sent);
                // DBG(dbg_buf);
            }
        }
    }
}

/**
 * @brief Send a single MIDI byte with flow control and timeout
 * @param byte MIDI byte to send
 * @return true if sent successfully, false if timeout
 * 
 * Waits for ACK from MIDI interface with timeout
 * Uses UART2 for transmission
 */
bool Midi::_sendByte(uint8_t byte) {
    uint32_t start = HAL_GetTick();
    while (!_ack) {
        if (HAL_GetTick() - start > MIDI_SEND_TIMEOUT_MS) {
            _connected = false;
            return false;
        }
    }
    _ack = false;
    HAL_UART_Transmit(&huart2, &byte, 1, 10);
    return true;
}

/**
 * @brief Check if MIDI interface is connected
 * @return true if connected, false otherwise
 */
bool Midi::isConnected() {
    bool state = (HAL_GPIO_ReadPin(USB_RDY_GPIO_Port, USB_RDY_Pin) == GPIO_PIN_RESET);
    _connected = state;
    return _connected;
}

extern "C" {

/**
 * @brief GPIO interrupt callback for MIDI control signals
 * @param GPIO_Pin Pin that triggered the interrupt
 * 
 * Handles:
 * - CH345T ACK signal (sets _ack flag)
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (!_midi_inst) { return; }

    if (GPIO_Pin == CH345_ACK_IT_Pin) {         // ACK Received from MIDI interface
        _midi_inst->_ack = true;
    }
}

} // extern "C"