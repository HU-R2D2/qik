#include "MotorController.h"
#include "Qik.h"
#include <simpletools.h>

MotorController::MotorController(int motor_id, Encoder * enc) : 
  motor_id(motor_id), enc(enc), motor_speed(10), motor_direction(0){
  cogId = cogstart(&MotorController::run, this, stack,  sizeof(MotorController));
  enc->add_encoder_listener(this);
}
  
void MotorController::set_motor_speed(int power){
  motor_speed = power;
}

int MotorController::get_motor_speed(){
  return motor_speed;
}  

void MotorController::set_motor_direction(int direction){
  motor_direction = direction;
}

void MotorController::run(void* obj){
  MotorController* motorController = reinterpret_cast<MotorController*>(obj);
  
  int old_motor0_direction = motorController->motor_direction;
  
  //Pin connected to the tx pin of the Qik motor controller.
  const int transmit_pin_qik = 25;
  
  //Pin connected to the rx pin of the Qik motor controller.
  const int receive_pin_qik = 26;
  
  //Baud rate used to communicate with the Qik motor controller.
  const int baud_rate = 115200;

  Qik qik(transmit_pin_qik, receive_pin_qik, baud_rate);
  
  //value to change the value of the motor. 
  int countChangeStepSize = 100;
  
  //Stop the motors.
  //This is done so the rosbee won't drive away and/or stop while the 
  //program is rebooted.
  qik.set_motor_speed(motorController->motor_id,0);
  
  //Encoder counts for 1 wheel turn for the rosbee. for another wheel needs other value. 
  int totalCounts360WheelTurn = 3000; // In encoder counts
  
  //circumference of the wheel of the rosbee in mm. 
  int wheelCircumference = 386; // In mm
  
  int set_power = 0;
  int adjusted_power = 0;
  
  
  
  double p = 0.8; // p...
  double i = 0; // integral
  double d = 0; // differential
  double iError0=0;
  int previous_speed_difference0=0;
  
  double power_motor0 = 0;
  
  long last_time = CNT;
  long last_time2 = CNT;
  long last_time3 = CNT;
  
  const int MILLISECOND = 100000;
  const int SECOND = MILLISECOND * 1000;
  const int encoder_wait_time = MILLISECOND * 60;
  const int encoder_wait_period_time = MILLISECOND * 20;
  const int wait_time = MILLISECOND * 100;
  while(true){
    int temp_motor_speed = motorController->motor_speed;
    int temp_motor0_direction = motorController->motor_direction;
    
    if(temp_motor0_direction != old_motor0_direction){
       qik.set_motor_speed(motorController->motor_id, (int)(power_motor0=0));
       old_motor0_direction = temp_motor0_direction;
    }
    
    int encoder_speed0 = motorController->enc->getSpeed();
    int encoder_total_count_speed = 0;
    int encoder_total_speed0 = 0;
    while(CNT - last_time2 < encoder_wait_time){
      encoder_total_speed0+=motorController->enc->getSpeed();
      encoder_total_count_speed++;
      while(CNT - last_time3 < encoder_wait_period_time){}
      last_time3+=encoder_wait_period_time;
    }
    last_time2+=encoder_wait_time;
    
    if(encoder_total_count_speed > 0){
      encoder_speed0 = encoder_total_speed0/encoder_total_count_speed;
    }      
    int speed_difference0 = temp_motor0_direction*(temp_motor_speed-temp_motor0_direction*encoder_speed0);
    
    double pError0  = p * (double)speed_difference0;
    iError0 += i * (double)speed_difference0;
    double dError0 = d * (previous_speed_difference0-speed_difference0);
    
    if(speed_difference0 != 0){
      qik.set_motor_speed(Qik::M0, (int)(power_motor0+=pError0));
    }
    if(power_motor0 > 100){power_motor0=100;}
    if(power_motor0 < -100){power_motor0=-100;}
    
    previous_speed_difference0 = speed_difference0;
    
    //print("s0:%d  s1:%d\n", encoder_speed0, encoder_speed1);
    //print("sd0:%f  sd1:%f\n", speed_difference0, speed_difference1);
    
    while(CNT - last_time < wait_time){pause(2);}
    last_time+=wait_time;
  } 
}

void MotorController::new_speed(int speed){
    
}