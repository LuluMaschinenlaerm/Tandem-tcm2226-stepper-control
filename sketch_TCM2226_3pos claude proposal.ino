#include <AccelStepper.h>

#define ButtonUp A0
#define ButtonDown A1
#define ButtonSlowCW A2
#define ButtonSlowCCW A3
#define EnablePin 8

// Three-position toggle switch (ON-OFF-ON)
// Middle terminal -> GND, outer terminals -> D4 and D5
// D4 side = 40 mm, center = 50 mm, D5 side = 60 mm
#define DistSwitchA 4
#define DistSwitchB 5

AccelStepper stepper(1, 7, 6); // (driver, step, dir)

// SLOW mode parameters
const float slowSpeed = 266.66;         // steps/s (1/8 microstepping)
const long stepsPerRev = 200L * 8;      // 1.8 deg motor x 1/8 microstep = 1600

// Spindle: M4 thread, 0.8 mm travel per revolution
const float mmPerRev = 0.8;

// SLOW mode travel distances in mm
// At 266.66 steps/s: 40 mm = 50 rev = 300 s, 50 mm = 62.5 rev = 375 s, 60 mm = 75 rev = 450 s
const float distMmA      = 40.0;   // switch toward D4 (original distance)
const float distMmCenter = 50.0;   // switch in center
const float distMmB      = 60.0;   // switch toward D5

// FAST mode: fraction of max speed
const float fastSpeedPercent = 0.5;

bool slowRunning = false;
int slowDirection = 1;

long slowStartPos = 0;      // position when the timed move started
long slowTargetSteps = 0;   // distance chosen by the switch for this move

bool lastSlowCW = HIGH;
bool lastSlowCCW = HIGH;

// Read the three-position switch and return the distance in steps
long getSlowDistanceSteps() {
  float mm;
  if (digitalRead(DistSwitchA) == LOW) {
    mm = distMmA;
  } else if (digitalRead(DistSwitchB) == LOW) {
    mm = distMmB;
  } else {
    mm = distMmCenter;
  }
  float revs = mm / mmPerRev;
  return (long)(revs * stepsPerRev + 0.5);  // round to nearest step
}

// Lock in the distance and start position at the beginning of a timed move
void startSlowMove(int direction) {
  slowDirection = direction;
  slowStartPos = stepper.currentPosition();
  slowTargetSteps = getSlowDistanceSteps();

  Serial.print("Timed move: ");
  Serial.print(direction > 0 ? "CW, " : "CCW, ");
  Serial.print(slowTargetSteps);
  Serial.print(" steps (");
  Serial.print(slowTargetSteps / (float)stepsPerRev * mmPerRev, 1);
  Serial.println(" mm)");
}

void setup() {

  pinMode(ButtonUp, INPUT_PULLUP);
  pinMode(ButtonDown, INPUT_PULLUP);
  pinMode(ButtonSlowCW, INPUT_PULLUP);
  pinMode(ButtonSlowCCW, INPUT_PULLUP);

  pinMode(DistSwitchA, INPUT_PULLUP);
  pinMode(DistSwitchB, INPUT_PULLUP);

  pinMode(EnablePin, OUTPUT);
  digitalWrite(EnablePin, HIGH); // driver OFF

  stepper.setMaxSpeed(10000); // max safe speed for NEMA 17 + 1/8 microstep
  Serial.begin(9600);
}

void loop() {

  bool slowCW = digitalRead(ButtonSlowCW);
  bool slowCCW = digitalRead(ButtonSlowCCW);

  // FAST CW
  if (digitalRead(ButtonUp) == LOW) {
    slowRunning = false;
    digitalWrite(EnablePin, LOW);
    stepper.setSpeed(stepper.maxSpeed() * fastSpeedPercent);
    stepper.runSpeed();
    return;
  }

  // FAST CCW
  if (digitalRead(ButtonDown) == LOW) {
    slowRunning = false;
    digitalWrite(EnablePin, LOW);
    stepper.setSpeed(-stepper.maxSpeed() * fastSpeedPercent);
    stepper.runSpeed();
    return;
  }

  // TOGGLE SLOW CW
  if (slowCW == LOW && lastSlowCW == HIGH) {
    slowRunning = !slowRunning;
    if (slowRunning) startSlowMove(1);
  }

  // TOGGLE SLOW CCW
  if (slowCCW == LOW && lastSlowCCW == HIGH) {
    slowRunning = !slowRunning;
    if (slowRunning) startSlowMove(-1);
  }

  lastSlowCW = slowCW;
  lastSlowCCW = slowCCW;

  if (slowRunning) {
    long travelled = abs(stepper.currentPosition() - slowStartPos);
    if (travelled < slowTargetSteps) {
      digitalWrite(EnablePin, LOW);
      stepper.setSpeed(slowSpeed * slowDirection);
      stepper.runSpeed();
    } else {
      slowRunning = false;
    }
  } else {
    digitalWrite(EnablePin, HIGH); // driver OFF -> no holding current
  }
}
