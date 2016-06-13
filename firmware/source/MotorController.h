#ifndef _MOTOR_CONTROLLER_H
#define _MOTOR_CONTROLLER_H 

#include "Encoder.h"

class MotorController : public EncoderListener{
private:
  int motor_id;
  Encoder * enc;
  
  //Encoder counts for 1 wheel turn for the rosbee. for another wheel needs other value.
  const static int totalCounts360WheelTurn=3000; // In encoder counts
  //circumference of the wheel of the rosbee in mm.
  const static int wheelCircumference=386; // In mm
  volatile int motor_speed;
  volatile int motor_direction;
  
  char stack[250];
  int cogId;
  static void run(void* obj);
public:
  MotorController(int motor_id, Encoder * enc);
    
  void set_motor_speed(int power);
  
  int get_motor_speed();
  
  void set_motor_direction(int direction);
  
  void new_speed(int speed);
};

#endif // _MOTOR_CONTROLLER_H