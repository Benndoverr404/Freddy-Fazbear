# Firmware

The `.ino` files here are Arduino-style sketches for the ESP32 animatronic eyes.

## Sketch files

- `Animatronic_Eyes_Annotated_Color_Coded.ino` — annotated version with comments and color-coded sections in the source.
- `Animatronic_Eyes_Current_Sketch.ino` — copy of the current project sketch as supplied in the working files.

## Libraries

The annotated sketch includes:
- `Bluepad32.h` — Bluetooth game-controller support.
- `ESP32Servo.h` — servo control for ESP32.

Install compatible versions of these libraries using their documented installation instructions. Record tested versions here once confirmed.

## Upload and controller setup

1. Install the ESP32 board support package in Arduino IDE.
2. Install the required libraries.
3. Open the sketch and select the matching ESP32 board and serial port.
4. Upload the sketch.
5. Pair the controller according to the Bluepad32 instructions and verify that the serial monitor reports a connection.
6. Test movements with the mechanism safely supported and clear of obstructions.

## Calibration

Servo angles in the sketch are starting values, not universal settings. Adjust them for the actual linkage and mounting geometry. Start with conservative ranges and disconnect power if a servo buzzes, stalls, or pushes against a hard stop.

Record tested values and the hardware revision here:
- Eye center angles: see the constants in the sketch; verify mechanically.
- Eye left/right angles: see the constants in the sketch; verify mechanically.
- Eyelid open/closed angles: see the constants in the sketch; verify mechanically.

## Version notes

Keep one clearly named primary sketch. When changing behavior, update the change notes and retain older known-working versions in Git history rather than adding many ambiguously named copies.
