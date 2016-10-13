#include "Arduino.h"
#include "svo_motor_driver.h"

Servo svo;
ServoState servoState;
UltraSoundState ultraSoundState;

void setupServo() {
  // Attach servo
  svo.attach(13);
  
  // Setup servo
  for (int svo_pos = svo.read(); svo_pos >= 0; svo_pos -= 1) { 
    svo.write(svo_pos);              
    delay(2);                       
  }
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
        delay(2);
      }
    } else {
      for (int svo_pos = currAngle; svo_pos >= angle; svo_pos -= 1) { 
        svo.write(svo_pos);              
        delay(2);
      }
    }
  }
} 

void driveServo() {
  if (ultraSoundState.on) {
    updateDistance();
  }
  
  if (servoState.on) {
    int currAngle = svo.read();
     Serial.println(ultraSoundState.dist);
    if (ultraSoundState.on && (ultraSoundState.dist > 20) && (ultraSoundState.dist < 500)) {
      Serial.println("1");
      if (currAngle > 0) {
        Serial.println(currAngle);
        for (int svo_pos = currAngle; svo_pos >= 0; svo_pos -= 1) { 
          svo.write(svo_pos);              
          delay(2);                       
        }
      }
    } else {
      Serial.println("2");
      Serial.println(svo.read());
      for (int svo_pos = svo.read(); svo_pos >= 0; svo_pos -= 1) { 
        Serial.println(svo_pos);
        svo.write(svo_pos);              
        delay(10);                       
      }
      for (int svo_pos = svo.read(); svo_pos <= 180; svo_pos += 1) { 
        svo.write(svo_pos);              
        delay(10);                       
      }
    }
  }
}
