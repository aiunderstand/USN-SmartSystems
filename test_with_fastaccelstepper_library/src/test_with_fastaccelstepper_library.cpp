#include <Arduino.h>
#include "FastAccelStepper.h"

    //TB6560 motor driver switches (these are not at the extremes, but good for getting started)
    //OFF ON OFF OFF (1 AMP Running Current)
    //ON (20% stop current)
    //OFF OFF (no microsteps)
    //OFF ON (50% decay) 

    //TB6560 motor driver wiring
    //A+ (black wire motor)
    //A- (green wire motor)
    //B+ (red wire motor)
    //B- (blue wire motor)
    //PWR (12V of power supply, note that it can handle 24V but is not needed for simple usage)
    //GND (ground of power supply)
    
    //CLK+ (wire to pin 6 of Arduino MEGA)
    //CLK- (wire to GND of Arduino MEGA)
    //CW+ (wire to pin 5 of Arduino MEGA)
    //CW- (wire to GND of Arduino MEGA or connect to CLK-)
    //EN+ (wire to external red button pin)
    //EN- (wire to GND of Arduino MEGA or connect to CLK-)
    
#define enablePin 4 //use this pin as an emergency stop pin (stop = high), wire to external red button.
#define dirPin 5 //use this pin to switch direction clockwise or counterclockwise
#define stepPin 6 //only pins 5,6,11,46 allow 50 KHz PWM  

FastAccelStepperEngine engine = FastAccelStepperEngine();
FastAccelStepper *stepper = NULL;

enum States {
  INIT = 0,
  CW=1,
  RUNNING_CW = 2,
  CCW=3,
  RUNNING_CCW=4,
  STOP=5
};

States state = States::INIT;

void setup() {
  Serial.begin(115200);

  engine.init(); 
  stepper = engine.stepperConnectToPin(stepPin);
  stepper->setDirectionPin(dirPin);

  if (stepper) 
    Serial.println("Motor found");   
  else
    Serial.println("ERROR: No motor found");   
 }

void loop() {
  switch (state) {
      case States::INIT:
       Serial.println("INIT");  
        stepper->setSpeedInUs(20);
        stepper->setAcceleration(100);
        state = States::CW;
        break;
      case States::CW:
       Serial.println("CW");  
        stepper->move(100); // 100 steps CW as NON blocking call
        state = States::RUNNING_CW;
        break;
      case States::RUNNING_CW:
        if (!stepper->isRunning()) {
          state = States::CCW;
        }
        break;
      case States::CCW:
        Serial.println("CCW");  
        stepper->move(-100); // 100 steps CCW as NON blocking call
        state = States::RUNNING_CCW;
        break;
      case States::RUNNING_CCW:
        if (!stepper->isRunning()) {
          state = States::CW;
        }
        break;
      default: 
        state = States:: INIT;
  }
}
