#include <Wire.h>
#include <Servo.h>

// SET PIN
#define ESC_PIN 9
Servo escServo;

// DEFINE MIN/MAX PULSE LENGTH IN MILLISECONDS (ms)
const double ESC_MIN_PL = 1;
const double ESC_MAX_PL = 2;

// DEFINE MIN/MAX THROTTLE DECIMAL PERCENTAGES TO INPUT
// in general:
    // 0 = 0%  no movement
    // 1.0 = 100% = full speed forwards
    // -1.0 = -100% = full speed backwards
const double ESC_MIN = -1;
const double ESC_MAX = 1;


// DEFINE SERIAL INPUT
int percentThrottle;
char serialString[100];


void setup() {
  // setup esc servo
  escServo.attach(ESC_PIN);
  
  Serial.begin(9600);
  
  yield();
}


// maps input value to output value for given input range and output range
double mapVal(double val, double fromMin, double fromMax, double toMin, double toMax){
  double out = 0;
  if (val < fromMin){ val = fromMin; }
  if (val > fromMax){ val = fromMax; }
  out = (((toMax-toMin)/(fromMax-fromMin))*(val-fromMin)) + toMin;
  return out; 
}


// maps throttle percentage to PWM pulse length in μs (converts ms ->μs!!!)
double convertToPL(double percentThrottle){
  double pulseLength = mapVal(percentThrottle,ESC_MIN,ESC_MAX,ESC_MIN_PL*pow(10,3),ESC_MAX_PL*pow(10,3));
  return pulseLength;
}

void sendMotorPulse(double percentThrottle){
  double pulseLength = convertToPL(percentThrottle);
  escServo.writeMicroseconds(pulseLength);
}


void loop() {
  percentThrottle = Serial.parseInt();

  double pulseLength = convertToPL(percentThrottle);
  
  sprintf(serialString,"percentThrottle: %d \t pulse length: %lf", percentThrottle, pulseLength);
  Serial.println(serialString);
  

  // convert throttle decimal percentage into pulse length duration and send pulse
  sendMotorPulse(percentThrottle);
  
  delay(1500);

  
}
