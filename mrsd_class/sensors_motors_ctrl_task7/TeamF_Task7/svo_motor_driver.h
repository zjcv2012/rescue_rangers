#ifndef _SERVO_MOTOR_DRIVER_H_
#define _SERVO_MOTOR_DRIVER_H_

#include <Servo.h>

struct UltraSoundState { 
  int MAX_DIST;
  int MIN_DIST;
  int pin;
  int dist;
  bool us_on;
};

struct ServoState {
  int svo_pos;
  bool svo_on;
}; 

void setupServo(int usPin);
void driveServo(int angle);
void driveServo();
void updateUSSensorState(bool on);
void updateServoState(bool on);

extern UltraSoundState ultraSoundState;
extern ServoState servoState;

#endif
