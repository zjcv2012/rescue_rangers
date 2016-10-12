#include "svo_motor_driver.h"
#include "stepper_motor_driver.h"
#include "dc_motor_driver.h"

bool sensorMode = true;

void setup() {

  // Global mode
  sensorMode = true;

  // Setup motors
  setupServo(A1);
  setupStepper();
  DC_Initial();

  // Initialize serial
  Serial.begin(9600);
  Serial.println("Ready");
}

void writeStatus() {
  String statusMsg = "bom,";

  // Servo status
  statusMsg = statusMsg + "m_ser," + servoState.svo_on + "," + servoState.svo_pos;

  // Stepper status
  statusMsg = statusMsg + ",m_stp," + stepperStatus.angle;

  // DC status
  //statusMsg = statusMsg + ",m_dcm," + stepperStatus.angle;

  // UltrasoundSensor status
  statusMsg = statusMsg + ",s_usr," + ultraSoundState.us_on + "," + ultraSoundState.dist;

  // InfraredSensor status
  statusMsg = statusMsg + ",s_isr," + irSensorStatus.on_off + "," + irSensorStatus.Distance;

  // ForceSensor status
  statusMsg = statusMsg + ",s_isr," + irSensorStatus.on_off + "," + irSensorStatus.Distance;

  // Print msg
  statusMsg += ",eom"
  Serial.println(statusMsg);
}

void processCommand(String commands[20], int numCommands) {
  if (numCommands == 0)
    return;
    
  if (commands[0].equals("sensor-based")) {
    sensorMode = true;
    return;
  } else if (commands[0].equals("gui-based")) {
    sensorMode = false;
    return;
  } else {
    if (!sensorMode) {
      if (commands[0].equals("m_dcm")) {
      } else if (commands[0].equals("m_stp")) {
        driveStepper(commands[2].toInt(), commands[1].toInt());
      } else if (commands[0].equals("m_ser")) {
        int on = commands[1].toInt();
        updateServoState(on == 1);
        if (on) {
          int angle = commands[2].toInt();
          driveServo(angle);
        }
      } else if (commands[0].equals("s_for")) {
      } else if (commands[0].equals("s_isr")) {
        int on = commands[1].toInt();
        irSensorStatus.on_off = on;
      } else if (commands[0].equals("s_usr")) {
        int on = commands[1].toInt();
        updateUSSensorState(on == 1);
      }
    }
  }
}

void displayCommands(String commands[20], int numCommands) {
  for (int i=0 ; i<numCommands ; i++) {
    Serial.println(commands[i]);
  }
}

int readCommand(String commands[20]) {
  int count = 0;
  if (Serial.available() > 0) {
    String input = Serial.readString();
    if (!input.startsWith("bom") || !input.endsWith("eom\n")) 
      return 0;
    input.trim();

    String currString = "";
    int currPos = input.indexOf(',', 0) + 1;
    while (!currString.equals("eom")) {
      int pos = input.indexOf(',', currPos);
      if (pos != -1) {
        currString = input.substring(currPos, pos); 
        commands[count++] = currString;
        currPos = pos+1;
      } else
        break;
    }
  }
  return count;
}

void loop() {
  
  // Check UI override
  String commands[20];
  int numCommands = readCommand(commands);
  if (numCommands > 0)
      processCommand(commands, numCommands);

  // Check individual motors
  if (sensorMode) {
    driveServo();
  }
  //writeStatus();
  delay(1000);  
}

/* 
 *  TEST CASES
 *  
 *  1. Send gui-based. 
 *  
 *  
 * 
 * bom,sensor-based,eom
   bom,m_ser,1,0,na,na,eom
   bom,m_stp,1,0,na,na,eom
bom,m_dcm,1,0,na,0,eom
bom,s_usr,1,0,na,na,eom
bom,s_isr,1,0,na,na,eom
bom,s_for,1,0,na,na,eom
bom,m_ser,0,0,na,na,eom
bom,m_ser,1,0,na,na,eom
bom,m_stp,0,0,na,na,eom
bom,m_stp,1,0,na,na,eom
bom,m_dcm,0,0,na,na,eom
bom,m_dcm,1,0,na,na,eom
bom,s_usr,0,0,na,na,eom
bom,s_usr,1,0,na,na,eom
bom,s_irs,0,0,na,na,eom
bom,s_irs,1,0,na,na,eom
bom,s_for,0,0,na,na,eom
bom,s_for,1,0,na,na,eom
bom,s_for,0,0,na,na,eom
bom,gui-based,eom
bom,m_ser,1,0,na,na,eom
bom,m_stp,1,0,na,na,eom
bom,m_dcm,1,0,na,0,eom
bom,s_usr,1,0,na,na,eom
bom,s_isr,1,0,na,na,eom
bom,s_for,1,0,na,na,eom
bom,m_ser,1,20,0,na,eom
bom,m_ser,1,20,1,na,eom
bom,m_stp,1,30,0,na,eom
bom,m_stp,1,30,1,na,eom
bom,m_dcm,1,na,0,400,eom
bom,m_dcm,1,na,1,400,eom
bom,m_dcm,1,39,1,na,eom
bom,m_dcm,1,39,0,na,eom
 * /
 */

