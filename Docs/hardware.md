# Hardware Design

## PCB Components List

All required components for the PCB are listed in the BOM tables in the [BOM folder](../Hardware/Schematic%20&%20PCB/PCB%20BOM). The design files and Gerber files are also available in the [Schematic/PCB folder](../Hardware/Schematic%20&%20PCB). The PCB consists of two parts: the main board and the user interface small board.

## PCB Schematic Description

### Main Board Schematic
![Main Board Schematic](../Hardware/Schematic%20&%20PCB/Images/MotherBoard_Schematic.png)

The main board includes the following parts:
- MCU
- Power supply
- Communication interfaces
- Piezo sensor signal processing

Signal input/output flow:
1. 10-channel signal processing circuits perform biasing and clipping on the raw signals from external piezo sensors
2. Processed signals are connected to the MCU's ADC
3. MCU sends MIDI messages to the CH345T USB-MIDI chip
4. Sends debug information to CH340K
5. Controls the onboard buzzer
6. Through 6pin GH1.25 interface:
   - Sends I2C data to OLED
   - Controls LEDs
   - Receives button inputs

### User Interface Small Board Schematic
![User Interface Small Board Schematic](../Hardware/Schematic%20&%20PCB/Images/UI_Schematic.png)

- Receives data from MCU through 6pin interface
- MCU also detects button signals

## PCB Layout

### Main Board PCB Layout
![Main Board PCB Layout](../Hardware/Schematic%20&%20PCB/Images/MotherBoard_PCB_Top.png)

- Top: Three interfaces
  - MIDI
  - UART_DEBUG
  - SWD
- Right side:
  - 6pin interface for connecting UI small board
  - 10pin output control interface
- Bottom: 10 independent piezo sensor signal processing circuits
- Center:
  - Power selection circuit
  - Power toggle circuit
  - DCDC buck converter
  - MCU
  - 4 extra IOs

## PCB Soldering Notes

### Main Board
1. Feedback resistors for TLV62568 (100k/453k) must not be swapped
2. CH340K section:
   - R18 and C19 should be soldered as alternatives (choose one only)
   - Recommended to solder only R18 to supply 3.3v power to V3 pin (see CH340K datasheet for details)
3. Pay attention to diode direction in signal processing circuits
4. All user LEDs are optional:
   - TX/RX indicators
   - MIDI IN/OUT indicators
   - MIDI USB_RDY
   - 3v3 power indicator
5. For MCU crystal circuit, the 1M resistor can be left unsoldered

### UI Board
1. OLED pin insertion depth:
   - Do not insert fully
   - Recommended method:
     - After the case is printed
     - Place the small board in position
     - Insert OLED so pins just touch the bottom of the case notch
