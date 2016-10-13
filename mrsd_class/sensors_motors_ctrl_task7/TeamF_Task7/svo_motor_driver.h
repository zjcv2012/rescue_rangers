#ifndef _SERVO_MOTOR_DRIVER_H_
#define _SERVO_MOTOR_DRIVER_H_

#include <Servo.h>

struct UltraSoundState { 
  int MAX_DIST;
  int MIN_DIST;
  int pin;
  int dist;
  int on;
};

struct ServoState {
  int svo_pos;
  int on;
}; 

void setupServo();
void driveServo(int angle);
void driveServo();
void updateUSSensorState(int on);
void updateServoState(int on);

extern UltraSoundState ultraSoundState;
extern ServoState servoState;

#endif
