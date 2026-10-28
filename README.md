# Stepper Spindle Control – Arduino Nano + TMC2226

Arduino Nano sketch that drives a NEMA 17 stepper motor through a TMC2226 driver to move a load along an M5 threaded spindle. Four push buttons provide fast and timed (slow) motion in both directions, and a three-position toggle switch selects the travel distance for the timed moves.

![Circuit diagram](docs/circuit_TCM2226_3pos.png)

## Operating modes

| Button | Pin | Mode | Behaviour |
|---|---|---|---|
| Fast CW | A0 | Fast forward | Runs while the button is held |
| Fast CCW | A1 | Fast reverse | Runs while the button is held |
| Slow CW | A2 | Timed forward | Press to start, press again to stop |
| Slow CCW | A3 | Timed reverse | Press to start, press again to stop |

A timed move stops automatically when the selected distance has been travelled. Pressing either slow button during a timed move stops it. Pressing a fast button cancels a timed move.

The driver is disabled whenever the motor is not moving, so the motor draws no holding current at rest.

## Distance selection

The three-position toggle switch selects how far a timed move travels. The spindle has an M5 thread with a 0.8 mm pitch, so one motor revolution moves the load 0.8 mm.

| Switch position | Travel | Revolutions | Steps | Duration |
|---|---|---|---|---|
| Toward D4 | 40 mm | 50 | 80,000 | 300 s |
| Center (off) | 50 mm | 62.5 | 100,000 | 375 s |
| Toward D5 | 60 mm | 75 | 120,000 | 450 s |

The switch is read once, when a timed move starts. Changing the switch position during a move has no effect until the next move. If the switch is disconnected, the sketch defaults to the center position (50 mm).

## Hardware

- Arduino Nano
- TMC2226 stepper driver (V1.2 module), 1/8 microstepping
- NEMA 17 stepper motor (200 steps/rev)
- 4 momentary push buttons
- 1 three-position toggle switch, ON-OFF-ON (SPDT, center off)
- 12 V DC power supply
- Recommended: 100 µF electrolytic capacitor across the driver's VM and GND, close to the driver

## Wiring

| Arduino Nano | Connects to |
|---|---|
| A0, A1, A2, A3 | Push buttons (other side of each button to GND) |
| D4 | Toggle switch outer terminal (40 mm) |
| D5 | Toggle switch outer terminal (60 mm) |
| GND | Toggle switch middle terminal (COM), button ground, driver GND, supply − |
| D6 | TMC2226 DIR |
| D7 | TMC2226 STEP |
| D8 | TMC2226 EN |
| 3V3 | TMC2226 VDD (logic supply) |
| VIN | 12 V supply + |

| TMC2226 | Connects to |
|---|---|
| VM | 12 V supply + |
| GND (power side) | Supply − |
| 1A, 1B, 2A, 2B | Stepper motor coils |

All buttons and the toggle switch use the Nano's internal pull-up resistors, so no external resistors are needed.

## Software setup

1. Install the [Arduino IDE](https://www.arduino.cc/en/software).
2. Install the **AccelStepper** library: *Sketch → Include Library → Manage Libraries*, search for "AccelStepper" and install it.
3. Open `sketch_TCM2226_3pos/sketch_TCM2226_3pos.ino`.
4. Select *Tools → Board → Arduino Nano* and the correct port, then upload.

The Serial Monitor (9600 baud) shows the direction, step count and distance each time a timed move starts.

## Configuration

The main settings are constants at the top of the sketch:

| Constant | Default | Meaning |
|---|---|---|
| `distMmA` | 40.0 | Travel in mm, switch toward D4 |
| `distMmCenter` | 50.0 | Travel in mm, switch in center |
| `distMmB` | 60.0 | Travel in mm, switch toward D5 |
| `mmPerRev` | 0.8 | Spindle pitch (M5 thread) |
| `stepsPerRev` | 1600 | 200 full steps × 1/8 microstepping |
| `slowSpeed` | 266.66 | Timed-mode speed in steps/s (10 rpm, 8 mm/min) |
| `fastSpeedPercent` | 0.5 | Fast-mode speed as a fraction of max speed (10,000 steps/s) |

If you change the microstepping on the driver, update `stepsPerRev` to match.

## Repository structure

```
├── README.md
├── sketch_TCM2226_3pos/
│   └── sketch_TCM2226_3pos.ino
└── docs/
    └── circuit_TCM2226_3pos.png
```
