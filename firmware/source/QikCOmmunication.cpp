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

int version = 1;

Encoder * enc0;
Encoder * enc1;

volatile int motor_power = 10;
volatile int motor0_direction = 0;
volatile int motor1_direction = 0;

void set_motor_power(int power){
  //TODO(Bouke): handle concurrency
  motor_power = power;
}

int get_motor_power(){
  return motor_power;
}  

void set_motor0_direction(int direction){
  //TODO(Bouke): handle concurrency
  motor0_direction = direction;
}

void set_motor1_direction(int direction){
  //TODO(Bouke): handle concurrency
  motor1_direction = direction;
}

void motors(void* obj){
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
  
  //Encoder counts for 1 wheel turn for the rosbee. for another wheel needs other value. 
  int totalCounts360WheelTurn = 3000; // In encoder counts
  
  //circumference of the wheel of the rosbee in mm. 
  int wheelCircumference = 386; // In mm
  
  int set_power = 0;
  int adjusted_power = 0;
  
  while(true){
    int temp_motor_power = motor_power;
    int temp_motor0_direction = motor0_direction;
    int temp_motor1_direction = motor1_direction;
    
    if(temp_motor0_direction != old_motor0_direction || set_power != temp_motor_power){
      old_motor0_direction = temp_motor0_direction;
      adjusted_power = old_motor0_direction*temp_motor_power;
      //qik.set_motor_speed(Qik::M0, adjusted_power);
    }
    if(set_power != temp_motor_power || temp_motor1_direction != old_motor1_direction){
      old_motor1_direction = temp_motor1_direction;
      qik.set_motor_speed(Qik::M1, old_motor1_direction*temp_motor_power);
      set_power = temp_motor_power;
    }else{
      int speed0 = enc0->getSpeed();
      int speed1 = enc1->getSpeed();
      if(temp_motor0_direction == temp_motor1_direction){
        if(speed0 < speed1){
           qik.set_motor_speed(Qik::M0, adjusted_power+=1);
           
        }else if(speed0 > speed1){
           qik.set_motor_speed(Qik::M0, adjusted_power-=1);
        }
      } else{
        if(-speed0 < speed1){
           qik.set_motor_speed(Qik::M0, adjusted_power-=1);
           
        }else if(-speed0 > speed1){
           qik.set_motor_speed(Qik::M0, adjusted_power+=1);
        }
      }
      if(adjusted_power > 100){adjusted_power = 100;}
      if(adjusted_power < -100){adjusted_power = -100;}
      
      //print("s0:%d\n", speed0);
      //print("s1:%d\n", speed1);
      //print("adjustedPower:%d\n", adjusted_power);
    }
    pause(30);
  }    
}

int main(){
  //Pin connected to the photointerrupter of the encoder on the left motor.
  int left_wheel_pin = 0;
  //Pin connected to the photointerrupter of the encoder on the right motor.
  int right_wheel_pin = 3;
  
  enc0 = new Encoder(left_wheel_pin, 0);
  enc1 = new Encoder(right_wheel_pin, 1);
  
  int temp=1;
  //motors(&temp);
  cog_run(motors, 128);
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
        set_motor_power(speed);
      }break; 
      case uart_get_speed:{
        uart.send(get_motor_power());
      }break; 
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