#include <SharpIR.h>
#include <Stepper.h>

#include "stepper_motor_driver.h"

#define ir A0
#define model 20150

StepperMotorStatus stepperStatus;
IRSensorStatus irSensorStatus;

boolean done=false;
int dis_sum = 0;
int count = 0;
int num = 0;
int dis_1 = 20;

SharpIR sharp(ir, 25, 93, model);

// ir: the pin where your sensor is attached
// 25: the number of readings the library will make before calculating a mean distance
// 93: the difference between two consecutive measurements to be taken as valid
// model: an int that determines your sensor:  1080 for GP2Y0A21Y
//                                            20150 for GP2Y0A02Y
//                                            (working distance range according to the datasheets)
//////////////////////////////////////////////////////////////////////////////////////////////////////

const int stepsPerRevolution = 100;  // change this to fit the number of steps per revolution
// for your motor


// initialize the stepper library on pins 8 through 9:
Stepper myStepper(stepsPerRevolution, 8, 9, 10,11); //pin 9 step;pin 10 direction
int sensorpin = 0;                 // analog pin used to connect the sharp sensor
int motorSpeed = 200;               // variable to store the values from sensor(initially zero)
float angle_2 = 0;
float angle_sum=0;
int stepCount = 0;  // number of steps the motor has taken
/////////////////////////////////////////////////////////////////////////////////////////////////////

void setupStepper() 
{
  // put your setup code here, to run once:
  pinMode (ir, INPUT);
}


void driveStepper(int angle_2, bool on_off) 
{
  stepperStatus.on_off = on_off;
  if (stepperStatus.on_off)
  {
    int stepNeed = int(angle_2*16/1.8);
    myStepper.setSpeed(motorSpeed);
    myStepper.step(stepNeed);
    angle_sum = angle_sum+angle_2;
    stepperStatus.angle = angle_sum;
    stepperStatus.on_off = 1;
  } 
  else 
  {
    // swtich off 
    stepperStatus.on_off = 0;
  }
}

void IRSensorAndStepper(bool on_off_sensor, int Distance) 
{
  irSensorStatus.on_off = on_off_sensor;
  if (irSensorStatus.on_off)
  {
    delay(20);    // it gives you time to open the serial monitor after you upload the sketch
    if(count%20!=0||count==0)
    { 
      int dis_2=sharp.distance();  // this returns the distance to the object you're measuring
      count++;
	    if (abs(dis_1-dis_2)<0.05*dis_1)
      {
        num++;
        dis_sum = dis_sum+dis_2;
      }
     
      dis_1 = dis_2;
    }
    else
    {
	    int meanDistance=dis_sum/num;
	    irSensorStatus.Distance = meanDistance;
	    int motorangle =  map(meanDistance, 20, 80, 20, 360);
      if (20 < meanDistance && meanDistance < 81)
      {
        driveStepper(motorangle, 1) ;
      }
      else
      {
        driveStepper(0, 1) ;
      }
	    count=0;
	    num=0;
	    dis_sum=0;
    }  

    irSensorStatus.on_off = 1;
  } 
  else 
  {
    // swtich off 
    irSensorStatus.on_off= 0;
  } 
}
