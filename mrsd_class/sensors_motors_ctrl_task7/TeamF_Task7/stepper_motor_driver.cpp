#include <SharpIR.h>
#include <Stepper.h>

#include "stepper_motor_driver.h"

#define ir A1
#define model 20150

StepperMotorStatus stepperStatus;
IRSensorStatus irSensorStatus;

boolean done=false;
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
Stepper myStepper(stepsPerRevolution, 6, 9, 10,11); //pin 9 step;pin 10 direction
int sensorpin = 0;                 // analog pin used to connect the sharp sensor
float angle_sum=0;
int stepCount = 0;  // number of steps the motor has taken
/////////////////////////////////////////////////////////////////////////////////////////////////////

void setupStepper() 
{
  // put your setup code here, to run once:
  pinMode (ir, INPUT);
  
  stepperStatus.on_off = 1;
  stepperStatus.spd = 200;
  stepperStatus.angle = 0;
  
  irSensorStatus.on_off = 1; 
}

void updateStepperState(int angle, int motorspd, bool on_off) 
{
  stepperStatus.on_off = on_off;
  stepperStatus.angle = angle;
  stepperStatus.spd = motorspd;
}

void updateIRSensorState(bool on_off)
{
  irSensorStatus.on_off = on_off;
}


void driveStepper() 
{
  if (stepperStatus.on_off)
  {
    int angle = stepperStatus.angle;
    int stepNeed = int(angle*4/1.8);
    myStepper.setSpeed(stepperStatus.spd);
    myStepper.step(stepNeed);
    angle_sum = angle_sum+angle;
    stepperStatus.angle = angle_sum;
    stepperStatus.on_off = 1;
  } 
}

void IRSensorAndStepper() 
{
    if (!irSensorStatus.on_off)
      return;

    // it gives you time to open the serial monitor after you upload the sketch
    int num = 0;
    int count = 0;
    int dis_sum = 0;
    
    while (count < 10 )
    { 
        int dis_2 = sharp.distance();  // this returns the distance to the object you're measuring
        count++;
        
        if(abs(dis_1-dis_2)<0.05*dis_1)
        {
            num++;
            dis_sum = dis_sum+dis_2;
        }
          
        dis_1 = dis_2;
        delay(1);
     }

     irSensorStatus.Distance = dis_sum/num;
     if (20 < irSensorStatus.Distance && irSensorStatus.Distance<81)
     {
       stepperStatus.spd =  map(irSensorStatus.Distance, 20, 80, 20, 200);
       stepperStatus.angle = 100;
       driveStepper() ;
     }
     else
     {
       stepperStatus.spd = 0;
       driveStepper();
     }
}
