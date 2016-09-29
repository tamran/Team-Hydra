#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// SET PIN
#define ESC_PIN 9

// DEFINE PWM FREQ IN Hz
#define FREQ 250

// DEFINE NUMBER OF TICKS PER PULSE
#define NUM_TICKS  4096

// CALCULATE TICK LENGTH IN MILLISECONDS (ms)
const double TICK_LENGTH = pow(10,3)/(FREQ * NUM_TICKS); // gives ms/tick

// DEFINE MIN/MAX PWM LENGTHS IN MILLISECONDS (ms)
const double ESC_MIN_PL = 1;
const double ESC_MAX_PL = 2;


// DEFINE MIN/MAX THROTTLE DECIMAL PERCENTAGES TO INPUT
// in general:
    // 0 = 0%  no movement
    // 1.0 = 100% = full speed forwards
    // -1.0 = -100% = full speed backwards
const double ESC_MIN = -1;
const double ESC_MAX = 1;


// user input from serial monitor
int percentThrottle;
char serialString[100];


// maps input value to output value for given input range and output range
double mapVal(double val, double fromMin, double fromMax, double toMin, double toMax){
  double out = 0;
  if (val < fromMin){ val = fromMin; }
  if (val > fromMax){ val = fromMax; }
  out = (((toMax-toMin)/(fromMax-fromMin))*(val-fromMin)) + toMin;
  return out; 
}

// maps throttle percentage to PWM pulse length in ticks
int convertToTicks(double percentThrottle){
  double pulseLength = mapVal(percentThrottle,ESC_MIN,ESC_MAX,ESC_MIN_PL,ESC_MAX_PL); // gives desired pulse length in ms
  int pulseTicks = round(pulseLength/TICK_LENGTH);
  if (pulseTicks = NUM_TICKS){
    pulseTicks = pulseTicks - 1;
  }
  
  return pulseTicks;
}

// sends pwm pulse based on decimal percentage of throttle 
void sendMotorPulse(double percentThrottle){
  
  // calculate min/max ticks
  int minTick = round(ESC_MIN_PL/TICK_LENGTH);
  int maxTick = round(ESC_MAX_PL/TICK_LENGTH);
  if (maxTick >= NUM_TICKS){          // tick index can't exceed 4095
    maxTick = NUM_TICKS-1;
  }

  // map pulse tick to value in between min/max ticks
  int pulseTick = mapVal(percentThrottle,ESC_MIN,ESC_MAX,minTick,maxTick);
  if (pulseTick >= NUM_TICKS){          // tick index can't exceed 4095
    pulseTick = NUM_TICKS-1;
  }

  pwm.setPWM(ESC_PIN,0,pulseTick);

}


// send 0 PWM signal
void setDigitalLow(uint8_t servoNum) {
  pwm.setPWM(servoNum,0,0);
}

// send MAX PWM signal
void setDigitalHigh(uint8_t servoNum) {
  pwm.setPWM(servoNum,0,NUM_TICKS-1);
}


// send motor min throttle
void setMotorLow(){
  
  // calculate min tick
  int minTick = round(ESC_MIN_PL/TICK_LENGTH);
 
  pwm.setPWM(ESC_PIN,0,minTick);
  
}

// send motor max throttle
void setMotorHigh(){
  
  // calculate max tick
  int maxTick = round(ESC_MAX_PL/TICK_LENGTH);
  if (maxTick >= NUM_TICKS){          // tick index can't exceed 4095
    maxTick = NUM_TICKS-1;
  }
  
  pwm.setPWM(ESC_PIN,0,maxTick);
}


 void setup() {
  // initiate PWM signal
  pwm.begin();
  pwm.setPWMFreq(FREQ);
  
  
  Serial.begin(9600);
  
  yield();
 
  // setup generator
  setDigitalLow(ESC_PIN);
 
  while (!Serial) {
    ;
  }
  
  Serial.println("All systems go!");
   
  
 }




void loop() {
  percentThrottle = Serial.parseInt();
  
  // convert value from 0% to 100% throttle into pulse length duration; negative percent mean backwards rotation
  double pulseLength = map(percentThrottle,ESC_MIN,ESC_MAX,ESC_MIN_PL,ESC_MAX_PL);
  
  int pulseTicks = convertToTicks(percentThrottle);
  
  sprintf(serialString,"percent throttle: %d \t pulse Ticks:0 - %d \t pulse length: %lf", percentThrottle, pulseTicks, pulseLength);
  Serial.println(serialString);
  

  // send pulse
  sendMotorPulse(percentThrottle);
  
    
  delay(1500);

  
}
