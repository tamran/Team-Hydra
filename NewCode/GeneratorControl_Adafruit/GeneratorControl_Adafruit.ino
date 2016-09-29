#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

//SET PINS
#define SPARK_PIN  0
#define CHOKE_PIN  1
#define THRTL_PIN  2
#define ESC_PIN  9 

// DEFINE PWM FREQ IN Hz
const int FREQ = 250;

// DEFINE NUMBER OF TICKS PER PULSE
const int NUM_TICKS = 4096;

// CALCULATE TICK LENGTH IN MILLISECONDS (ms)
const double TICK_LENGTH = pow(10,3)/(FREQ * NUM_TICKS); // gives ms/tick

// DEFINE MIN/MAX PWM LENGTHS IN MILLISECONDS (ms)
const double SPARK_MIN_PL = 0;
const double SPARK_MAX_PL = (1/FREQ);

const double CHOKE_MIN_PL = 0.610;
const double CHOKE_MAX_PL = 1.424;
 
const double THRTL_MIN_PL =  1.566;
const double THRTL_MAX_PL = 2.197; 

const double ESC_MIN_PL = 1;
const double ESC_MAX_PL = 2;


// DEFINE MIN/MAX THROTTLE DECIMAL PERCENTAGES TO INPUT
// in general:
    // 0 = 0%  no movement
    // 1.0 = 100% = full speed forwards
    // -1.0 = -100% = full speed backwards -- SPARK, THROTTLE, AND CHOKE SERVOS DON'T MOVE IN REVERSE DIRECTION!!!
const double SPARK_MIN = 0;
const double SPARK_MAX = 1;

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
  int pulseTick = mapVal(percentThrottle,minThrottle,maxThrottle,minTick,maxTick);
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
  // initiate PWM signal
  pwm.begin();
  pwm.setPWMFreq(FREQ);
  
  
  Serial.begin(9600);
  
  yield();
 
  // setup generator and ignition
  setDigitalLow(ESC_PIN);
  setDigitalLow(SPARK_PIN);
 
  while (!Serial) {
    ;
  }
  
  Serial.println("All systems go!");
   
  
 }
 
 void loop() {
     if(MODE == 0) {
        Serial.println("Waiting for command: 1/2/3 - Start Motor, 4/5 - Idle Motor, 6 - Stop Motor (Choke)");
     }
 
     MODE = Serial.parseInt();

     switch (MODE) {
       case 1:
         Serial.println("Initiating Ignition Sequence");
         
         setServoHigh(SPARK_PIN);
         sendPulse(THRTL_PIN, 0.4);
         setServoLow(CHOKE_PIN);
         
         setServoHigh(ESC_PIN);
         
         delay(1500);
         
         sendPulse(ESC_PIN, 0.4);
         break;
         
       case 2:
         Serial.println("Moving to Start Sequence Phase 2...");
         sendPulse(ESC_PIN, 0.4);
         
         sendPulse(THRTL_PIN, 0.4);
         sendPulse(CHOKE_PIN, 0.5);
         
         break;
         
       case 3:
         Serial.println("Moving to Start Sequence Phase 3...");
         sendPulse(ESC_PIN, 0.4);
         sendPulse(THRTL_PIN, 0.65);
         sendPulse(CHOKE_PIN, 0.5);
         break;
         
       case 4:
         Serial.println("Initiating Idle Sequence...");
         sendPulse(ESC_PIN, 0.2);
         sendPulse(THRTL_PIN, 0.65);
         sendPulse(CHOKE_PIN, 0.5);
         break;
         
       case 5:
         Serial.println("Moving to Idle Sequence Phase 2..");
         sendPulse(ESC_PIN, 0.0);
         sendPulse(THRTL_PIN, 0.65);
         sendPulse(CHOKE_PIN, 0.5);
         break;
         
       case 6:
         Serial.println("Initiating Stop Sequence and Disabling Ignition");
         
         setServoLow(SPARK_PIN);
 
         sendPulse(ESC_PIN, 0.0);
         sendPulse(THRTL_PIN, 0.0);
         sendPulse(CHOKE_PIN, 1.0);
         break;
         
       default:
         break;
     }
 }
