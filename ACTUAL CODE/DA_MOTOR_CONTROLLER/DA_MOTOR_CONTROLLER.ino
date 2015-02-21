#include <Servo.h>
//###################################
//###################################
// Constants
//###################################
//###################################

//************************************
// Miscellaneous
//************************************

#define FORWARD HIGH
#define BACKWARD LOW

//************************************
// Motor pins
//************************************

#define NUM_MOTORS 6

//Motor thruster pins
// ADJUST THESE WHEN PINS ARE FINALIZED
#define MT1t_PIN 11
#define MT2t_PIN 9
#define MT3t_PIN 7
#define MT4t_PIN 5
#define MT5t_PIN 2
#define MT6t_PIN 12 // 6 Controls vertical motors
int motor_thrust_pins[] = {MT1t_PIN, MT2t_PIN, MT3t_PIN, MT4t_PIN, MT5t_PIN, MT6t_PIN};

//Motor direction pins
// ADJUST THESE WHEN PINS ARE FINALIZED
#define MT1d_PIN 10
#define MT2d_PIN 1
#define MT3d_PIN 60
#define MT4d_PIN 4
#define MT5d_PIN 1
#define MT6d_PIN 1
int motor_dir_pins[] = {MT1d_PIN, MT2d_PIN, MT3d_PIN, MT4d_PIN, MT5d_PIN, MT6d_PIN};

//************************************
// Servo Values (unsigned 0-90 integers)
//************************************

#define SERVO_1_PIN 41
#define SERVO_2_PIN 39

Servo servo1;
Servo servo2;

//###################################
//###################################
// Global Variables
//###################################
//###################################

//************************************
// Motor Values (signed 8-bit integers)
//************************************

int motor_values[] = {0, 0, 0, 0, 0};
int servo_values[] = {0, 0};


//###################################
//###################################
// Code!
//###################################
//###################################

void setup() {
  //*********************************
  // Initialize Systems
  //*********************************
  
  // Init Serial
  Serial.begin(115200);
  Serial.println("Serial initialized");

  // Set thrust pins to output
  pinMode(13, OUTPUT);
  pinMode(MT1t_PIN, OUTPUT);
  pinMode(MT2t_PIN, OUTPUT);
  pinMode(MT3t_PIN, OUTPUT);
  pinMode(MT4t_PIN, OUTPUT);
  pinMode(MT5t_PIN, OUTPUT);
  pinMode(MT6t_PIN, OUTPUT);
  Serial.println("Motors initialized");
  
//  // Set direction pins to output
  pinMode(13, OUTPUT);
  pinMode(MT1d_PIN, OUTPUT);
  pinMode(MT2d_PIN, OUTPUT);
  pinMode(MT3d_PIN, OUTPUT);
  pinMode(MT4d_PIN, OUTPUT);
  pinMode(MT5d_PIN, OUTPUT);
  pinMode(MT6d_PIN, OUTPUT);
  Serial.println("Direction initialized");
  
//  // Attach servos
  servo1.attach(SERVO_1_PIN);
  servo2.attach(SERVO_2_PIN);
  
  //*********************************
  // Test Systems
  //*********************************
  
  // Blink to acknowledge
  digitalWrite(13, HIGH);
  delay(500);
  digitalWrite(13, LOW);
  delay(500);
  digitalWrite(13, HIGH);
  delay(500);
  digitalWrite(13, LOW);
    
  //Test motors
  for (int i = 0; i < NUM_MOTORS; i++) {
    Serial.print("Testing motor ");
    Serial.println(i);
    
    for (int j = 0; j < 256; j++) {
      analogWrite(motor_thrust_pins[i], j);
      delay(1);
    }
    
    for (int j = 255; j >= 0; j--) {
      analogWrite(motor_thrust_pins[i], j);
      delay(1);
    }
  }
  
  Serial.println("Testing servos...");
  //Test servos
  //  #1
  servo1.write(70);
  delay(500);
  servo1.write(100);
  //  #2
  servo2.write(30);
  delay(500);
  servo2.write(50);
  Serial.println("Servos tested");
  
} // End setup

