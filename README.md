# Firmware - Teensy3.2
Firmware for the teensy-3.2 microcontroller board.

# Toolchain Requirements
* Visual Studio
* Visual Micro - An extension for Visual Studio that enables compilation of Arduino-compatible projects

# Software Requirements
1. VCO Control Waveform Output:
   * Must be able to output a voltage waveform using DAC
   * Must have a configurable triangle wave sweep range
   * Must have a configurable, consistent output period
   * For FMCW operation, must be configured to sweep with a 200ms period
2. Down-Mixed Waveform Input:
   * Must have a configurable, consistent sample period
   * Must alternate between capture and compute phases
   * Should use DMA channel to collect ADC data
   * Must initiate capture shortly after a DAC sweep begins to avoid VCO transients
3. User Interface:
   * Must allow configuration of baseband amplifier gain
   * Must display the configured digital potentiometer value
   * Must allow configuration of Doppler and FMCW modes
   * Must display the measured Doppler speed and FMCW ranges in their respective modes
   * Should display a portion of the FFT for visual indication of received data
