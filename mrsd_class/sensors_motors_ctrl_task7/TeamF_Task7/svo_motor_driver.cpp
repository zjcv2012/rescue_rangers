#include "Arduino.h"
#include "svo_motor_driver.h"

Servo svo;
ServoState servoState;
UltraSoundState ultraSoundState;

void setupServo(int usPin) {
  // Attach servo
  svo.attach(7);
  
  // Setup servo
  for (int svo_pos = svo.read(); svo_pos >= 0; svo_pos -= 1) { 
    svo.write(svo_pos);              
    delay(15);                       
  }
  servoState.svo_pos = 0;  
  
  // Setup Ultrasound
  ultraSoundState.MAX_DIST = 256;  
  ultraSoundState.MIN_DIST = 6;
  ultraSoundState.pin = usPin;
  ultraSoundState.dist = 500;
}

void updateDistance() {
  int volt = analogRead(ultraSoundState.pin);
  ultraSoundState.dist = volt/2;
}

void updateServoState(bool on) {
  servoState.svo_on = on;
}

void updateUSSensorState(bool on) {
  ultraSoundState.us_on = on;
}

void driveServo(int angle){
  if (servoState.svo_on) {
    int currAngle = svo.read();
    if (currAngle > angle) {
      for (int svo_pos = currAngle; svo_pos <= angle; svo_pos += 1) { 
        svo.write(svo_pos);              
        delay(15);
      }
    } else {
      for (int svo_pos = currAngle; svo_pos >= angle; svo_pos -= 1) { 
        svo.write(svo_pos);              
        delay(15);
      }
    }
  }
} 

void driveServo() {
  if ((servoState.svo_on) && (ultraSoundState.us_on)){
    updateDistance();
    if (ultraSoundState.dist < 20) {
      int currAngle = svo.read();
      int svo_pos = servoState.svo_pos;
      if (currAngle == 180) {
        for (svo_pos = 180; svo_pos >= 0; svo_pos -= 1) { 
          svo.write(svo_pos);              
          delay(15);                       
        }    
      } else if (currAngle == 0) {
        for (svo_pos = 0; svo_pos <= 180; svo_pos += 1) { 
          svo.write(svo_pos);              
          delay(15);                       
        }    
      }
    } else {
    }
    ultraSoundState.dist += 1;
  }
}
