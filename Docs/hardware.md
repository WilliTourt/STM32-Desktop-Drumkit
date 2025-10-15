# STM32 Desktop Drumkit - Hardware Description

> This document will introduce the hardware design related to STM32 Desktop Drumkit, as well as the explanations related to PCB soldering and 3D part printing.

![Main Board PCB](../Hardware/Schematic%20&%20PCB/Images/MotherBoard.jpg)
![UI Board PCB](../Hardware/Schematic%20&%20PCB/Images/UI%20Board.jpg)
![3D Case](../Hardware/3D%20Files/ImageSW.png)

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

The connection layout are corresponding to STM32CubeMX configuration. Use CubeMX to see peripherals and pin configuration.

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
   ![CH340K](../Hardware/Schematic%20&%20PCB/Images/CH340K.jpg)
3. Pay attention to diode direction in signal processing circuits
4. The 10k-68k resistors in the signal processing circuit are current-limiting resistors, 10k is recommended.
5. All user LEDs are optional:
   - TX/RX indicators
   - MIDI IN/OUT indicators
   - MIDI USB_RDY
   - 3v3 power indicator
6. For MCU crystal circuit, the 1M resistor can be left unsoldered

### UI Board
1. OLED pin insertion depth:
   - Do not insert fully
   - Recommended method:
     - After the case is printed
     - Place the small board in position
     - Insert OLED so pins just touch the bottom of the case notch (noticed in [Assembly Guide](Docs/partsassemble.md))

## 3D Printing

All related 3D files are in the [3D Files folder](../Hardware/3D%20Files). The following parts need to be printed:
- Case ([Desktop Drumkit V1.0.step](../Hardware/3D%20Files/step%20&%203mf/Desktop%20Drumkit%20V1.0.STEP))
- Top cover ([Drumkit Cover.step](../Hardware/3D%20Files/step%20&%203mf/Drumkit%20Cover.STEP))
- Ride cymbal stand ([DrumpadStand_Ride.step](../Hardware/3D%20Files/step%20&%203mf/DrumpadStand_Ride.STEP))
- 7 different sizes of drumhead fasteners ([Fasteners folder](../Hardware/3D%20Files/step%20&%203mf/Fasteners)), among which the hi-hat and high tom fasteners need to be printed twice.

### 3D Printing Settings

If you use Bambu Studio, you can directly use the 3mf project files I uploaded, with all slicing options pre-configured (but pay attention to switching printer and print bed options). However, the main case 3mf file was not uploaded (because the file size exceeds 100M).

If you use other 3D printing software, please pay attention to the following settings:
- Case:
  - Supports: No need to add supports for the bottom slots, **but full supports are required for the upper fastener ring parts!!**
  - Top shell thickness: At least **1.6mm**, the thicker the better. This ensures structural integrity when pressing the fasteners during assembly.
  - Sparse infill: Not lower than 12%
  - Speed: Recommended to be slightly slower

- Fasteners
  - Wall count: Fill the entire ring directly

- Top cover
  - The top cover model has very thin walls, slow down the printing speed!! You can also try Arachne wall generation.

- Ride cymbal stand
  - Normal printing, no special settings required.
