struct StepperMotorStatus
{   
  int angle;
  int spd;
  bool on_off;
};

struct IRSensorStatus
{
  int Distance;
  bool on_off;
};


extern StepperMotorStatus stepperStatus;
extern IRSensorStatus irSensorStatus;

void setupStepper();
void updateStepperState(int angle, int spd, bool on_off);
void driveStepper();

void updateIRSensorState(bool on_off);
void IRSensorAndStepper();


