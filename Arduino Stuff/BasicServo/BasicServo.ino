#include <Servo.h>

#define SERVO_1_PIN 0

Servo servo1

void setup() {

  Serial.begin(115200);
  servo1.attach(SERVO_1_PIN);
  
}

void loop() {

  if (Serial.available() > 0) {
  int a = Serial.parseInt();
  servo1.write(a);
  }
}

