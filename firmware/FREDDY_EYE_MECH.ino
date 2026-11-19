#include <Bluepad32.h>
#include <ESP32Servo.h>

// ==================== SERVO PINS ====================
const int RIGHT_EYE_PIN = 13;
const int RIGHT_LID_PIN = 14;
const int LEFT_EYE_PIN  = 19;
const int LEFT_LID_PIN  = 25;

// ==================== LED OUTPUTS ====================
// GPIO 32 = individual diode LEDs
// GPIO 33 = LED strips
const int DIODE_LED_PIN = 32;
const int STRIP_LED_PIN = 33;

// ==================== SERVO CALIBRATION ====================
const int LEFT_CENTER = 82;
const int LEFT_LOOK_LEFT = 110;
const int LEFT_LOOK_RIGHT = 48;

const int RIGHT_CENTER = 95;
const int RIGHT_LOOK_LEFT = 120;
const int RIGHT_LOOK_RIGHT = 60;

const int RIGHT_LID_OPEN = 31;
const int LEFT_LID_OPEN = 31;

const int RIGHT_LID_OPEN_ANGLE = 76;
const int RIGHT_LID_CLOSED = 165;
const int LEFT_LID_OPEN_ANGLE = 31;
const int LEFT_LID_CLOSED = 130;

// ==================== SERVOS ====================
Servo rightEye;
Servo leftEye;
Servo rightLid;
Servo leftLid;

// ==================== CONTROLLER ====================
ControllerPtr myController = nullptr;

// ==================== EYE STATE ====================
int eyeDirection = 0;

bool previousX = false;
bool previousB = false;
bool previousA = false;
bool previousY = false;
bool previousStart = false;

bool previousLeftBumper = false;
bool previousRightBumper = false;
bool previousLeftTrigger = false;
bool previousRightTrigger = false;
bool previousBothTriggers = false;

// ==================== LED MODES ====================
// RB cycles: OFF -> SOLID -> FLICKER -> OFF
enum StripMode {
  STRIP_OFF,
  STRIP_SOLID,
  STRIP_FLICKER
};

StripMode stripMode = STRIP_OFF;
bool diodeLEDOn = false;

// ==================== AUTOPILOT ====================
bool idleAutopilot = false;

unsigned long lastIdleEyeChange = 0;
unsigned long idleEyeInterval = 15000;
unsigned long nextIdleBlink = 0;

// ==================== BLINK TIMING ====================
bool leftBlinking = false;
bool rightBlinking = false;
bool bothBlinking = false;

unsigned long blinkStartTime = 0;
const unsigned long BLINK_CLOSE_TIME = 180;

// ==================== LED FLICKER ENGINE ====================
bool ledBurstActive = false;
bool ledBurstOutputOn = false;

unsigned long ledBurstEndTime = 0;
unsigned long nextLedToggle = 0;
unsigned long nextManualBurst = 0;
unsigned long nextAutoBurst = 0;

// Forward declaration
void startLedFlickerBurst();

// ==================== CONNECTION CALLBACKS ====================
void onConnectedController(ControllerPtr ctl) {
  myController = ctl;
  Serial.println("Xbox controller connected.");
}

void onDisconnectedController(ControllerPtr ctl) {
  if (myController == ctl) {
    myController = nullptr;
  }

  idleAutopilot = false;
  eyeDirection = 0;

  centerEyes();

  ledBurstActive = false;
  ledBurstOutputOn = false;
  digitalWrite(STRIP_LED_PIN, LOW);

  Serial.println("Xbox controller disconnected.");
}

// ==================== EYE MOVEMENT ====================
void moveEyesLeft() {
  leftEye.write(LEFT_LOOK_LEFT);
  rightEye.write(RIGHT_LOOK_LEFT);
}

void moveEyesRight() {
  leftEye.write(LEFT_LOOK_RIGHT);
  rightEye.write(RIGHT_LOOK_RIGHT);
}

void centerEyes() {
  leftEye.write(LEFT_CENTER);
  rightEye.write(RIGHT_CENTER);
}

// ==================== BLINK HELPERS ====================
bool anyBlinking() {
  return leftBlinking || rightBlinking || bothBlinking;
}

void openBothLids() {
  leftLid.write(LEFT_LID_OPEN_ANGLE);
  rightLid.write(RIGHT_LID_OPEN_ANGLE);
}

void closeBothLids() {
  leftLid.write(LEFT_LID_CLOSED);
  rightLid.write(RIGHT_LID_CLOSED);
}

