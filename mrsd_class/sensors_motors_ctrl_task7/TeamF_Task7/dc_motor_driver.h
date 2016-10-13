//Define struct for send motor status back
struct DC_Motor_Status{
  int onoff;
  int degree;
  int vel;
  int dir;
};


void DC_Initial();
void updateState(int state, int velocity, int d, int degree);
void driveDCMotor();
void get_Motor_Status(DC_Motor_Status&);

