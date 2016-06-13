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
volatile int motor_speed=80;
volatile int motor0_direction=0;
volatile int motor1_direction=0;

void set_motor_speed(int power){
  motor_speed = power;
}

int get_motor_speed(){
  return (enc0->getSpeed() + enc1->getSpeed())/2;
}  

void set_motor0_direction(int direction){
  motor0_direction = direction;
}

void set_motor1_direction(int direction){
  motor1_direction = direction;
}

void controlMotors(void* obj){
  int old_motor0_direction = motor0_direction;
  int old_motor1_direction = motor1_direction;
  
  //Pin connected to the tx pin of the Qik motor controller.
  int transmit_pin_qik = 25;
  
  //Pin connected to the rx pin of the Qik motor controller.
  int receive_pin_qik = 26;
  
  //Baud rate used to communicate with the Qik motor controller.
  int baud_rate = 115200;

  Qik qik(transmit_pin_qik, receive_pin_qik, baud_rate);
  
  //value to change the value of the motor. 
  int countChangeStepSize = 100;
  
  //Stop the motors.
  //This is done so the rosbee won't drive away and/or stop while the 
  //program is rebooted.
  qik.set_motor_speed(Qik::M0,0);
  qik.set_motor_speed(Qik::M1,0);
  
  int set_power = 0;
  int adjusted_power = 0;
  
  
  
  double p = 0.8; // p...
  double i = 0; // integral
  double d = 0; // differential
  double iError0=0;
  double iError1=0;
  int previous_speed_difference0=0;
  int previous_speed_difference1=0;
  
  double power_motor0 = 0;
  double power_motor1 = 0;
  
  long last_time = CNT;
  long last_time2 = CNT;
  long last_time3 = CNT;
  
  const int MILLISECOND = 100000;
  const int SECOND = MILLISECOND * 1000;
  const int encoder_wait_time = MILLISECOND * 60;
  const int encoder_wait_period_time = MILLISECOND * 20;
  const int wait_time = MILLISECOND * 100;
  while(true){
    int temp_motor_speed = motor_speed;
    int temp_motor0_direction = motor0_direction;
    int temp_motor1_direction = motor1_direction;
    
    if(temp_motor0_direction != old_motor0_direction){
       qik.set_motor_speed(Qik::M0, (int)(power_motor0=0));
       old_motor0_direction = temp_motor0_direction;
    }
    if(temp_motor1_direction != old_motor1_direction){
       qik.set_motor_speed(Qik::M1, (int)(power_motor0=0));
       old_motor1_direction = temp_motor1_direction;
    }
    
    int encoder_speed0 = enc0->getSpeed();
    int encoder_speed1 = enc1->getSpeed();
    int encoder_total_count_speed = 0;
    int encoder_total_speed0 = 0;
    int encoder_total_speed1 = 0;
    while(CNT - last_time2 < encoder_wait_time){
      encoder_total_speed0+=enc0->getSpeed();
      encoder_total_speed1+=enc1->getSpeed();
      encoder_total_count_speed++;
      while(CNT - last_time3 < encoder_wait_period_time){}
      last_time3+=encoder_wait_period_time;
    }
    last_time2+=encoder_wait_time;
    
    if(encoder_total_count_speed > 0){
      encoder_speed0 = encoder_total_speed0/encoder_total_count_speed;
      encoder_speed1 = encoder_total_speed1/encoder_total_count_speed;
    }      
    int speed_difference0 = temp_motor0_direction*(temp_motor_speed-temp_motor0_direction*encoder_speed0);
    int speed_difference1 = temp_motor1_direction*(temp_motor_speed-temp_motor1_direction*encoder_speed1);
    
    double pError0  = p * (double)speed_difference0;
    double pError1  = p * (double)speed_difference1;
    iError0 += i * (double)speed_difference0;
    iError1 += i * (double)speed_difference1;
    double dError0 = d * (previous_speed_difference0-speed_difference0);
    double dError1 = d * (previous_speed_difference1-speed_difference1);
    
    if(speed_difference0 != 0){
      qik.set_motor_speed(Qik::M0, (int)(power_motor0+=pError0));
    }
    if(speed_difference1 != 0){
      qik.set_motor_speed(Qik::M1, (int)(power_motor1+=pError1));
    }
    
    if(power_motor0 > 100){power_motor0=100;}
    if(power_motor0 < -100){power_motor0=-100;}
    if(power_motor1 > 100){power_motor1=100;}
    if(power_motor1 < -100){power_motor1=-100;}
    
    previous_speed_difference0 = speed_difference0;
    previous_speed_difference1 = speed_difference1;
    
    //print("s0:%d  s1:%d\n", encoder_speed0, encoder_speed1);
    //print("sd0:%f  sd1:%f\n", speed_difference0, speed_difference1);
    
    while(CNT - last_time < wait_time){pause(2);}
    last_time+=wait_time;
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

  int temp=1;
  //motors(&temp);
  cog_run(controlMotors, 128);
  Uart uart;
  
  while (1) {
    char c = uart.readChar();
    
    uart.send(c);
    uart.send((char)uart_ok);
    
    switch(c){
      case uart_forward:{
        set_motor0_direction(1);
        set_motor1_direction(1);
      }break; 
      case uart_backward:{
        set_motor0_direction(-1);
        set_motor1_direction(-1);
      }break; 
      case uart_set_speed:{
        char speed = uart.readChar();
        set_motor_speed(speed);
      }break; 
      case uart_get_speed:{
        print("%d\n", get_motor_speed());
        //uart.send(get_motor_speed());
      break;}
      case uart_rotate_left:{
        set_motor0_direction(-1);
        set_motor1_direction(1);
      }break; 
      case uart_rotate_right:{
        set_motor0_direction(1);
        set_motor1_direction(-1);
      }break; 
      case uart_stop:{
        set_motor0_direction(0);
        set_motor1_direction(0);
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