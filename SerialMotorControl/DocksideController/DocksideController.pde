import procontroll.*;
import net.java.games.input.*;
import processing.serial.*;

// Notes on the controller:
//   - Axis are inverted, so we used negative multipliers
//   - The "x" axis is the y-axis irl, but the controller's wierd like that

Serial port;

ControllIO controllIO;
ControllDevice joypad;
ControllStick joy1;
ControllStick joy2;
ControllButton aButton; //a
ControllButton bButton; //b
ControllButton xButton; //x
ControllButton yButton; //y

int joy1x = 0;
int joy1y = 0;
int joy2x = 0; 
int joy2y = 0;
boolean aButtonValue = false;
boolean bButtonValue = false;
boolean xButtonValue = false;
boolean yButtonValue = false;
<<<<<<< HEAD
=======
int aButtonInt = 0;
int bButtonInt = 0;
int xButtonInt = 0;
int yButtonInt = 0;
>>>>>>> 69c75d612d48ad005ef7ac424eb0d93548679c89


void setup(){
  size(600, 600);
  noStroke();
  textSize(32);
  
  
  //Set up controller
  controllIO = ControllIO.getInstance(this);
  joypad = controllIO.getDevice("Afterglow Gamepad for Xbox 360");
 
  //Set up joystick w/ proper multiplier for x and y values (negative because otehrwise it's inverted)
  joy1 = joypad.getStick(0);
  joy1.setMultiplier(-255);
 
  
  //Set Up 2nd joystick w/ proper multiplier for x and y values (see above for why)
  joy2 = joypad.getStick(1); 
  joy2.setMultiplier(-255); 
  

  //Set up serial
  println(Serial.list());
  port = new Serial(this, Serial.list()[10], 9600);
}

void draw() {
  background(0.5);
  
  joy1y = (int) joy1.getX();
  joy1x = -(int) joy1.getY(); //y-values need to be inverted, because the controller is weird like that, as I believe I has already noted
  joy2y = (int) joy2.getX();
  joy2x = -(int) joy2.getY(); 
<<<<<<< HEAD
  aButtonValue = (boolean)joypad.getButton(11).pressed();
  bButtonValue = (boolean)joypad.getButton(12).pressed();
  xButtonValue = (boolean)joypad.getButton(13).pressed();
  yButtonValue = (boolean)joypad.getButton(14).pressed();
=======
  aButton = joypad.getButton(11);
  bButton = joypad.getButton(12);
  xButton = joypad.getButton(13);
  yButton = joypad.getButton(14);
>>>>>>> 69c75d612d48ad005ef7ac424eb0d93548679c89
  
  text("x joystick 1: " + str(joy1x), 50, 50);
  text("y joystick 1: " + str(joy1y), 50, 100);
  text("x joystick 2: " + str(joy2x), 50, 150); 
  text("y joystick 2: " + str(joy2y), 50, 200);
  text ("a Button:" + str(aButtonValue), 50, 250);
  text ("b Button:" + str(bButtonValue), 50, 300);
  text ("x Button:" + str(xButtonValue), 50, 350);
  text ("y Button:" + str(yButtonValue), 50, 400);
  // translates boolean values to a 1 or a 0 
  if (aButtonValue == true) {
   aButtonInt = 1; 
  } else {
   aButtonInt = 0;  
  }
  
  if (bButtonValue == true) {
   bButtonInt = 1; 
  } else {
   bButtonInt = 0;  
  }
  
  if (xButtonValue == true) {
   xButtonInt = 1; 
  } else {
   xButtonInt = 0;  
  }
  
  if (yButtonValue == true) {
   yButtonInt = 1; 
  } else {
   yButtonInt = 0;  
  }
  String toSend = "!";
  toSend += str(joy1x) + "/";
  toSend += str(joy1y) + "/";
  toSend += str(joy2x) + "/";
  toSend += str(joy2y) + "/";
  toSend += str (aButtonInt) + "|";
  toSend += str (bButtonInt) + "|";
  toSend += str (xButtonInt) + "|";
  toSend += str (yButtonInt) + "|";
  
  port.write(toSend);
  
  delay(10);
}

//You can also control it with the keyboard (using the up and down arrow keys)
void keyPressed(){
  if (key == CODED) {
    if (keyCode == UP) {
      joy1x ++;
      background(0.5);
    } else if (keyCode == DOWN) {
      joy1x --;
      background(0.5);
    }
    
    joy1x = constrain( joy1x, -255, 255 );
    
  }
}
  