void startBothBlink() {
  closeBothLids();

  bothBlinking = true;
  leftBlinking = false;
  rightBlinking = false;

  blinkStartTime = millis();
}

void startLeftBlink() {
  leftLid.write(LEFT_LID_CLOSED);

  leftBlinking = true;
  bothBlinking = false;
  rightBlinking = false;

  blinkStartTime = millis();
}

void startRightBlink() {
  rightLid.write(RIGHT_LID_CLOSED);

  rightBlinking = true;
  bothBlinking = false;
  leftBlinking = false;

  blinkStartTime = millis();
}

void updateBlinking() {
  if (!anyBlinking()) {
    return;
  }

  if (millis() - blinkStartTime >= BLINK_CLOSE_TIME) {
    if (bothBlinking || leftBlinking) {
      leftLid.write(LEFT_LID_OPEN_ANGLE);
    }

    if (bothBlinking || rightBlinking) {
      rightLid.write(RIGHT_LID_OPEN_ANGLE);
    }

    bothBlinking = false;
    leftBlinking = false;
    rightBlinking = false;
  }
}

// ==================== RANDOM EYE AUTOPILOT ====================
unsigned long chooseIdleEyeInterval() {
  // 25% chance of a longer pause.
  if (random(0, 100) < 25) {
    return random(20000, 35001);
  }

  return random(15000, 20001);
}

int chooseIdleEyeDirection() {
  long choice = random(0, 100);

  if (choice < 35) return 0;
  if (choice < 68) return -1;

  return 1;
}

void setIdleEyeDirection(int direction) {
  if (direction == -1) {
    moveEyesLeft();
  } else if (direction == 1) {
    moveEyesRight();
  } else {
    centerEyes();
  }
}

void scheduleIdleBlink() {
  nextIdleBlink = millis() + random(4000, 10001);
}

void startIdleAutopilot() {
  idleAutopilot = true;
  eyeDirection = 0;

  centerEyes();

  lastIdleEyeChange = millis();
  idleEyeInterval = chooseIdleEyeInterval();

  scheduleIdleBlink();

  // First automatic LED flicker after a random delay.
  nextAutoBurst = millis() + random(5000, 16001);

  Serial.println("IDLE AUTOPILOT ON");
}

void stopIdleAutopilot() {
  idleAutopilot = false;
  eyeDirection = 0;

  centerEyes();

  ledBurstActive = false;
  ledBurstOutputOn = false;

  Serial.println("IDLE AUTOPILOT OFF");
}

void updateIdleAutopilot() {
  if (!idleAutopilot) return;

  unsigned long now = millis();

  // Random eye movement with long pauses.
  if (now - lastIdleEyeChange >= idleEyeInterval) {
    int direction = chooseIdleEyeDirection();

    setIdleEyeDirection(direction);

    lastIdleEyeChange = now;
    idleEyeInterval = chooseIdleEyeInterval();

    Serial.print("Autopilot gaze: ");

    if (direction == -1) {
      Serial.println("LEFT");
    } else if (direction == 1) {
      Serial.println("RIGHT");
    } else {
      Serial.println("CENTER");
    }
  }

  // Random autonomous blinks.
  if ((long)(now - nextIdleBlink) >= 0) {
    if (!anyBlinking()) {
      startBothBlink();
    }

    scheduleIdleBlink();
  }

  // Random LED flicker bursts during autopilot.
  if ((long)(now - nextAutoBurst) >= 0) {
    if (!ledBurstActive) {
      startLedFlickerBurst();
    }

    nextAutoBurst = now + random(8000, 25001);
  }
}

// ==================== LED FLICKER ====================

// Start one irregular flicker burst.
void startLedFlickerBurst() {
  ledBurstActive = true;

  // Random starting state.
  ledBurstOutputOn = random(0, 2) == 1;

  // Longer bursts: 2.5 to 6 seconds.
  ledBurstEndTime = millis() + random(2500, 6001);

  // Irregular flicker changes every 30 to 180 ms.
  nextLedToggle = millis() + random(30, 181);
}

