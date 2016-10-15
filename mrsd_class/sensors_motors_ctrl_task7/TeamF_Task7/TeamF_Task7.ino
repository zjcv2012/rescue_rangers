#include "svo_motor_driver.h"
#include "stepper_motor_driver.h"
#include "dc_motor_driver.h"

bool sensorMode;

void setup() {

  // Global mode
  sensorMode = false;

  // Setup motors
  DC_Initial();
  setupStepper();
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
  statusMsg = statusMsg + dc_status.onoff + "," + dc_status.degree + "," + dc_status.dir + "," + dc_status.vel + ",";

  // UltrasoundSensor status
  statusMsg = statusMsg + ultraSoundState.on + "," + ultraSoundState.dist + ",na,na,";

  // InfraredSensor status
  statusMsg = statusMsg + irSensorStatus.on_off + "," + irSensorStatus.Distance + ",na,na,";

  // Potentio,eter status
  statusMsg = statusMsg + "1," + dc_status.sensorValue + ",na,na," ;

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
    updateState(0, 0, 0, 0);
    return;
  } else {
    if (!sensorMode) {
      if (commands[0].equals("m_dcm")) {
        // bom,m_dcm,1,90,na,na,eom
        String on = commands[1];
        String deg = commands[2];
        String dir = commands[3];
        String vel = commands[4];
        if (on.toInt() == 1) {
          if (vel.equals("na"))
            updateState(3, 0, dir.toInt(), deg.toInt());
          else 
            updateState(2, vel.toInt(), dir.toInt(), 0);
        } else {
            updateState(0, 0, 0, 0);
        }
        driveDCMotor();  
      } else if (commands[0].equals("m_stp")) {
        // bom,m_stp,1,90,1,na,eom
        // bom,m_stp,1,90,0,na,eom
        int angle = commands[2].toInt();
        int dir = commands[3].toInt() == 1 ? 1: -1;
        updateStepperState(angle*dir, 200, 1);
        driveStepper();
      
      } else if (commands[0].equals("m_ser")) {
        // bom,m_ser,1,90,na,na,eom
        int on = commands[1].toInt();
        updateServoState(on);
        if (on == 1) {
          int angle = commands[2].toInt();
          driveServo(angle);
        }
      } else if (commands[0].equals("s_isr")) {
        int on = commands[1].toInt();
        irSensorStatus.on_off = on;
      } else if (commands[0].equals("s_usr")) {
        int on = commands[1].toInt();
        updateUSSensorState(on);
      }
    }
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
    //Serial.println("log.." + input);

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

      // Enable Ultrasonic and Servo Motor 
      updateUSSensorState(1);
      driveServo();
  } else {
    DC_Motor_Status dc_status;
    get_Motor_Status(dc_status);
    //Serial.println(dc_status.state);
    driveDCMotor();
  }
  writeStatus();
  delay(50);
}



/* 
 *  TEST CASES
 *  
 * bom,sensor-based,eom
 * bom,m_ser,1,0,na,na,eom
 * bom,m_stp,1,0,na,na,eom
 * bom,m_dcm,1,0,na,0,eom
 * bom,s_usr,1,0,na,na,eom
 * bom,s_isr,1,0,na,na,eom
 * bom,s_for,1,0,na,na,eom
 * bom,m_ser,0,0,na,na,eom
 * bom,m_ser,1,0,na,na,eom
 * bom,m_stp,0,0,na,na,eom
 * bom,m_stp,1,0,na,na,eom
 * bom,m_dcm,0,0,na,na,eom
 * bom,m_dcm,1,0,na,na,eom
 * bom,s_usr,0,0,na,na,eom
 * bom,s_usr,1,0,na,na,eom
 * bom,s_irs,0,0,na,na,eom
 * bom,s_irs,1,0,na,na,eom
 * bom,s_for,0,0,na,na,eom
 * bom,s_for,1,0,na,na,eom
 * bom,s_for,0,0,na,na,eom
 * bom,gui-based,eom
 * bom,m_ser,1,0,na,na,eom
 * bom,m_stp,1,0,na,na,eom
 * bom,m_dcm,1,0,na,0,eom
 * bom,s_usr,1,0,na,na,eom
 * bom,s_isr,1,0,na,na,eom
 * bom,s_for,1,0,na,na,eom
 * bom,m_ser,1,20,0,na,eom
 * bom,m_ser,1,20,1,na,eom
 * bom,m_stp,1,30,0,na,eom
 * bom,m_stp,1,30,1,na,eom
 * bom,m_dcm,1,na,0,400,eom
 * bom,m_dcm,1,na,1,400,eom
 * bom,m_dcm,1,39,1,na,eom
 * bom,m_dcm,1,39,0,na,eom
 */

