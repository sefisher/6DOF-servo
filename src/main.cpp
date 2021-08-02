#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h> //For PSA9685 - add this to PlatformIO.ini: 'lib_deps = adafruit/Adafruit PWM Servo Driver Library@^2.4.0'
#include <WiFi.h> // ESP32 WiFi include
#include <WiFiUdp.h>

//=====WIFI SETTINGS
#include "secrets.h" // My WiFi configuration. You can also directly edit SSID and WIFIPSWD below and comment this out
//#define SSID "Your Wifi AP SSID"
//#define WIFIPSWD "Your Wifi AP Password"

#define USE_WIFI_AND_UDP_FOR_FLYPTMOVER true
#define ALLOW_SERIAL_INPUT true
#define STREAM_SERIAL_OUTPUT false

#define NUM_ACTUATORS 6

//Other Hardware settings
#define ONBOARD_LED  2

//=====UDP COMMS SETTINGS
#define UDP_PORT 8266
#define INPUT_SIZE 255
char packetBuffer[INPUT_SIZE];
unsigned int localPort = UDP_PORT;
WiFiUDP Udp; // Creation of wifi Udp instance

//=====SERVO BOARD SETTINGS
// called this way, it uses the default address 0x40
Adafruit_PWMServoDriver board1 = Adafruit_PWMServoDriver(0x40);
//Adafruit_PWMServoDriver board2 = Adafruit_PWMServoDriver(0x41); //this is for a second board if for some reason you have more than 16 servos

// Depending on your servo make, the pulse width min and max may vary, you
// want these to be as small/large as possible without hitting the hard stop
// for max range. You'll have to tweak them as necessary to match your servos.

// You can watch video V1 here to understand the servos: http://youtu.be/y8X9X10Tn1k
// #define SERVOMIN 125 // this is the 'minimum' pulse length count (out of 4096)
// #define SERVOMAX 575 // this is the 'maximum' pulse length count (out of 4096)

// For the Smraza 10 Pcs SG90 9G Micro Servo Motor (https://www.amazon.com/gp/product/B07L2SF3R4)
// the max is 575 and at 60Hz setting the min to 141 puts the servo arm about 180degrees CW 
// the from 575 position - so this works for a 0 to 180 degree range of motion; 
// using these to set a 0 angle (141) and 180 angle (590)

//NOTE: Using these servos in a "linear" actuator doesn't result in linear motion.
#define SERVOMIN 141 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX 575 // this is the 'maximum' pulse length count (out of 4096)

int servoNumber = 0;
String inString = "";
int inputMode = 0; //0 = angle (default); 1 = pulse (direct, bypasses SERVOMAX/MIN settings); 3=off;

//declare functions
void blink_short();
int angleToPulse(int ang);
void ConnectToWiFi();

void setup()
{    
  pinMode(ONBOARD_LED,OUTPUT);
    Serial.begin(9600);

  if(USE_WIFI_AND_UDP_FOR_FLYPTMOVER){
    ConnectToWiFi();
  }
  Serial.println("==============================\n==========Servo Test==========\n==============================");
    if(ALLOW_SERIAL_INPUT){
      Serial.println("Enter 'a' for entering angles; Enter 'p' for entering pulse width directly; or 'o' to turn off serial input control.");
      Serial.println("**WARNING** - Pulse width entries go to servo ignoring SERVOMAX or SERVOMIN and can cause damage ot servo.");
      Serial.println("\n====ENTER SERVO ANGLE TO MOVE SERVOS====\n");
  }
  board1.begin();
  // board2.begin(); //this is for adding mulitple boards if we arecontorlling more than 16 servos.
  board1.setPWMFreq(60); // Analog servos run at ~60 Hz updates
  //board2.setPWMFreq(60);
  //yield();
}

void loop()
{
  if(USE_WIFI_AND_UDP_FOR_FLYPTMOVER){
    int packetSize = Udp.parsePacket();
    if (packetSize) {
      int len = Udp.read(packetBuffer, 255);
      if (len > 0) packetBuffer[len-1] = 0;
      if(STREAM_SERIAL_OUTPUT){
        Serial.print("Recieved (IP/Data): ");
        Serial.print(Udp.remoteIP());//Serial.print(" / ");
        //Serial.print(packetSize);Serial.print(" / ");
        Serial.println(packetBuffer);
      }

      //FLYPTMOVER UDP OUTPUT SHOULD BE SET UP TO SEND THE FOLLOWING:
      //STRING OF 6 AXIS POSITIONS USING DECIMAL FORMAT ON A 12 BIT SCALE (0 TO 4095)
      //EACH AXIS IS SEPARATED BY A ';' (e.g., "2295;1367;3548;2512;1971;745;")
      
      //Parse the UDP input string and send update positions to the number of actuators connected.
      int count=0;
      // Read each command pair 
      char* axis = strtok(packetBuffer, ";");
      while (axis != 0)
      {
        int actpos = atoi(axis);
        float scaledpos = 180-((actpos*1.0f)/4096.0f)*180.;
        if(STREAM_SERIAL_OUTPUT){
          Serial.print(count);Serial.print(": ");Serial.print(actpos);Serial.print(" scaled: ");Serial.println(scaledpos);
        }
        if(count < NUM_ACTUATORS) board1.setPWM(count, 0, angleToPulse(scaledpos));
        // Find the next command in input string
        axis = strtok(0, ";");
        count++;
      }
    }
  }
  if(ALLOW_SERIAL_INPUT){
    while (Serial.available() > 0) {
      inString = Serial.readString();
      if(STREAM_SERIAL_OUTPUT){
        Serial.print("Value:");
        Serial.println(inString.toInt());
        Serial.print("String:");
        Serial.println(inString);
      }
      if(inString == "a"){
          Serial.println("Angle Mode");
          inputMode = 0;
      }else if(inString == "p"){
          Serial.println("Pulse Mode; ***WARNING - NO SERVOMIN/SERVOMAX PROTECTION PROVIDED.");
          inputMode = 1;    
      }else if(inString == "o"){
          Serial.println("Actuator input mode off.");
          inputMode = 1;
      }else{
        if(inputMode == 0) //Angle Input
        {
          int state = inString.toInt();
        
          if ((state < 0)||(state>180))
          {
            Serial.print(">");
            Serial.println(state);
            Serial.println("Angle outside allowed range.");
          }

          if (state >= 0 && state <= 180)
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
  }
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
  if(STREAM_SERIAL_OUTPUT){
    Serial.print("Angle: ");
    Serial.print(ang);
    Serial.print(" pulse: ");
    Serial.println(pulse);
  }
  return pulse;
}

void blink_short(){
  delay(250);
  digitalWrite(ONBOARD_LED,HIGH);
  delay(100);
  digitalWrite(ONBOARD_LED,LOW);
}

void ConnectToWiFi()
{
 
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, WIFIPSWD);
  
  Serial.print("Connecting to "); Serial.println(SSID);
  uint8_t i = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print('.');
    delay(500);
 
    if ((++i % 16) == 0)
    {
      Serial.println(F(" still trying to connect"));
    }
  }
 
  Serial.print(F("Connected. My IP address is: "));
  Serial.println(WiFi.localIP());
  blink_short();blink_short();blink_short();
  Udp.begin(localPort);
}