// Update flicker without blocking the ESP32.
void updateLedFlicker() {
  unsigned long now = millis();

  // Manual flicker repeats with only 1–2 seconds of darkness.
  if (stripMode == STRIP_FLICKER &&
      !ledBurstActive &&
      (long)(now - nextManualBurst) >= 0) {
    startLedFlickerBurst();
  }

  // Update or finish an active flicker burst.
  if (ledBurstActive) {
    if ((long)(now - ledBurstEndTime) >= 0) {
      ledBurstActive = false;
      ledBurstOutputOn = false;

      // Short, randomized pause between manual bursts.
      nextManualBurst = now + random(1000, 2001);
    } else if ((long)(now - nextLedToggle) >= 0) {
      // Randomly switch the strip on or off.
      // The irregular timing avoids rhythmic blinking.
      ledBurstOutputOn = random(0, 100) < 55;

      nextLedToggle = now + random(30, 181);
    }
  }

  // Determine the final strip output.
  bool stripOutput = false;

  if (ledBurstActive) {
    stripOutput = ledBurstOutputOn;
  } else if (stripMode == STRIP_SOLID) {
    stripOutput = true;
  } else {
    stripOutput = false;
  }

  // Flicker controls the LED strips on GPIO 33 only.
  digitalWrite(STRIP_LED_PIN, stripOutput ? HIGH : LOW);
}

// ==================== STRIP MODE CONTROL ====================
void cycleStripMode() {
  if (stripMode == STRIP_OFF) {
    stripMode = STRIP_SOLID;

    ledBurstActive = false;
    ledBurstOutputOn = false;

    Serial.println("LED STRIPS: SOLID ON");
  } else if (stripMode == STRIP_SOLID) {
    stripMode = STRIP_FLICKER;
    nextManualBurst = millis() + random(100, 501);

    Serial.println("LED STRIPS: RANDOM FLICKER");
  } else {
    stripMode = STRIP_OFF;

    ledBurstActive = false;
    ledBurstOutputOn = false;

    Serial.println("LED STRIPS: OFF");
  }

  nextManualBurst = millis() + random(1000, 2001);
}

// Both triggers toggle flicker mode.
void toggleTriggerFlicker() {
  if (stripMode == STRIP_FLICKER) {
    stripMode = STRIP_OFF;

    ledBurstActive = false;
    ledBurstOutputOn = false;

    digitalWrite(STRIP_LED_PIN, LOW);

    Serial.println("TRIGGER COMBO: LED FLICKER OFF");
  } else {
    stripMode = STRIP_FLICKER;

    ledBurstActive = false;
    ledBurstOutputOn = false;

    // Begin the first burst shortly after activation.
    nextManualBurst = millis() + random(100, 501);

    Serial.println("TRIGGER COMBO: LED FLICKER ON");
  }
}

// ==================== JOYSTICK CONTROL ====================
void updateJoystick() {
  if (idleAutopilot || eyeDirection != 0) return;

  int x = myController->axisX();

  if (abs(x) < 30) return;

  int leftAngle;
  int rightAngle;

  if (x < 0) {
    int amount = constrain(map(-x, 30, 512, 0, 100), 0, 100);

    leftAngle = map(
      amount, 0, 100, LEFT_CENTER, LEFT_LOOK_LEFT
    );

    rightAngle = map(
      amount, 0, 100, RIGHT_CENTER, RIGHT_LOOK_LEFT
    );
  } else {
    int amount = constrain(map(x, 30, 512, 0, 100), 0, 100);

    leftAngle = map(
      amount, 0, 100, LEFT_CENTER, LEFT_LOOK_RIGHT
    );

    rightAngle = map(
      amount, 0, 100, RIGHT_CENTER, RIGHT_LOOK_RIGHT
    );
  }

  leftEye.write(leftAngle);
  rightEye.write(rightAngle);
}

