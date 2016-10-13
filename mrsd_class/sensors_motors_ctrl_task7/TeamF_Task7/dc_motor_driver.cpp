#include <Arduino.h>
#include "PID_v1.h"
#include "dc_motor_driver.h"

// Pin Definition
#define encoder0PinA  2
#define buttonPin 3
#define encoder0PinB  4
#define E1_2Pin 5
#define l1Pin 8
#define l2Pin 7

void DC_Initial();
void DC_Move_Degree(int degree, int d);
void DC_Move_PID(int velocity, int d);



// Initialization for Interrupt
long debounce_time=150;    // the debounce time; increase if the output flickers
volatile unsigned long last_micros;

//Initialization for motr
int velocity;
int d;
int degree;
int encoder0Pos;
int currentLoc=0;
int lastLoc=0;
double vel; //velocity for encoder
bool onoff; //shows that motor is moving or not, true means motor is moving.

int state=3; //state of the motor, 0 is for motionless, 1 is for sensor input, 2 for is pid control,3 is for position control
int dir=0; //direction of the motor, LOW means forward

//Define global variables for state1
int sensorPin = A0;    // select the input pin for the potentiometer
int sensorValue = 0;  // variable to store the value coming from the sensor
int sensorSpeed=0;   // Speed sent to motors according to the sensor

//Define global variables for state2, i.e. DC_Move_PID
double Setpoint, Input, Output;
double Kp=4, Ki=3, Kd=0;
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

//Define global variables for state3, i.e. DC_Move_Degree
int destLoc;
int error; //for position amendment
bool DC_Move_Start=false;  // to make sure DC moves only if it is required

//this function is called every 1 second
void callback()
{
  if((long)(micros()-last_micros) >= 1000000)
  {
    double time = (double(micros() - last_micros))/1000000;
    last_micros=micros();
    currentLoc=encoder0Pos;
    vel=((double)(currentLoc-lastLoc))/(3*time);
    lastLoc=currentLoc;
    vel=abs(vel);
  }
  if(state==2)
  {
      myPID.SetMode(AUTOMATIC);
  } 
  else
  {
      myPID.SetMode(MANUAL);
  }    
}


void driveDCMotor() {
  callback();
  
  switch(state)
  {
    case 0:
      analogWrite(E1_2Pin, 0); 
    break;
    case 1:
      sensorValue = analogRead(sensorPin);
      sensorSpeed = map(sensorValue, 0, 1023, 0, 255);
      analogWrite(E1_2Pin, sensorSpeed);
    break;
    case 2:
      DC_Move_PID(velocity, d);
    break;
    case 3:
       if(DC_Move_Start==false)
       {
         Serial.println("GoGoGo!");
         DC_Move_Degree(degree, d);
       }
    break;
    default:
    break;   
  }
  delay(5);
}

void Interrupt()
{
 
  if(state==1)
  {
    dir=!dir;  
    digitalWrite(l1Pin,dir);
    digitalWrite(l2Pin,!dir);
  }
}

//this is for button interruption
void debounceInterrupt()
{
  if((long)(micros()-last_micros)>=debounce_time*1000)
  {
    Interrupt();
    last_micros=micros();
  }
}


//this is for encoder interruption
void debounce_Encoder() {
   //delay(10);
    //Serial.println("encoder!");
    if (digitalRead(encoder0PinB) == LOW) 
    {  // check channel B to see which way
      encoder0Pos = encoder0Pos - 1;         // CCW
    } 
    else 
    {
      encoder0Pos = encoder0Pos + 1;         // CW
    }
    //currentLoc=encoder0Pos;
    if(state==3)
    {
     //Serial.println(encoder0Pos);
     Serial.println(destLoc-encoder0Pos);
      if(DC_Move_Start==true)
     {
     
        if(dir==0)
        { 
          if((encoder0Pos+error)>=destLoc)
          {
             DC_Move_Start=false;
            analogWrite(E1_2Pin, 0);
            //Serial.println("State"); 
            state=0;   //let it stop
          }
        }
        else
        {
          if((encoder0Pos-error)<=destLoc)
          {
             DC_Move_Start=false;
            analogWrite(E1_2Pin, 0);
            Serial.println("stop"); 
            state=0;  //let it stop
          }
        }
      }
    }
 // Serial.println (encoder0Pos, DEC);          // debug - remember to comment out
}


void DC_Initial()
{
  //Initialize Input
  pinMode(encoder0PinA, INPUT); 
  digitalWrite(encoder0PinA, HIGH);       // turn on pullup resistor
  pinMode(encoder0PinB, INPUT); 
  digitalWrite(encoder0PinB, HIGH);       // turn on pullup resistor
  pinMode(buttonPin,INPUT);
 

//Initialize Output
  pinMode(l1Pin, OUTPUT);
  pinMode(l2Pin, OUTPUT);
  pinMode(E1_2Pin, OUTPUT);

//Initialize the Interrupt functions
  attachInterrupt(digitalPinToInterrupt(3),debounceInterrupt,FALLING);
  attachInterrupt(digitalPinToInterrupt(encoder0PinA ), debounce_Encoder, FALLING);  // encoder pin on interrupt 0 - pin 2

  
  encoder0Pos=0;
  digitalWrite(l1Pin,dir);
  digitalWrite(l2Pin,!dir);
  analogWrite(E1_2Pin, 0);   
   
  Serial.begin (9600);
  Serial.println("start");                // a personal quirk

  

   //initialize the pid value
  Input = vel;
  Setpoint = 0;
  myPID.SetMode(MANUAL);
  }

// for state2 pid control, i.e. velocity control
 void DC_Move_PID(int velocity, int d)
  {
      dir=!d;
      Input = vel;
      Setpoint = velocity;
      myPID.Compute();
      digitalWrite(l1Pin,dir);
      digitalWrite(l2Pin,!dir);

      analogWrite(E1_2Pin, Output);
  }



// for state3 motor control, i.e. positon control
  void DC_Move_Degree(int degree, int d)
  {
    DC_Move_Start=true;
    int increStep=floor(degree/2);
    if(degree>=150)
    {
      error=30;
    }
    else if(60<=degree<150)
    {
      error=15+(degree-60)/6;
    }
    else
    {
       error=(int)degree/4;
     }
    if(d==1)
    {
      destLoc=encoder0Pos+increStep;
      dir=0;
      Serial.println("forward");
    }
    else
    {
      destLoc=encoder0Pos-increStep;
      dir=1;
      Serial.println("reverse");
    }
     digitalWrite(l1Pin,dir);
     digitalWrite(l2Pin,!dir);
     analogWrite(E1_2Pin, 140);    
  }

 
void get_Motor_Status(DC_Motor_Status& dc_Motor)
{
  callback();
  dc_Motor.onoff= (vel==0?0:1);
  dc_Motor.degree=(currentLoc%180)*2;
  dc_Motor.vel=vel;
  dc_Motor.dir=!dir;
  dc_Motor.state = state;
}

void updateState(int s, int v, int dr, int deg) {
  state = s;
  velocity = v;
  d = dr;
  degree = deg;
}

