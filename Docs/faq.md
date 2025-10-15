# STM32 Desktop Drumkit - Frequently Asked Questions (FAQ)

## No sound from computer after hitting the drum pad?

1. Check if the screen displays "MIDI: READY!". If not, check if the MIDI interface is properly connected.
2. Verify the polarity of the piezo sensor connections. Refer to the `Piezo Sensor Fabrication & Installation` section in the [Assembly Guide](../Docs/partsassemble.md).
3. Check the MIDI connection settings and MIDI mapping in your DAW.
    ![MIDI Settings](../Images/Debug/FL%20Screenshot.png)
4. Recalibrate the piezo sensor to ensure its waveform is normal.

## Inaccurate velocity output?

1. Fine-tune the parameters. See details in the [Debugging and Calibration Guide](../Docs/howtodebug.md).
2. This could also be affected by the sampling rate. If the peak waveform of the piezo sensor is very short, the STM32 might not be able to capture the most accurate peak.

## Experiencing crosstalk/false triggering?

1. Check your `hit_threshold` and `HIT_THRESHOLD_OFFSET` parameters. See details in the [Debugging and Calibration Guide](../Docs/howtodebug.md).

2. If you still experience crosstalk after parameter adjustment, check if the problematic piezo sensor's waveform has strong jumps at the moment of hitting. Try replacing the piezo sensor. You can also try using a higher capacitance value for the filter capacitor in the signal processing circuit.