// ==================== BUTTON HANDLING ====================
void handleButtons() {
  bool xPressed = myController->x();
  bool bPressed = myController->b();
  bool aPressed = myController->a();
  bool yPressed = myController->y();

  bool startPressed =
      myController->miscButtons() & MISC_BUTTON_START;

  if (startPressed && !previousStart) {
    if (idleAutopilot) {
      stopIdleAutopilot();
    } else {
      startIdleAutopilot();
    }
  }

  // Manual eye controls are disabled during autopilot.
  if (!idleAutopilot) {
    if (xPressed && !previousX) {
      if (eyeDirection == -1) {
        eyeDirection = 0;
        centerEyes();
        Serial.println("X: Eyes centered.");
      } else {
        eyeDirection = -1;
        moveEyesLeft();
        Serial.println("X: Eyes LEFT.");
      }
    }

    if (bPressed && !previousB) {
      if (eyeDirection == 1) {
        eyeDirection = 0;
        centerEyes();
        Serial.println("B: Eyes centered.");
      } else {
        eyeDirection = 1;
        moveEyesRight();
        Serial.println("B: Eyes RIGHT.");
      }
    }

    if (aPressed && !previousA) {
      eyeDirection = 0;
      centerEyes();

      bothBlinking = false;
      leftBlinking = false;
      rightBlinking = false;

      openBothLids();

      Serial.println("A: Center eyes and open lids.");
    }
  }

  // Y manually blinks both eyelids.
  if (yPressed && !previousY) {
    startBothBlink();

    if (idleAutopilot) {
      scheduleIdleBlink();
    }
  }

  // LB toggles the individual diode LEDs on GPIO 32.
  bool leftBumper = myController->l1();
  bool rightBumper = myController->r1();

  if (leftBumper && !previousLeftBumper) {
    diodeLEDOn = !diodeLEDOn;

    digitalWrite(
      DIODE_LED_PIN,
      diodeLEDOn ? HIGH : LOW
    );

    Serial.println(
      diodeLEDOn ? "Diode LEDs ON" : "Diode LEDs OFF"
    );
  }

  // RB cycles strip modes: OFF -> SOLID -> FLICKER -> OFF.
  if (rightBumper && !previousRightBumper) {
    cycleStripMode();
  }

  // Read analog triggers.
  bool leftTrigger = myController->brake() > 100;
  bool rightTrigger = myController->throttle() > 100;
  bool bothTriggers = leftTrigger && rightTrigger;

  // Both triggers together toggle LED-strip flicker.
  // Suppress individual eyelid actions for the combo.
  if (bothTriggers && !previousBothTriggers) {
    toggleTriggerFlicker();
  } else {
    // A single trigger blinks its corresponding eyelid.
    if (leftTrigger && !previousLeftTrigger &&
        !rightTrigger) {
      startLeftBlink();

      if (idleAutopilot) {
        scheduleIdleBlink();
      }
    }

    if (rightTrigger && !previousRightTrigger &&
        !leftTrigger) {
      startRightBlink();

      if (idleAutopilot) {
        scheduleIdleBlink();
      }
    }
  }

  // Remember states for edge detection.
  previousX = xPressed;
  previousB = bPressed;
  previousA = aPressed;
  previousY = yPressed;
  previousStart = startPressed;

  previousLeftBumper = leftBumper;
  previousRightBumper = rightBumper;

  previousLeftTrigger = leftTrigger;
  previousRightTrigger = rightTrigger;
  previousBothTriggers = bothTriggers;
}

// ==================== SETUP ====================
void setup() {
  Serial.begin(115200);

  pinMode(DIODE_LED_PIN, OUTPUT);
  pinMode(STRIP_LED_PIN, OUTPUT);

  digitalWrite(DIODE_LED_PIN, LOW);
  digitalWrite(STRIP_LED_PIN, LOW);

  rightEye.setPeriodHertz(50);
  leftEye.setPeriodHertz(50);
  rightLid.setPeriodHertz(50);
  leftLid.setPeriodHertz(50);

  rightEye.attach(RIGHT_EYE_PIN, 500, 2500);
  rightLid.attach(RIGHT_LID_PIN, 500, 2500);
  leftEye.attach(LEFT_EYE_PIN, 500, 2500);
  leftLid.attach(LEFT_LID_PIN, 500, 2500);

  centerEyes();
  openBothLids();

  randomSeed(micros());

  nextManualBurst = millis() + random(100, 501);

  BP32.setup(
    &onConnectedController,
    &onDisconnectedController
  );

  BP32.enableNewBluetoothConnections(true);

  Serial.println("Animatronic eyes ready.");
  Serial.println("START = Toggle idle autopilot.");
  Serial.println("X = Eyes left / center.");
  Serial.println("B = Eyes right / center.");
  Serial.println("A = Center eyes and open lids.");
  Serial.println("Y = Blink both lids.");
  Serial.println("LB = Toggle diode LEDs (GPIO 32).");
  Serial.println("RB = Cycle strip modes (GPIO 33).");
  Serial.println("BOTH TRIGGERS = Toggle strip flicker.");
}

// ==================== MAIN LOOP ====================
void loop() {
  BP32.update();

  if (myController && myController->isConnected()) {
    handleButtons();

    if (idleAutopilot) {
      updateIdleAutopilot();
    } else {
      updateJoystick();
    }
  }

  updateBlinking();
  updateLedFlicker();

  delay(5);
}