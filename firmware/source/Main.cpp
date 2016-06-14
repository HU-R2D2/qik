/**
* This is the main Blank Simple C++ Project program file.
*/

#include <simpletools.h>
#include <stdio.h>            // Recommended over iostream for saving space
#include <propeller.h>        // Propeller-specific functions

#include "Qik.h"
#include "Commands.h"
#include "Uart.h"
#include "Encoder.h"
#include <time.h>
#include <Main.h>

Encoder * enc0;
Encoder * enc1;

volatile int motor_speed = 480;
volatile int motor_direction0 = 0;
volatile int motor_direction1 = 0;

void set_motor_speed(int speed){
  motor_speed = speed;
}

int get_motor_speed0(){
  return enc0->get_speed();
}

int get_motor_speed1(){
  return enc1->get_speed();;
}

int get_distance0(){
  return enc0->get_distance();
}

int get_distance1(){
  return enc1->get_distance();
}    

void set_motor_direction0(int direction){
  motor_direction0 = direction;
}

void set_motor_direction1(int direction){
  motor_direction1 = direction;
}

unsigned char in_range(int value, int target, int range){
  return value > target - range && value < target + range;
}  

double control_motor_power(double motor_power, int speed, int target_speed, double elapsed_seconds){
  if(target_speed == 0){
    return 0;
  }  
  
  int power_addition = 1;
  
  if(!in_range(speed, target_speed, 5)){
    power_addition = 5;
  }  
  
  if(!in_range(speed, target_speed, 10)){
    power_addition = 10;
  }
  
  if(!in_range(speed, target_speed, 20)){
    power_addition = 15;
  } 
  
  if(!in_range(speed, target_speed, 30)){
    power_addition = 20;
  }
  
  if(!in_range(speed, target_speed, 50)){
    power_addition = 30;
  } 
   
  if(speed < target_speed){
    motor_power += power_addition * elapsed_seconds;
  }else if(speed > target_speed){
    motor_power -= power_addition * elapsed_seconds; 
  } 
  
  return motor_power;
}  

void controlMotors(void* obj){
  //Pin connected to the tx pin of the Qik motor controller.
  int transmit_pin_qik = 25;
  
  //Pin connected to the rx pin of the Qik motor controller.
  int receive_pin_qik = 26;
  
  //Baud rate used to communicate with the Qik motor controller.
  int baud_rate = 115200;

  Qik qik(transmit_pin_qik, receive_pin_qik, baud_rate);
  
  //Stop the motors.
  //This is done so the rosbee won't drive away and/or stop while the 
  //program is rebooted.
  qik.set_motor_speed(Qik::M0,0);
  qik.set_motor_speed(Qik::M1,0);
  
  double motor_power0 = 0;
  double motor_power1 = 0;
  
  long previous_cnt = CNT;
  
  for(;;){
    long current_cnt = CNT;
    long elapsed_cnt = current_cnt - previous_cnt;
    previous_cnt = current_cnt;
    double elapsed_seconds = (double)elapsed_cnt / 80000000.0;
    
    int encoder_speed0 = enc0->getSpeed();
    int encoder_speed1 = enc1->getSpeed();
    
    motor_power0 = control_motor_power(motor_power0, encoder_speed0, motor_speed * motor_direction0, elapsed_seconds); 
    motor_power1 = control_motor_power(motor_power1, encoder_speed1, motor_speed * motor_direction1, elapsed_seconds); 
    
    qik.set_motor_speed(Qik::M0, (int)motor_power0);
    qik.set_motor_speed(Qik::M1, (int)motor_power1);
    
    waitcnt(CNT + 20000000);
  }    
}

int version = 1; 
int main(){  
  //Pin connected to the photointerrupter of the encoder on the left motor.
  int left_wheel_pin = 0;
  //Pin connected to the photointerrupter of the encoder on the right motor.
  int right_wheel_pin = 3;
  
  enc0 = new Encoder(left_wheel_pin, 0);
  enc1 = new Encoder(right_wheel_pin, 1);

  cog_run(controlMotors, 128);
  
  Uart uart;
  
  while (1) {
    char c = uart.readChar();
    
    uart.send(c);
    uart.send((char)uart_ok);
    
    switch(c){
      case uart_forward:{
        set_motor_direction0(1);
        set_motor_direction1(1);
      }break; 
      case uart_backward:{
        set_motor_direction0(-1);
        set_motor_direction1(-1);
      }break; 
      case uart_set_speed:{
        char speed = uart.readInt();
        set_motor_speed(speed);
      }break; 
      case uart_get_speed0:{
        uart.send(get_motor_speed0());
      break;}
      case uart_get_speed1:{
        uart.send(get_motor_speed1());
      break;}
      case uart_get_distance0:{
        uart.send(get_distance0());
      break;}
      case uart_get_distance1:{
        uart.send(get_distance1());
      break;}
      case uart_rotate_left:{
        set_motor_direction0(-1);
        set_motor_direction1(1);
      }break; 
      case uart_rotate_right:{
        set_motor_direction0(1);
        set_motor_direction1(-1);
      }break; 
      case uart_stop:{
        set_motor_direction0(0);
        set_motor_direction1(0);
     }break; 
      case uart_version:{
        uart.send(version);
      }break;
      case uart_encoder_speed0:{
        uart.send(enc0->getSpeed());
      }break; 
      case uart_encoder_speed1:{
        uart.send(enc1->getSpeed());
      }break; 
    }      
  }  
  
  return 0;
}