void loop() {
  
  //*********************************
  // Receive serial input
  //*********************************
  Serial.println("Waiting for values...");
  while (Serial.available() <= 0 || (char)Serial.read() != '!') {
    // Wait for start signal...
  }
  delay(1); // To give the next byte a chance to come in
  // By this point we know that either there is nothing in the RX buffer
  //   or the first byte in the buffer is the second byte of the message
  //   (as we already read the first byte to see if it was a '!')
  char inputString[40];
  int i = 0;
  while (Serial.available() > 0) {
    char in_char = (char)Serial.read();
    
    if (in_char == '$') {
      break; // Stop byte recieved, breaking out
    }
    
    inputString[i] = in_char;
    i++;
    delay(1);
  }
  
  delay(1);
  
  Serial.println("Values received: ");
  Serial.println(inputString);
  
  //************************************
  // Parse the received values
  //
  //  Reference Message:
  //    !123/123/123/123/123/123/000/000$
  //  note: ! is received, but is not stored
  //************************************
  
  for (int i = 0; i < NUM_MOTORS; i++) {
    char input_buffer[4];
    for (int j = 0; j < 3; j++) {
      input_buffer[j] = inputString[i*4 + j];
    }
    input_buffer[3] = '\0';
    int m_val = atoi(input_buffer);
    Serial.print("Parsing value: ");
    Serial.println(m_val);
    motor_values[i] = m_val - 256;
  }
  
  // For debugging, print out the motor values
  Serial.print("Parsed motor values: {");
  for (int i = 0; i < NUM_MOTORS; i++) {
    Serial.print(motor_values[i]);
    Serial.print(", ");
  }
  Serial.println("}");
  
  // Parse servo values
  for (int i = 0; i < 2; i++) {
    char input_buffer[4];
    for (int j = 0; j < 3; j++) {
      input_buffer[j] = inputString[24 + i*4 + j];
    }
    input_buffer[3] = '\0';
    int s_val = atoi(input_buffer);
    Serial.print("Parsing servo value: ");
    Serial.println(s_val);
    servo_values[i] = s_val;
  }
    
  //*********************************
  // Write out motor values
  //*********************************
  
  for (int i = 0; i < NUM_MOTORS; i++){
    int motorSpeed = motor_values[i];
    
    controlMotor(i, motorSpeed);
    
    delay(1);
  }
  
  // Write out servo values
  Serial.print("Writing ");
  Serial.print(servo_values[0]);
  Serial.println(" to servo 1");
  servo1.write(servo_values[0]);
  
  Serial.print("Writing ");
  Serial.print(servo_values[1]);
  Serial.println(" to servo 2");
  servo2.write(servo_values[1]);

}
//Returns True if the motor speed was successfully set
//Returns False if the method failed to set the motor speed (e.g. if you said set motor number 12 to a speed of 10000)
//motorNum is a number in the set {0,1,2,3,4,5}
//  {0,1} are the up down motors
//  3 is forward left
//  4 is forward right
//  5 is backward right
//  6 is backward left
//motorSpeed is number between -100 and 100
//  -100 is full speed backward
//  100 is full speed forward
boolean controlMotor(int motorNum, int motorSpeed){   //
  int forward = 1;                                    //IDK if these constants are right, so constants so that i can change easily :D
  int reverse = 0;
  int dirPins[] = {42,44,46,48,50,52};
  int speedPins[] = {2,3,4,5,6,7};
  int direction = 0;
  if(motorSpeed > 0){                                 //if motor speed, is positive then direction should be forward
    if(motorSpeed <= 100){                            //if motor speed, is also less than 100 then it is in the correct range and should go forward
      int direction = forward;                        //1=forward
    }                                                 //
    else{                                             //if motor speed is more than 100,
      return false;                                   //return false because it is an illogical value
    }                                                 //
  }                                                   //
  if(motorSpeed < 0){                                 //if motor speed is negative, then dir should be backward
    if(motorSpeed >= -100){                           //if motor speed is also greater than -100, then it is in the correct range
      int direction = reverse;                        //0=backward
    }                                                 //
    else{                                             //if motor speed is less than 100,
      return false;                                   //return false because it is an illogical value
    }                                                 //
  }                                                   //
  int speed = map(abs(motorSpeed),0,100,50,255);       //map a value from 0 to 100 into the range of 0 to 255 (because that is 8 bits for PWM)
  if(motorNum >= 0 && motorNum <= 5){                 //if motor num is not in the set of logical ranges for motor num, return false
    return false;
  }
  Serial.print(F("Writing "));
  Serial.print(speed);
  Serial.print(F(" to motor "));
  Serial.println(motorNum);
  analogWrite(speedPins[motorNum],speed);
  digitalWrite(dirPins[motorNum],direction);
}

}
