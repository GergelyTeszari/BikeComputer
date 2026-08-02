# Bike Computer

An Arduino Nano / Uno based bicycle computer prototype for measuring wheel speed, travelled distance, and average speed using a wheel-mounted magnet sensor and a 16×2 LCD.

This is a compact embedded hobby project built around a real hardware prototype: an Arduino board, LCD display, button panel, wheel sensor, and magnet trigger assembly.

## Prototype photos

### Full prototype setup

![BikeComputer prototype with LCD, button panel, wheel sensor and magnet assembly](docs/images/bikecomputer-full-prototype.jpg)

### Display and button panel

![BikeComputer prototype showing the LCD welcome screen](docs/images/bikecomputer-closeup-welcome.jpg)

## Project overview

The firmware reads a wheel rotation sensor, estimates speed from the elapsed time between wheel rotations, updates an odometer, calculates a sampled average speed, and displays the current values on a 16×2 character LCD.

```text
Wheel magnet + sensor
        ↓
Arduino Nano / Uno
        ↓
Speed / distance calculation
        ↓
16×2 LCD output
```

## Current features

- Wheel rotation detection through a digital sensor input.
- Current speed calculation from wheel circumference and elapsed rotation time.
- Odometer-style trip-distance accumulation.
- Sampled average speed calculation.
- 16×2 LCD output showing speed, distance, and average speed.
- Metric-unit operation using meters, kilometres, and km/h.
- Status LED feedback while the wheel sensor is triggered.
- Four hardware button inputs prepared for UI functions.
- Button 1 resets the current trip values.
- Button 2–4 are reserved for future UI functions.
- `millis()`-based main-loop timing without a continuous blocking delay.

## Hardware

The project was written for an Arduino Nano board, but the firmware should also be portable to an Arduino Uno with matching pin assignments.

Main hardware components:

- Arduino Nano or Arduino Uno
- 16×2 character LCD
- wheel-mounted magnet
- magnetic / reed / hall-effect rotation sensor
- four push buttons
- status LED on pin 13
- custom hand-built wiring / prototype board

## Pin mapping

The current firmware uses the following pin assignments:

| Function | Arduino pin |
|---|---:|
| LCD RS | 10 |
| LCD EN | 8 |
| LCD D4 | 3 |
| LCD D5 | 2 |
| LCD D6 | 5 |
| LCD D7 | 4 |
| Wheel sensor input | 12 |
| Status LED / blinker | 13 |
| Button 1 | A0 |
| Button 2 | A1 |
| Button 3 | A4 |
| Button 4 | A5 |

## Display layout

The LCD is formatted around a compact two-line layout:

```text
Speed  ODO  AVG
--.-  --.-- --.-
```

## Configuration

The wheel circumference is configured directly in the source code:

```cpp
const float WHEEL_CIRCUMFERENCE_M = 2.04f;
```

The value is defined in meters and should be adjusted to match the real wheel and tire size.

Main timing and filtering parameters are also defined near the top of the sketch:

```cpp
const float MAX_VALID_SPEED_KMH = 100.0f;
const float SPEED_SMOOTHING_FACTOR = 0.5f;

const unsigned long STOPPED_TIMEOUT_MS = 2000UL;
const unsigned long AVERAGE_SAMPLE_PERIOD_MS = 30000UL;
const unsigned long MIN_DISPLAY_REFRESH_PERIOD_MS = 250UL;
```

## Build and upload

Open `BikeComputer.ino` in the Arduino IDE, install the required pin-change interrupt dependency if needed, select the target Arduino board and port, then upload the sketch.

The firmware uses:

```cpp
#include <LiquidCrystal.h>
#include "PinChangeInterrupt.h"
```

## Implementation notes

This project is intentionally kept as a compact Arduino prototype. The firmware stays close to Arduino conventions instead of introducing a heavier architecture, because the target hardware is an Arduino Nano / Uno class board with limited SRAM and simple timing requirements.

The current version keeps the code in a single `.ino` file, but separates the main responsibilities into small functions:

```text
setup()
loop()
showWelcomeScreen()
handleWheelSensor()
handleWheelRotation()
updateStoppedState()
updateAverageSpeed()
handleButtons()
resetTrip()
printLCD()
```

Button interrupt callbacks do not write to the LCD and do not call `delay()`. They only set small `volatile bool` flags, which are processed from the main loop.

## Current status

This repository represents a working Arduino prototype and a cleaned-up single-file sketch.

The code is not meant to look like a large production firmware project. Its purpose is to show a practical sensing-to-display embedded prototype while keeping the implementation simple enough for the target hardware.

## Known limitations

- Metric mode is currently the only fully supported unit mode.
- Button 2–4 are wired and handled, but do not yet have user-facing functions.
- Button debounce handling is still minimal.
- Odometer / trip values are not persisted after power-off.
- The project does not yet include a wiring diagram.
- The 3D enclosure model is not yet documented in the repository.
- The refactored sketch should be validated on the physical prototype after each change.

## Possible next improvements

Recommended next steps that stay within Arduino-scope:

- Add simple button debounce timing.
- Use Button 2 to switch display pages.
- Use Button 3 / Button 4 for configuration or backlight/menu functions.
- Add EEPROM persistence for trip distance or total odometer.
- Add a wiring diagram.
- Add the 3D enclosure model files and document print notes.
- Add a short demo video or GIF of the sensor and display during operation.
- Add a small changelog describing the move from the original prototype sketch to the cleaned-up sketch.

## Suggested enclosure documentation

If the 3D model files are added, a useful structure would be:

```text
hardware/enclosure/
├── README.md
├── bikecomputer_enclosure_top.stl
├── bikecomputer_enclosure_bottom.stl
└── photos/
```

The enclosure README could document print orientation, material, screw sizes, fitment caveats, board mounting, LCD mounting, and sensor-cable routing.

## What this project demonstrates

- Embedded input processing with a real sensor.
- Sensor-based speed measurement.
- LCD output on a constrained microcontroller.
- Arduino hardware integration.
- Interrupt flag handling without doing heavy work inside callbacks.
- A practical hand-built prototype with room for iterative improvement.
