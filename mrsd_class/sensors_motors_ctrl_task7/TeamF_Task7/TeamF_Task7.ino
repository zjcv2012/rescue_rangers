#include "svo_motor_driver.h"
#include "stepper_motor_driver.h"
#include "dc_motor_driver.h"

bool sensorMode = true;

void setup() {

  // Global mode
  sensorMode = true;

  // Setup motors
  setupStepper();
  DC_Initial();
  setupServo();

  // Initialize serial
  Serial.begin(9600);
  Serial.println("Ready");
}

// Servo, Stepper, DCMotor, US, IR, F
// State, Reading, Direction (0 F, 1 R), RPM (only for DC), 
void writeStatus() {
  String statusMsg = "bom,";

  // Servo status
  statusMsg = statusMsg + servoState.on + "," + servoState.svo_pos + ",na,na,";

  // Stepper status
  statusMsg = statusMsg + stepperStatus.on_off + "," + stepperStatus.angle + ",na,na,";

  // DC status
  DC_Motor_Status dc_status;
  get_Motor_Status(dc_status);
  statusMsg = statusMsg + dc_status.onoff + "," + dc_status.degree + "," + dc_status.vel + "," + dc_status.dir + ",";

  // UltrasoundSensor status
  statusMsg = statusMsg + ultraSoundState.on + "," + ultraSoundState.dist + ",na,na,";

  // InfraredSensor status
  statusMsg = statusMsg + irSensorStatus.on_off + "," + irSensorStatus.Distance + ",na,na,";

  // ForceSensor status
  statusMsg = statusMsg + "0,0,na,na," ;

  // Print msg
  if (sensorMode == true)
    statusMsg += "1";
  else
    statusMsg += "0"; 
  statusMsg += ",eom";
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
        // bom,m_stp,1,90,na,na,eom
        updateStepperState(commands[2].toInt(), 200, 1);
        driveStepper();
      
      } else if (commands[0].equals("m_ser")) {
        // bom,m_ser,1,90,na,na,eom
        int on = commands[1].toInt();
        updateServoState(on);
        if (on == 1) {
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
    Serial.println("log.." + input);
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


      // DC Motor
      updateState(1, 0, 0, 0);
      driveDCMotor();
      
      // Stepper Motor
      updateStepperState(0, 200, 1);
      updateIRSensorState(1);
      IRSensorAndStepper();

      // Servo Motor
      driveServo();
  } 
  else {
    delay(1000);
  }
  writeStatus();
}

/* 
 *  TEST CASES
 *  
 *  bom,sensor-based,eom  - Serial input based functinality disabled
 *  bom,gui-based,eom  - Sensor based functionality disabled
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

