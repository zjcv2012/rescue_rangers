#include "Arduino.h"
#include "svo_motor_driver.h"

Servo svo;
ServoState servoState;
UltraSoundState ultraSoundState;

void setupServo() {
  // Attach servo
  svo.attach(13);
  
  servoState.svo_pos = 0;  
  servoState.on = 1;  
  
  // Setup Ultrasound
  ultraSoundState.MAX_DIST = 256;  
  ultraSoundState.MIN_DIST = 6;
  ultraSoundState.pin = A3;
  ultraSoundState.dist = 0;
  ultraSoundState.on = 1;
  
}

void updateDistance() {
  int sum = 0;
  for (int i = 0; i < 5 ; i++)
  {
    int volt = analogRead(ultraSoundState.pin);
    sum += volt;
    delay(5);
  }
  ultraSoundState.dist = sum/5;
}

void updateServoState(int on) {
  servoState.on = on;
}

void updateUSSensorState(int on) {
  ultraSoundState.on = on;
}

void driveServo(int angle){
  if (servoState.on) {
    int currAngle = svo.read();
    if (currAngle > angle) {
      for (int svo_pos = currAngle; svo_pos <= angle; svo_pos += 1) { 
        svo.write(svo_pos);              
        delay(20);
      }
    } else {
      for (int svo_pos = currAngle; svo_pos >= angle; svo_pos -= 1) { 
        svo.write(svo_pos);              
        delay(20);
      }
    }
  }
} 

void driveServo() {

  updateDistance();
  if (ultraSoundState.dist > 30) {
  
    for (int svo_pos = 180; svo_pos >= 0; svo_pos -= 1) { 
      svo.write(svo_pos);              
      delay(20);                       
    }
    for (int svo_pos = 0; svo_pos <= 180; svo_pos += 1) { 
      svo.write(svo_pos);              
      delay(20);                       
    }
  }
}
