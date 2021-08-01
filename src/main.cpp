#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_PWMServoDriver.h"

// called this way, it uses the default address 0x40
Adafruit_PWMServoDriver board1 = Adafruit_PWMServoDriver(0x40);
//Adafruit_PWMServoDriver board2 = Adafruit_PWMServoDriver(0x41);

// Depending on your servo make, the pulse width min and max may vary, you
// want these to be as small/large as possible without hitting the hard stop
// for max range. You'll have to tweak them as necessary to match the servos you
// have!
// Watch video V1 to understand the two lines below: http://youtu.be/y8X9X10Tn1k
#define SERVOMIN 125 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX 575 // this is the 'maximum' pulse length count (out of 4096)

int servoNumber = 0;
String inString = "";
int inputMode = 0; //0 = angle (default); 1 = pulse (direct, bypasses SERVOMAX/MIN settings);
int angleToPulse(int ang);

void setup()
{
  Serial.begin(9600);
  Serial.println("=====2 channel Servo Test=====");
  Serial.println("Enter 'a' for entering angles; Enter 'p' for entering pulse width directly.");
  Serial.println("**WARNING** - Pulse width entries go to servo ignoring SERVOMAX or SERVOMIN and can cause damage ot servo.");
  Serial.println("\n====ENTER SERVO ANGLE TO MOVE SERVOS====\n");
  board1.begin();
  // board2.begin();
  board1.setPWMFreq(60); // Analog servos run at ~60 Hz updates
  //board2.setPWMFreq(60);
  //yield();
}

void loop()
{
  //Serial.println(".");
  //for (int angle = 0; angle < 270; angle += 10)
  // {
  //   //for (int i = 0; i < 16; i++) //changed from example 16 channels to 1
  //   for (int i = 0; i < 2; i++) //changed from example 16 channels to 1
  //   {
  //     //board2.setPWM(i, 0, angleToPulse(angle));
  //     board1.setPWM(i, 0, angleToPulse(angle));
  //   }
  // }
  while (Serial.available() > 0) {
    inString = Serial.readString();
    Serial.print("Value:");
    Serial.println(inString.toInt());
    Serial.print("String:");
    Serial.println(inString);
    if(inString == "a"){
        Serial.println("Angle Mode");
        inputMode = 0;
    }else if(inString == "p"){
        Serial.println("Pulse Mode; ***WARNING - NO SERVOMIN/SERVOMAX PROTECTION PROVIDED.");
        inputMode = 1;
    }else{
      if(inputMode == 0) //Angle Input
      {
        int state = inString.toInt();
      
        if ((state < 10)||(state>180))
        {
          Serial.print(">");
          Serial.println(state);
          Serial.println("Angle outside allowed range.");
        }

        if (state >= 10 && state <= 180)
        {
          Serial.print(">");
          Serial.println(state);
          Serial.print("turning servo to ");
          Serial.print(state);
          Serial.println(" degrees");
          board1.setPWM(0, 0, angleToPulse(state));
          board1.setPWM(1, 0, angleToPulse(state));
        }
      }
      else if(inputMode==1) //Pulse Input
      {
        int state = inString.toInt();

        Serial.print(">");
        Serial.println(state);
        Serial.print("sending servo ");
        Serial.print(state);
        Serial.println(" as pulse width.");
        board1.setPWM(0, 0, state);
        board1.setPWM(1, 0, state);
      }
    }
    // clear the string for new input:
    inString = "";
  }



// if (Serial.available())
// {
//   int state = Serial.parseInt();
    
//   if (state < 10)
//   {
//     Serial.print(">");
//     Serial.println(state);
//     Serial.println("cannot execute command");
//   }

//   if (state >= 10 && state < 180)
//   {
//     Serial.print(">");
//     Serial.println(state);
//     Serial.print("turning servo to ");
//     Serial.print(state);
//     Serial.println(" degrees");
//     //myservo.write(state);
//     board1.setPWM(0, 0, angleToPulse(state));
//     board1.setPWM(1, 0, angleToPulse(state));
//   }
// }
  // robojax PCA9865 16 channel Servo control
  delay(100);
}

/*
 * angleToPulse(int ang)
 * gets angle in degree and returns the pulse width
 * also prints the value on seial monitor
 * written by Ahmad Nejrabi for Robojax, Robojax.com
 */
int angleToPulse(int ang)
{
  int pulse = map(ang, 0, 180, SERVOMIN, SERVOMAX); // map angle of 0 to 180 to Servo min and Servo max
  Serial.print("Angle: ");
  Serial.print(ang);
  Serial.print(" pulse: ");
  Serial.println(pulse);
  return pulse;
}

