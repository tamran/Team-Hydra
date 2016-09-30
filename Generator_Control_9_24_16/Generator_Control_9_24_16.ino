 #include <Wire.h>
 #include <Adafruit_PWMServoDriver.h>
 Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

 // GLOBAL VARIABLES SETTING THE PINS FOR THE SERVOS AND THE MOTOR/GENERATOR, AND THEIR MIN/MAX POSITIONS/VALUES 
 #define SPARK_IO   0
	
 #define CHOKE_IO   1
 #define CHOKE_MIN  150
 #define CHOKE_MAX  350
 
 #define THRTL_IO   2
 #define THRTL_MIN  385
 #define THRTL_MAX  540

 /*
 #define GENERATOR_IO   3
 #define GENERATOR_MIN  150
 #define GENERATOR_MAX  255
 */

 // will be using servo pin to control the ESC which in turn controls the motor
 // 
 #define GENERATOR_IO   9
 #define GENERATOR_MIN  245.69
 #define GENERATOR_MAX  491.39
 
 /* used to transform a fractional value from 0 and 1 to a viable pulse length for PWM signal
  * 0 = minimal viable duration, 1 = maximal viable duration 
  * Note: min and max duration different for each servos, and are defined as global variables
  */
 uint16_t convertToPWM(uint8_t servonum, double val) {
   uint16_t mMin;
   uint16_t mMax;
   switch (servonum) {
       case THRTL_IO:
         mMax = THRTL_MIN;
         mMin = THRTL_MAX;
         //Left hand orientation means we must switch Min and Max
         break;
       case CHOKE_IO:
         mMin = CHOKE_MIN;
         mMax = CHOKE_MAX;
         break;
       case GENERATOR_IO:
         mMin = GENERATOR_MIN;
         mMax = GENERATOR_MAX;
         break;
       default:
         return 0;
   }
 
   return (val*(mMax-mMin))+mMin;
 }

 
 /* Sends chosen servo/generator a PWM signal
  * Inputs:
  *     servonum = which servo pin
  *     pos = position/throttle value; must be between 0 and 1 where 0 = minimum position/throttle, 1 = maximum position/throttle
  */
 void setValue(uint8_t servonum, float pos) {
   if(pos < 0) { pos = 0; }
   if(pos > 1) { pos = 1; }
   uint16_t pulselen = convertToPWM(servonum, pos);
   if(pulselen == 0 ) { return; }
   switch (servonum) {
       case THRTL_IO:
        // Serial.println("Setting Throttle: "+(pos*100));

        // MISSING COMMAND TO SEND PWM SIGNAL TO THE THROTTLE SERVO??? 
        //pwm.setPWM(servonum, 0, pulselen);
        //breaks;
        
       case CHOKE_IO:
        // Serial.println("Setting Choke Air Intake: "+(pos*100));
         pwm.setPWM(servonum, 0, pulselen);
         break;
       case GENERATOR_IO:
        // Serial.println("Setting Generator Speed: "+(pos*100));
         pwm.setPWM(servonum, 0, pos);
         break;
       default:
         return ;
   }
   
 }
 
 void setDigitalLow(uint8_t servonum) {
   pwm.setPWM(servonum, 0, 1);
 }
 
 void setDigitalHigh(uint8_t servonum) {
   pwm.setPWM(servonum, 0, 4095);
 }

 
 /*
  * Function for testing the range of a servo. It will go from the "from" position
  * to the "to" position, and then back to the "from" position.
  */
 void fromTo(uint8_t servonum, uint16_t from, uint16_t to) {
   for (uint16_t pulselen = from; pulselen < to; pulselen++) {
     pwm.setPWM(servonum, 0, pulselen);
   }
   delay(500);
   for (uint16_t pulselen = to; pulselen > from; pulselen--) {
     pwm.setPWM(servonum, 0, pulselen);
   }
   delay(500);
 }
 
 int data = 0;
 
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
   Serial.println("System Ready");
 }
 
 void loop() {
     if(data != 0) {
       Serial.println();
       data = 0;
       Serial.println("Waiting for command. 1/2/3 - Start Motor, 4/5 - Idle Motor, 6 - Stop Motor (Choke)");
       Serial.println();
     }
 
     data = Serial.parseInt();
     
     switch (data) {
       case 1:
         Serial.println("Initiating Start Sequence...");
         Serial.println("Enabling Ignition");
         setDigitalHigh(SPARK_IO);
         
         setValue(THRTL_IO, 0.4);
         setValue(CHOKE_IO, 0.0);
         
         setValue(GENERATOR_IO, GENERATOR_MAX); //Must send max to ready motor from stop.
         delay(1500);
         setValue(GENERATOR_IO, 0.4);
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
