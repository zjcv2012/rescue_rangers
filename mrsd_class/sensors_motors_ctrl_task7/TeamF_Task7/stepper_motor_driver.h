struct StepperMotorStatus
{   
  int angle;
  bool on_off;
};

struct IRSensorStatus
{
  int Distance;
  bool on_off;
};


extern StepperMotorStatus stepperStatus;
extern IRSensorStatus irSensorStatus;

void driveStepper(int angle_2, bool on_off);
void setupStepper();

