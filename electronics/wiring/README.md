# Wiring Guide

This page records the GPIO/control assignments in the current annotated firmware. It is **not yet a complete, verified schematic** for the power system.

## Signal pin map from the sketch

| ESP32 GPIO | Signal | Connects to |
|---:|---|---|
| 13 | `RIGHT_EYE_PIN` | Right eye servo signal |
| 14 | `RIGHT_LID_PIN` | Right eyelid servo signal |
| 19 | `LEFT_EYE_PIN` | Left eye servo signal |
| 25 | `LEFT_LID_PIN` | Left eyelid servo signal |
| 33 | `DIODE_LED_PIN` | Diode LED control circuit |
| 32 | `STRIP_LED_PIN` | LED strip control/driver input |

**Important:** GPIOs are control signals. They do not provide servo or LED-strip power. The diode LED output must use an appropriate current-limiting arrangement, and LED strips must be switched through a suitable driver rather than powered directly from a GPIO.

## Power distribution — document and verify before use

The project notes mention a PM01 5 V / 5 A servo power module, a 12 V battery option, DC-DC converters, 5 V LED strips, and 3 V diode LEDs. The final configuration needs to be confirmed and drawn before this repository can be considered a build-ready wiring guide.

Add a diagram that clearly labels:
- Battery positive/negative and any fuse or protection.
- Converter input/output voltage and current ratings.
- Servo supply positive, servo ground, and each servo signal.
- ESP32 supply and the required common ground/reference between control signals and drivers.
- LED driver/MOSFET input, output, and load power connections.
- Camera power and the camera-to-controller communication method.
- Connector names and polarity.

## Bring-up checklist

- [ ] Disconnect battery power before wiring changes.
- [ ] Check polarity and continuity with power disconnected.
- [ ] Confirm each converter's output voltage before connecting electronics.
- [ ] Test servo power separately from the controller logic where appropriate.
- [ ] Connect a single servo and verify direction/range before installing linkages.
- [ ] Confirm the servos do not stall against mechanical stops.
- [ ] Test LEDs through the intended driver.
- [ ] Add the finished schematic to this folder and link it here.

## Diagram

Place the verified diagram at `wiring-diagram.png` or `wiring-diagram.pdf` and add it below:

> Wiring diagram not yet added — do not treat the pin table alone as a complete wiring schematic.
