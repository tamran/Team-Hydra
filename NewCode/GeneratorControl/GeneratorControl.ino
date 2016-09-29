#include <Wire.h>
#include <Servo.h>
#include <Adafruit_PWMServoDriver.h>
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

//SET PINS
#define SPARK_PIN  0
#define CHOKE_PIN  1
#define THRTL_PIN  2
#define ESC_PIN  9 

// DEFINE PWM FREQ IN Hz
const int FREQ = 250

// DEFINE NUMBER OF TICKS PER PULSE
const int NUM_TICKS 4096

// CALCULATE TICK LENGTH IN MILLISECONDS (ms)
const double TICK_LENGTH = pow(10,3)/(FREQ * NUM_TICKS); // gives ms/tick

// DEFINE MIN/MAX PWM LENGTHS IN MILLISECONDS (ms)
const double SPARK_MIN_PL = 0
const double SPARK_MAX_PL = (1/FREQ)

const double CHOKE_MIN_PL = 0.610
const double CHOKE_MAX_PL = 1.424
 
const double THRTL_MIN_PL =  1.566
const double THRTL_MAX_PL = 2.197

const double ESC_MIN_PL = 1;
const double ESC_MAX_PL = 2;


// CALCULATE MIN/MAX TICKS
const int SPARK_MIN_TICK = mapVal(SPARK_MIN_PL,SPARK_MIN_PL,SPARK_MAX_PL, 0, NUM_TICKS);
const int SPARK_MAX_TICK = mapVal(SPARK_MAX_PL,SPARK_MIN_PL,SPARK_MAX_PL, 0, NUM_TICKS);

const int CHOKE_MIN_TICK = mapVal(CHOKE_MIN_PL,CHOKE_MIN_PL,CHOKE_MAX_PL, 0, NUM_TICKS);
const int CHOKE_MAX_TICK = mapVal(CHOKE_MAX_PL,CHOKE_MIN_PL,CHOKE_MAX_PL, 0, NUM_TICKS);

const int THRTL_MIN_TICK = mapVal(THRTL_MIN_PL,THRTL_MIN_PL,THRTL_MAX_PL, 0, NUM_TICKS);
const int THRTL_MAX_TICK = mapVal(THRTL_MAX_PL,THRTL_MIN_PL,THRTL_MAX_PL, 0, NUM_TICKS);


// DEFINE MIN/MAX THROTTLE DECIMAL PERCENTAGES TO ENTER INTO SERIAL
// in general:
    // 0 = 0%  no movement
    // 1.0 = 100% = full speed forwards
    // -1.0 = -100% = full speed backwards -- SPARK, THROTTLE, AND CHOKE SERVOS DON'T MOVE IN REVERSE DIRECTION!!!
const double SPARK_MIN = 0;
const double SPARK_MIN = 1;

const double CHOKE_MIN = 0;
const double CHOKE_MAX = 1;

const double THRTL_MIN = 0;
const double THRTL_MAX = 1;

const double ESC_MIN = -1;
const double ESC_MAX = 1;


// DEFINE SERIAL INPUT
int MODE = 0;



// maps input value to output value for given input range and output range
double mapVal(double val, double fromMin, double fromMax, double toMin, double toMax){
  double out = 0;
  if (val < fromMin){ val = fromMin; }
  if (val > fromMax){ val = fromMax; }
  out = (((toMax-toMin)/(fromMax-fromMin))*(val-fromMin)) + toMin;
  return out; 
}

// sends pwm pulse based on decimal percentage of throttle 
void sendPulse(int servoNum, double percentThrottle){
  double minPL,maxPL, minThrottle, maxThrottle;
  if (servoNum == SPARK_PIN){
    minPL = SPARK_MIN_PL;
    maxPL = SPARK_MAX_PL;
    minThrottle = SPARK_MIN;
    maxThrottle = SPARK_MAX;
  }
  else if (servoNum == CHOKE_PIN){
    minPL = CHOKE_MIN_PL;
    maxPL = CHOKE_MAX_PL;
    minThrottle = CHOKE_MIN;
    maxThrottle = CHOKE_MAX;
  }
  else if (servoNum == THRTL_PIN){
    minPL = THRTL_MIN_PL;
    maxPL = THRTL_MAX_PL;
    minThrottle = THRTL_MIN;
    maxThrottle = THRTL_MAX;
  }
  else if (servoNum == ESC_PIN){
    minPL = ESC_MIN_PL;
    maxPL = ESC_MAX_PL;    
    minThrottle = ESC_MIN;
    maxThrottle = ESC_MAX;
  }
  else{     // default 
    minPL = 1;
    maxPL = 2;
    minThrottle = 0;
    maxThrottle = 1;
  }

  // calculate min/max ticks
  int minTick = round(minPL/TICK_LENGTH);
  int maxTick = round(maxPL/TICK_LENGTH);
  if (maxTick >= NUM_TICKS){          // tick index can't exceed 4095
    maxTick = NUM_TICKS-1;
  }

  // map pulse tick to value in between min/max ticks
  int pulseTick = getMap(percentThrottle,minThrottle,maxThrottle,minTick,maxTick);
  if (pulseTick >= NUM_TICKS){          // tick index can't exceed 4095
    pulseTick = NUM_TICKS-1;
  }

  pwm.setPWM(servoNum,0,pulseTick);

}


