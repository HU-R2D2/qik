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
  
//Pin connected to the tx pin of the Qik motor controller.
int transmit_pin_qik = 25;

//Pin connected to the rx pin of the Qik motor controller.
int receive_pin_qik = 26;

//Baud rate used to communicate with the Qik motor controller.
int baud_rate = 115200;

//static volatile Channel send, receive;

static int motor_power = 20;
static int motor_left_direction = 1;
static int motor_right_direction = 1;

void set_motor_power(int power){
  //TODO(Bouke): handle concurrency
  motor_power = power;
}

int get_motor_power(){
  return motor_power;
}  

void set_motor_left_direction(int direction){
  //TODO(Bouke): handle concurrency
  motor_left_direction = direction;
}

void set_motor_right_direction(int direction){
  //TODO(Bouke): handle concurrency
  motor_right_direction = direction;
}

void motors(void* obj){
  Qik qik(transmit_pin_qik, receive_pin_qik, baud_rate);

  //Pin connected to the photointerrupter of the encoder on the left motor.
  int left_wheel_pin = 0;
  //Pin connected to the photointerrupter of the encoder on the right motor.
  int right_wheel_pin = 3;
  
  Encoder enc0(left_wheel_pin, 0);
  Encoder enc1(right_wheel_pin, 1);
  
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
  
  for(;;){
    if(set_power != motor_power){
      qik.set_motor_speed(Qik::M0, motor_power);
      qik.set_motor_speed(Qik::M1, motor_power);
      
      set_power = motor_power;
    }else{
      int speed0 = enc0.getSpeed();
      int speed1 = enc1.getSpeed();
      if(speed0 < speed1){
         //drive a bit faster
         qik.set_motor_speed(Qik::M0, motor_power++);
      }else if(speed0 > speed1){
         //drive a bit slower
         qik.set_motor_speed(Qik::M0, motor_power--);
      } 
    }    
    
    pause(250);
  }    
}

int main(){
  print("Started\n");
 
  cog_run(motors, 100);
  
  Uart uart;
  
  while (1) {
    char c = uart.readChar();
    
    print("Received %c", c);
    
    switch(c){
      case uart_forward:{
        print("going forward");
        motor_left_direction = 1;
        motor_right_direction = 1;
      }break; 
      case uart_backward:{
        motor_left_direction = -1;
        motor_right_direction = -1;
      }break; 
      case uart_set_speed:{
        char speed = uart.readChar();
        print("setting speed");
        set_motor_power(speed);
        print("set speed to %d", speed);
      }break; 
      case uart_get_speed:{
        uart.send(get_motor_power());
      }break; 
      case uart_rotate_left:{
        motor_left_direction = -1;
        motor_right_direction = 1;
      }break; 
      case uart_rotate_right:{
        motor_left_direction = 1;
        motor_right_direction = -1;
      }break; 
      case uart_stop:{
        motor_left_direction = 0;
        motor_right_direction = 0;
      }break; 
      case uart_version:{
        uart.send(version);
      }break; 
    }      
  }  
  
  return 0;
}