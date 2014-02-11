#include <PID_v1.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <Servo.h>

#define FORWARD HIGH
#define BACKWARD LOW

// Variables!
double motorValues[] = {0, 0, 0, 0, 0};
#define NUM_BUTTONS 7
double buttonValues[] = {0, 0, 0, 0, 0, 0, 0};
double cameraServoTiltVal = 0;
double cameraServoRollVal = 0;
double clawServoVal       = 0;

//Servos!
#define CAMERA_TILT_PIN 0
#define CAMERA_ROLL_PIN 0
#define CLAW_SERVO_PIN 0
Servo cameraTilt;
Servo cameraRoll;
Servo claw;

// Motor Pins!
#define motor1Pin 0
#define motor2Pin 0
#define motor3Pin 0
#define motor4Pin 0
#define motorUp1Pin 0
#define motorUp2Pin 0
#define motor1PinDir 0
#define motor2PinDir 0
#define motor3PinDir 0
#define motor4PinDir 0
#define motorUp1PinDir 0
#define motorUp2PinDir 0
int motorValuePins[] = {motor1Pin, motor2Pin, motor3Pin, motor4Pin, motorUp1Pin, motorUp2Pin};
int motorDirPins[] = {motor1PinDir, motor2PinDir, motor3PinDir, motor4PinDir, motorUp1PinDir, motorUp2PinDir};

String inputData = "";

boolean is_PID_on = false;
double setpoint = 101000;
double sensor = 0;
double depth = 0;
PID pid(&depth, &motorValues[4], &setpoint, 1, 1, 1, DIRECT);

Adafruit_BMP085 bmp;

void setup() {
  initSensors();
  initSerial();
  initPID();
  for (int i = 0; i < 6; i++) {
    pinMode(motorValuePins[i], OUTPUT);
    pinMode(motorDirPins[i], OUTPUT);
  }
  initServos();
}

void loop() {
  = recievePacket();
  parseThePacket();
  if (is_PID_on == true) {
    adjustDepthSetpoint();
    getSensorData();
    pid.Compute();
  }
  
  //actOnDepthValues();
  act();
}

void initSensors() {
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085. Feed me kittens");
    while(1) {}
  }
}

void initSerial() {
  // Initialize the serial
  Serial.begin(115200);
}

//intializes the PID
void initPID() {
  pid.SetMode(AUTOMATIC);
}

void initServos() {
  clawServo.attach(CLAW_SERVO_PIN);
  cameraTilt.attach(CAMERA_TILT_PIN);
  cameraRoll.attach(CAMREA_ROLL_PIN);
}

String recievePacket() {
  int index = 0;
  char data[32];
  
  while (Serial.available() <= 0 || Serial.read() != '!') {}
  delay(1);
  while (Serial.available() > 0) {
    char input = Serial.read();
    if (input == '$') { break; }
    data[index] = input;
    index ++;
    delay(1);
 }
 data[index] = '\0';
 inputData = String(data);
}

void parseThePacket() {
  parseMotorValues();
  parseButtonValues();
}

void parseMotorValues() {
  for (int i = 1; i <= 5; i++) {
    motorValues[i-1] = getMotorValueAt(i);
  }
}

void parseButtonValues() {
  for (int i = 1; i <= NUM_BUTTONS; i++) {
    buttonValues[i-1] = getButtonValueAt(i);
  }
  
  // Check if PID is still on
  if (buttonValues[6] == 1) {
    is_PID_on = true;
  } else {
    is_PID_on = false;
  }
  
  //Increment all the servo values
  //  Camera Roll
  if (buttonValues[0] == 1) {
    cameraServoTiltVal -= 1;
    cameraServoTiltVal = constrain(cameraServoTiltVal, 0, 179);
  }
  if (buttonValues[1] == 1) {
    cameraServoTiltVal += 1;
    cameraServoTiltVal = constrain(cameraServoTiltVal, 0, 179);
  }
  
  //   Camera Tilt
  if (buttonValues[2] == 1) {
    cameraServoRollVal -= 1;
    cameraServoRollVal = constrain(cameraServoRollVal, 0, 179);
  }
  if (buttonValues[3] == 1) {
    cameraServoRollVal += 1;
    cameraServoRollVal = constrain(cameraServoRollVal, 0, 179);
  }
  
  //   Claw
  if (buttonValues[4] == 1) {
    clawServoVal -= 1;
    clawServoVal = constrain(clawServoVal, 0, 179);
  }
  if (buttonValues[5] == 1) {
    clawServoVal += 1;
    clawServoVal = constrain(clawServoVal, 0, 179);
  }
  
}

int getMotorValueAt(int motorNumber) {
  int placeHolder = 0;
  
  switch (motorNumber) {
    case 1:
      placeHolder = 0;
      break;
    case 2:
      placeHolder = 4;
      break;
    case 3:
      placeHolder = 8;
      break;
    case 4:
      placeHolder = 12;
      break;
    case 5:
      placeHolder = 16;
      break;
  }
  String val = inputData.substring(placeHolder, placeHolder + 3);
  return val.toInt();
}

boolean getButtonValueAt(int buttonNumber) {
  int placeHolder = 0;
  
  switch (buttonNumber) {
    case 1:
      placeHolder = 22;
      break;
    case 2:
      placeHolder = 24;
      break;
    case 3:
      placeHolder = 26;
      break;
    case 4:
      placeHolder = 28;
      break;
    case 5:
      placeHolder = 30;
      break;
    case 6:
      placeHolder = 32;
      break;
    case 7:
      placeHolder = 34;
      break;
  }
  String val = inputData.substring(placeHolder, placeHolder + 1);
  return val.toInt();
}

// if the PID is on this will allow the user to manually adjust the depth setpoint
void adjustDepthSetpoint() {
  //Adjust depth setpoint
  double adjustment = motorValues[4];
  adjustment = map(adjustment, -256, 256, -10, 10);
  setpoint += adjustment;
}

void getSensorData() {
  sensor = bmp.readPressure();
  depth = (sensor - 101500)/101500;
  Serial.print("D ");
  Serial.println(depth);
}

void act() {
  // Act
  actOnMotors();
  actOnServos();
}

void actOnMotors() {
  //Deal with horizontal motors
  for (int i = 0; i < 4; i++) {
    int a = motorValues[i];
    a -= 256;
    if (a < 0) {
      a = abs(a);
      a = constrain(a, 80, 256);
      analogWrite(motorValuePins[i], a);
      digitalWrite(motorDirPins[i], BAKCKWARD;
    } else {
      a = abs(a);
      a = constrain(a, 80, 256);
      analogWrite(motorValuePins[i], a);
      digitalWrite(motorDirPins[i], FORWARD);
    }
  }
  
  //Now deal with those pesky depth motors
  int a = motorValues[4];
  a -= 256;
  if (a < 0) {
    a = abs(a);
    a = constrain(a, 80, 256);
    analogWrite(motorValuePins[4], a);
    digitalWrite(motorDirPins[4], BACKWARD);
    analogWrite(motorValuePins[5], a);
    digitalWrite(motorDirPins[5], BACKWARD);
  } else {
    a = abs(a);
    a = constrain(a, 80, 256);
    analogWrite(motorValuePins[4], a);
    digitalWrite(motorDirPins[4], FORWARD);
    analogWrite(motorValuePins[5], a);
    digitalWrite(motorDirPins[5], FORWARD);
  }
}

void actOnServos() {
  claw.write(clawServoVal);
  cameraTilt.write(cameraServoTiltVal);
  cameraRoll.write(cameraServoRollVal);
}
