// ESP32 ANIMATRONIC EYES — PROJECT SKETCH
// BLUE = pin numbers / connections | TEAL = servo positions | PURPLE = comments / helper functions
// ORANGE = setup (runs once) | GREEN = loop (repeats continuously)
// LIBRARIES: extra code that lets the ESP32 use the Xbox controller and servos.
#include <Bluepad32.h>
#include <ESP32Servo.h>

const int RIGHT_EYE_PIN = 13;
const int RIGHT_LID_PIN = 14;
const int LEFT_EYE_PIN = 19;
const int LEFT_LID_PIN = 25;
const int DIODE_LED_PIN = 33;
const int STRIP_LED_PIN = 32;

const int LEFT_CENTER = 82;
const int LEFT_LOOK_LEFT = 110;
const int LEFT_LOOK_RIGHT = 48;
const int RIGHT_CENTER = 95;
const int RIGHT_LOOK_LEFT = 120;
const int RIGHT_LOOK_RIGHT = 60;

const int RIGHT_LID_OPEN = 76;
const int RIGHT_LID_CLOSED = 165;
const int LEFT_LID_OPEN = 31;
const int LEFT_LID_CLOSED = 130;

Servo rightEye, leftEye, rightLid, leftLid;
ControllerPtr myController = nullptr;

int eyeDirection = 0; // 0=joystick, -1=left, 1=right
bool previousX = false, previousB = false;
bool previousA = false, previousY = false;
bool previousLeftBumper = false, previousRightBumper = false;
bool previousLeftTrigger = false, previousRightTrigger = false;
bool diodeLEDOn = false, stripLEDOn = false;

bool leftBlinking = false, rightBlinking = false, bothBlinking = false;
unsigned long blinkStartTime = 0;
const unsigned long BLINK_CLOSE_TIME = 180;
const unsigned long BLINK_OPEN_TIME = 260; // Defined but not used in this version

void onConnectedController(ControllerPtr ctl) {
  myController = ctl;
  Serial.println("Xbox controller connected.");
}
void onDisconnectedController(ControllerPtr ctl) {
  if (myController == ctl) myController = nullptr;
  Serial.println("Xbox controller disconnected.");
}

// FUNCTIONS: named mini-jobs the rest of the program can call.
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

// JOYSTICK: turns left-stick movement into eye angles.
void updateJoystick() {
  if (eyeDirection != 0) return;
  int x = myController->axisX();
  if (abs(x) < 30) return;

  int leftAngle, rightAngle;
  if (x < 0) {
    int amount = constrain(map(-x, 30, 512, 0, 100), 0, 100);
    leftAngle = map(amount, 0, 100, LEFT_CENTER, LEFT_LOOK_LEFT);
    rightAngle = map(amount, 0, 100, RIGHT_CENTER, RIGHT_LOOK_LEFT);
  } else {
    int amount = constrain(map(x, 30, 512, 0, 100), 0, 100);
    leftAngle = map(amount, 0, 100, LEFT_CENTER, LEFT_LOOK_RIGHT);
    rightAngle = map(amount, 0, 100, RIGHT_CENTER, RIGHT_LOOK_RIGHT);
  }
  leftEye.write(leftAngle);
  rightEye.write(rightAngle);
}

void openBothLids() {
  leftLid.write(LEFT_LID_OPEN);
  rightLid.write(RIGHT_LID_OPEN);
}
void closeBothLids() {
  leftLid.write(LEFT_LID_CLOSED);
  rightLid.write(RIGHT_LID_CLOSED);
}
void startBothBlink() {
  closeBothLids();
  bothBlinking = true;
  leftBlinking = rightBlinking = false;
  blinkStartTime = millis();
}
void startLeftBlink() {
  leftLid.write(LEFT_LID_CLOSED);
  leftBlinking = true;
  bothBlinking = false;
  blinkStartTime = millis();
}
void startRightBlink() {
  rightLid.write(RIGHT_LID_CLOSED);
  rightBlinking = true;
  bothBlinking = false;
  blinkStartTime = millis();
}
// BLINK TIMER: reopens lids after the short blink delay.
void updateBlinking() {
  if (bothBlinking && millis() - blinkStartTime >= BLINK_CLOSE_TIME) {
    openBothLids();
    bothBlinking = false;
  }
  if (leftBlinking && millis() - blinkStartTime >= BLINK_CLOSE_TIME) {
    leftLid.write(LEFT_LID_OPEN);
    leftBlinking = false;
  }
  if (rightBlinking && millis() - blinkStartTime >= BLINK_CLOSE_TIME) {
    rightLid.write(RIGHT_LID_OPEN);
    rightBlinking = false;
  }
}

// BUTTON HANDLER: checks which controller buttons/triggers were pressed.
void handleButtons() {
  bool xPressed = myController->x();
  bool bPressed = myController->b();
  bool aPressed = myController->a();
  bool yPressed = myController->y();

  if (xPressed && !previousX) {
    if (eyeDirection == -1) {
      eyeDirection = 0;
      centerEyes();
      Serial.println("X: Both eyes centered; joystick enabled.");
    } else {
      eyeDirection = -1;
      moveEyesLeft();
      Serial.println("X: Both eyes looking LEFT.");
    }
  }
  if (bPressed && !previousB) {
    if (eyeDirection == 1) {
      eyeDirection = 0;
      centerEyes();
      Serial.println("B: Both eyes centered; joystick enabled.");
    } else {
      eyeDirection = 1;
      moveEyesRight();
      Serial.println("B: Both eyes looking RIGHT.");
    }
  }
  if (aPressed && !previousA) {
    eyeDirection = 0;
    centerEyes();
    openBothLids();
    bothBlinking = leftBlinking = rightBlinking = false;
    Serial.println("A: Eyes centered, lids open.");
  }
  if (yPressed && !previousY) startBothBlink();

  bool leftBumper = myController->l1();
  bool rightBumper = myController->r1();
  if (leftBumper && !previousLeftBumper) {
    diodeLEDOn = !diodeLEDOn;
    digitalWrite(DIODE_LED_PIN, diodeLEDOn);
    Serial.println(diodeLEDOn ? "Diode LEDs ON" : "Diode LEDs OFF");
  }
  if (rightBumper && !previousRightBumper) {
    stripLEDOn = !stripLEDOn;
    digitalWrite(STRIP_LED_PIN, stripLEDOn);
    Serial.println(stripLEDOn ? "LED strips ON" : "LED strips OFF");
  }

  bool leftTrigger = myController->brake() > 100;
  bool rightTrigger = myController->throttle() > 100;
  if (leftTrigger && !previousLeftTrigger) startLeftBlink();
  if (rightTrigger && !previousRightTrigger) startRightBlink();

  previousX = xPressed;
  previousB = bPressed;
  previousA = aPressed;
  previousY = yPressed;
  previousLeftBumper = leftBumper;
  previousRightBumper = rightBumper;
  previousLeftTrigger = leftTrigger;
  previousRightTrigger = rightTrigger;
}

// SETUP: runs once when the ESP32 starts; prepares pins, servos and Bluetooth.
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
  BP32.setup(&onConnectedController, &onDisconnectedController);
  BP32.enableNewBluetoothConnections(true);
  Serial.println("Animatronic eyes ready.");
  Serial.println("X = both eyes left; press X again to center.");
  Serial.println("B = both eyes right; press B again to center.");
}

// LOOP: repeats constantly; checks controller input and updates blinking.
void loop() {
  BP32.update();
  if (myController && myController->isConnected()) {
    handleButtons();
    updateJoystick();
  }
  updateBlinking();
  delay(5);
}
