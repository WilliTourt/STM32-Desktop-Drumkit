# STM32 Desktop Drumkit - Software/Firmware/API Documentation

## System Overview

The STM32 Desktop Drumkit is a MIDI controller that converts drum pad hits into MIDI messages. It features:

- 10 drum pads with velocity-sensitive input
- OLED display for status and configuration
- USB MIDI output
- Configurable pad settings

## Hardware Interfaces

### Pin Configuration

The midi note config can be easily modified in midi.h if u want. See [Debugging & Parameter Calibration](Docs/howtodebug.md) for details.

| Pad Name    | ADC Group | ADC Channel | Output Port/Pin | MIDI Note |
|-------------|-----------|-------------|-----------------|-----------|
| OpenHiHat   | ADC1      | 0           | GPIOA/P4        | 46        |
| CloseHiHat  | ADC1      | 1           | GPIOA/P5        | 42        |
| Crash       | ADC1      | 2           | GPIOA/P6        | 49        |
| Ride        | ADC1      | 3           | GPIOA/P7        | 51        |
| SideStick   | ADC2      | 0           | GPIOC/P0        | 37        |
| Kick        | ADC2      | 1           | GPIOC/P1        | 35        |
| Snare       | ADC2      | 2           | GPIOC/P2        | 38        |
| MidTom      | ADC3      | 0           | GPIOA/P0        | 48        |
| LowTom      | ADC3      | 1           | GPIOA/P1        | 45        |
| HighTom     | ADC3      | 2           | GPIOC/P3        | 50        |

### Peripheral Usage

- **ADC1-3**: Pad input sensing (with DMA)
- **I2C1**: OLED display communication  
- **USART1**: Debug output
- **USART2**: MIDI output
- **GPIO**: Pad output triggers, buttons, LEDs

## Software Architecture

This project is designed to be modular. I chose to use C++ instead of C to better handle multiple insts of classes. The main entry point is `cpp_main.cpp`.
This project is developed using VSCode with EIDE plugin. If you also use EIDE, you can directly import the workspace.

### Main Components

1. **Pad Class** (`pad.h/cpp`)
   - Handles pad input processing
   - Hit detection and force measurement
   - Velocity mapping curves

2. **UI Class** (`ui.h/cpp`)
   - OLED display management
   - Menu navigation
   - Button input handling

3. **Midi Class** (`midi.h/cpp`)
   - MIDI message construction
   - Note On/Off handling
   - Channel state management

4. **Main Application** (`cpp_main.cpp`)
   - System initialization
   - Main processing loop
   - Module coordination

### Data Flow

1. Pad hits are detected via ADC/DMA
2. Force values are measured and mapped to velocity
3. MIDI Note On messages are sent
4. UI is updated with pad activity
5. MIDI Note Off is sent automatically after a certain period of time

## API Reference

### Pad Class

```cpp
class Pad {
public:
    // ADC groups (1-3)
    enum ADCGroup { ADC_1, ADC_2, ADC_3 };
    
    // Pad identifiers (10 pads total)
    enum PadID { OpenHiHat, CloseHiHat, Crash, Ride, SideStick, 
                Kick, Snare, MidTom, LowTom, HighTom, PAD_NUM };
    
    // Force mapping curves (linear/log/exp)
    enum ForceMappingCurve { CURVE_LINEAR, CURVE_LOG, CURVE_EXP };
    
    // Constructor: configures pad with ADC, GPIO and response settings
    // Pad parameters: ADC group, adc rank, output port/pin, ID, trigger threshold, limit, mapping curve type
    Pad(ADCGroup group, uint8_t adc_index, GPIO_TypeDef* port, uint16_t pin, 
        PadID id, uint16_t threshold, uint16_t upper_limit, ForceMappingCurve curve);
        
    // Core pad functions:
    void detectHit();      // Detect new hits (call first)
    bool isTriggered();    // Check if new hit detected
    void measureForce();   // Calculate force after trigger
    uint8_t getForce();    // Get velocity value (0-127)
    
    // Utility functions:
    PadID getID();        // Get pad identifier
    void setForceCurve(ForceMappingCurve curve); // Change mapping curve
    uint16_t getADCVal_DBG(); // Get raw ADC value (debug only)
};
```

### UI Class

```cpp 
class UI {
public:
    // Display modes (PAGE for real-time, MENU for navigation)
    enum class DisplayMode { PAGE, MENU };
    
    // Available pages (main, test, settings, stats)
    enum class Page { MAIN, PAD_TEST, PAD_SETTING, STATS };
    
    // Initialize UI and button with timing parameters
    void init();
    void buttonInit(uint16_t debounce, uint16_t click_ms, uint16_t press_ms, uint16_t multi_ms);
    
    // Runtime functions (call in main loop):
    void buttonTick();    // Process button input
    void update();        // Refresh display
    
    // Status updates:
    void updatePadStats(Pad::PadID id, uint32_t hits); // Update hit counts
    void updateMidiStats();  // Update MIDI stats
    void updateMidiConn(bool connected); // Update connection status
    
    // Display control:
    void setMode(DisplayMode mode); // Switch between PAGE/MENU
    void setPage(Page page);       // Change display page
    DisplayMode getMode();         // Get current mode
    Page getPage();                // Get current page
};
```

### Midi Class

```cpp
class Midi {
public:
    // Send MIDI messages:
    bool sendNoteOn(Pad::PadID padID, uint8_t velocity, uint8_t channel = 10);
    void sendNoteOff(Pad::PadID padID); // Auto uses pad's note/channel
    void sendNoteOff(uint8_t note, uint8_t channel = 10); // Manual note off
    
    // Handle automatic note off (call in main loop)
    void autoNoteOff();
    
    // Check connection status
    bool isConnected();
};
```

## Firmware

You can use STM32CubeProgrammer or STM32 Utility tools to flash the firmware.
Pre-built firmware files are available in the Project folder/build directory.
