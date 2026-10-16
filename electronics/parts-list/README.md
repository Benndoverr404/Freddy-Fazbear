# Parts List / Bill of Materials

This is a working list based on the current project notes. Verify exact models, quantities, and electrical ratings before ordering or wiring.

| Part | Current project note | Qty / status | Details still to confirm |
|---|---|---|---|
| ESP32 development board | Main servo/controller logic | 1 | Exact board model and revision |
| ESP32-CAM with OV2640 | Camera module | 1 | How it communicates with the main ESP32 |
| MG90S micro servos | Eye/eyelid movement | 4 in current sketch arrangement | Confirm final mechanical linkage and actual count |
| PM01 servo power module | 5 V / 5 A servo supply module | 1 mentioned | Input voltage range, wiring labels, actual load capacity |
| Battery | Earlier notes mention a 12 V, 2.6 Ah option | Unconfirmed | Final battery chemistry, protection, connector, runtime |
| DC-DC converter(s) | Used/planned for voltage conversion | Unconfirmed | Exact modules, input/output voltage and current |
| Small diode LEDs | 3 V, 20 mA noted previously | 2 mentioned | Confirm LED type and current-limiting method |
| LED strips | 5 V strips, about 2 A each in earlier notes | 2 mentioned | Confirm actual strip length/current and driver |
| MOSFET switching module | Planned for LED strip control | Unconfirmed | Exact module and active-high/low behavior |
| Xbox-compatible Bluetooth controller | Input device via Bluepad32 | 1 | Exact model and pairing steps |
| Wire, connectors, terminals | Power and signal wiring | As needed | Wire gauge, connector types, fuse rating |

## Ordering checklist

- [ ] Confirm each item’s exact model/part number.
- [ ] Record a supplier link and date checked.
- [ ] Separate parts already owned from parts still needed.
- [ ] Confirm voltage/current ratings and connector polarity.
- [ ] Record approved substitutions and what was tested.
