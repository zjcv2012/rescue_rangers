#include "Arduino.h"
#include "svo_motor_driver.h"

Servo svo;
ServoState servoState;
UltraSoundState ultraSoundState;

// Setup
void setupServoUltrasound() {
  // Setup servo
  svo.attach(13);  
  servoState.svo_pos = 0;  
  servoState.on = 1;  
  
  // Setup ultrasound
  ultraSoundState.MAX_DIST = 256;  
  ultraSoundState.MIN_DIST = 6;
  ultraSoundState.pin = A3;
  ultraSoundState.dist = 0;
  ultraSoundState.on = 1;
  
}

// Servo state update
void updateServoState(int on) {
  servoState.on = on;
}

// Ultrasound state update
void updateUSSensorState(int on) {
  ultraSoundState.on = on;
}

// Get distance from Ultrasound
void updateDistance() {
  int sum = 0;
  int numSamples = 5;
  for (int i = 0; i < numSamples ; i++) {
    int volt = analogRead(ultraSoundState.pin);
    sum += volt;
    delay(5);
  }
  ultraSoundState.dist = (sum/numSamples) * 0.5;
}

// Drive servo to specified angle
void driveServo(int angle){
  if (servoState.on) {
    int currAngle = svo.read();
    if (currAngle > angle) {
      for (int svo_pos = currAngle; svo_pos >= angle; svo_pos -= 1) { 
        svo.write(svo_pos);              
      }
    } else {
      for (int svo_pos = currAngle; svo_pos <= angle; svo_pos += 1) { 
        svo.write(svo_pos);              
      }
    }
    delay(20);
    servoState.svo_pos = angle;
  }
} 

// Compute servo angle. 
// Servo is set to 0 for less than 12 inches
// Servo is set to a position determined as a linear function of distance between 12 and 48 inches
// Servo is set to 180 beyond 48 inches.
int computeAngle(int dist) {
  if (dist < 12) {
    return 0;
  } else if (dist > 48) {
    return 180;
  } else {
    return 5*(dist-12);
  }
}

// Drive servo based on distance.
void driveServo() {
  updateDistance();
  if ((servoState.on == 1) && (ultraSoundState.on == 1)) {
    driveServo(computeAngle(ultraSoundState.dist));
  }
}
