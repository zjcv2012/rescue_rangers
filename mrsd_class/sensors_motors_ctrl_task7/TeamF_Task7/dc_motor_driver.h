//Define struct for send motor status back
struct DC_Motor_Status{
  bool onoff;
  int degree;
  int vel;
  int dir;
};


void DC_Initial();
