# ESP32 Animatronic Eyes

A build log and reference repository for a pair of controller-operated animatronic eyes, including the mechanical build, fabric work, electronics, wiring, and ESP32 firmware.

> **Project status:** Work in progress. Some build measurements, power details, and fabrication steps still need to be documented and verified. Check the relevant section before building from it.

## What this project does

- Moves both eyes left and right using an Xbox-compatible Bluetooth controller.
- Opens and closes the eyelids with servo motors.
- Supports blinking controls.
- Controls two lighting outputs.
- Uses an ESP32-based controller and a camera module as part of the wider project setup.

## Repository map

| Section | What belongs here |
|---|---|
| [`mechanical/3d-printing/`](mechanical/3d-printing/README.md) | STL/3MF/STEP files, print orientation, material, settings, fit notes, revisions |
| [`mechanical/welding/`](mechanical/welding/README.md) | Frame drawings, cut list, weld locations, materials, assembly order, safety notes |
| [`fabric-work/`](fabric-work/README.md) | Fabric patterns, face/skin/fur covering, seam notes, adhesives, photos |
| [`electronics/parts-list/`](electronics/parts-list/README.md) | Bill of materials, quantities, links, substitutions, and what is already owned |
| [`electronics/wiring/`](electronics/wiring/README.md) | Pin map, power distribution, wiring diagrams, connector notes, test procedure |
| [`firmware/`](firmware/README.md) | Arduino/ESP32 sketches and setup instructions |
| [`docs/`](docs/README.md) | General build notes, calibration, troubleshooting, and guides |
| [`media/`](media/) | Build photos and diagrams referenced by the documentation |

## Current electronics notes

The current annotated sketch defines these ESP32 GPIO assignments:

| Function | GPIO |
|---|---:|
| Right eye servo signal | 13 |
| Right eyelid servo signal | 14 |
| Left eye servo signal | 19 |
| Left eyelid servo signal | 25 |
| Small diode LED output | 33 |
| LED strip control output | 32 |

These are the **signal/control pins used in the sketch**, not a complete power-wiring diagram. Confirm the physical board, power module, grounds, and LED driver wiring against the actual hardware before applying power.

The current project notes mention:
- ESP32 development board and ESP32-CAM (OV2640) for the camera side of the project.
- Four servo functions in the current sketch: left/right eye movement and left/right eyelid movement.
- Bluepad32 and ESP32Servo libraries in the annotated sketch.
- A PM01 5 V / 5 A servo power module in the project setup.
- 5 V LED strips and separate low-voltage diode LEDs.

The exact battery, converter arrangement, LED current limits, connector layout, and final camera-to-controller communication method should be recorded in the wiring guide once confirmed.

## Firmware quick start

1. Install the Arduino IDE or another compatible ESP32 development environment.
2. Install the libraries used by the sketch: **Bluepad32** and **ESP32Servo**.
3. Open `firmware/Animatronic_Eyes_Annotated_Color_Coded.ino`.
4. Select the correct ESP32 board and port.
5. Upload the sketch, then pair the controller using the procedure in [`firmware/README.md`](firmware/README.md).
6. Calibrate servo angles carefully before attaching the mechanisms or allowing the servos to drive against a hard stop.

## Build sections

Start with the section that matches the part of the build you are working on. Add photos and measurements as you go so another person can reproduce the build.

## Safety

- Disconnect battery power before changing wiring.
- Do not power servos from an ESP32 GPIO pin.
- Use a suitable regulated supply for servos and LEDs, and ensure grounds are connected where required by the circuit.
- Verify polarity and voltage before connecting a battery.
- For welding, use appropriate eye/face, hand, clothing, ventilation, and fire protection.
- Keep servo linkages clear of fingers, fabric, and loose wires during testing.

## License

No license has been selected yet. Choose a license before inviting others to reuse or modify the project.