// send 0 PWM signal
void setDigitalLow(uint8_t servoNum) {
  pwm.setPWM(servoNum,0,0);
}

// send MAX PWM signal
void setDigitalHigh(uint8_t servoNum) {
  pwm.setPWM(servoNum,0,NUM_TICKS-1);
}


// send servo to min position
void setServoLow(int servoNum){
  double minPL;
  if (servoNum == SPARK_PIN){
    minPL = SPARK_MIN_PL;
  }
  else if (servoNum == CHOKE_PIN){
    minPL = CHOKE_MIN_PL;
  }
  else if (servoNum == THRTL_PIN){
    minPL = THRTL_MIN_PL;
  }
  else if (servoNum == ESC_PIN){
    minPL = ESC_MIN_PL;
  }
  else{     // default 
    minPL = 1;
  }

  // calculate min tick
  int minTick = round(minPL/TICK_LENGTH);
 
  pwm.setPWM(servoNum,0,minTick);
  
}

// send servo to max position
void setServoHigh(int servoNum){
  double maxPL;
  if (servoNum == SPARK_PIN){
    maxPL = SPARK_MAX_PL;
  }
  else if (servoNum == CHOKE_PIN){
    maxPL = CHOKE_MAX_PL;
  }
  else if (servoNum == THRTL_PIN){
    maxPL = THRTL_MAX_PL;
  }
  else if (servoNum == ESC_PIN){
    maxPL = ESC_MAX_PL;    
  }
  else{     // default 
    maxPL = 2;
  }

  // calculate max tick
  int maxTick = round(maxPL/TICK_LENGTH);
  if (maxTick >= NUM_TICKS){          // tick index can't exceed 4095
    maxTick = NUM_TICKS-1;
  }
  
  pwm.setPWM(servoNum,0,maxTick);
}


 void setup() {
   Serial.begin(9600);
   pwm.begin();
   pwm.setPWMFreq(60);
 
   /*pinMode(GENERATOR_IO, OUTPUT);
   analogWrite(GENERATOR_IO, GENERATOR_MIN);
   */
   // setup generator and ignition
   setDigitalLow(GENERATOR_IO);
   setDigitalLow(SPARK_IO);
 
   while (!Serial) {
     ;
   }
   
   Serial.println("Wait for 4 beeps and tri-tone from Generator ESC.");
   Serial.println("Then send 1 to begin system initialization.");
   
   while (data == 0) {
     data = Serial.parseInt();
   }
   //Serial.println("System Ready");
 }
 
 void loop() {
     if(mode == 0) {
        Serial.println("Waiting for command. 1/2/3 - Start Motor, 4/5 - Idle Motor, 6 - Stop Motor (Choke)");
     }
 
     mode = Serial.parseInt();

     switch (mode) {
       case 1:
         Serial.println("Initiating Ignition Sequence");
         setDigitalHigh(SPARK_IO);
         
         setValue(THRTL_IO, 40);
         setValue(CHOKE_IO, 0);
         
         setValue(GENERATOR_IO, GENERATOR_MAX); //Must send max to ready motor from stop.
         
         delay(1500);
         
         setValue(GENERATOR_IO, 40);
         break;
         
       case 2:
         Serial.println("Moving to Start Sequence Phase 2...");
         setValue(GENERATOR_IO, 0.4);
         setValue(THRTL_IO, 0.4);
         setValue(CHOKE_IO, 0.5);
         break;
         
       case 3:
         Serial.println("Moving to Start Sequence Phase 3...");
         setValue(GENERATOR_IO, 0.4);
         setValue(THRTL_IO, 0.65);
         setValue(CHOKE_IO, 0.5);
         break;
         
       case 4:
         Serial.println("Initiating Idle Sequence...");
         setValue(GENERATOR_IO, 0.2);
         setValue(THRTL_IO, 0.65);
         setValue(CHOKE_IO, 0.5);
         break;
         
       case 5:
         Serial.println("Moving to Idle Sequence Phase 2..");
         setValue(GENERATOR_IO, 0.0);
         setValue(THRTL_IO, 0.65);
         setValue(CHOKE_IO, 0.5);
         break;
         
       case 6:
         Serial.println("Initiating Stop Sequence...");
         
         Serial.println("Disabling Ignition");
         setDigitalLow(SPARK_IO);
 
         setValue(GENERATOR_IO, 0.0);
         setValue(THRTL_IO, 0.0);
         setValue(CHOKE_IO, 1.0);
         break;
       default:
         break;
     }
 }
