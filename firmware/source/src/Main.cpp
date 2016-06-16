////
//  ██████╗  ██████╗ ██████╗  ██████╗ ██████╗ ███████╗███████╗ ██████╗██╗   ██╗███████╗
//  ██╔══██╗██╔═══██╗██╔══██╗██╔═══██╗██╔══██╗██╔════╝██╔════╝██╔════╝██║   ██║██╔════╝
//  ██████╔╝██║   ██║██████╔╝██║   ██║██████╔╝█████╗  ███████╗██║     ██║   ██║█████╗  
//  ██╔══██╗██║   ██║██╔══██╗██║   ██║██╔══██╗██╔══╝  ╚════██║██║     ██║   ██║██╔══╝  
//  ██║  ██║╚██████╔╝██████╔╝╚██████╔╝██║  ██║███████╗███████║╚██████╗╚██████╔╝███████╗
//  ╚═╝  ╚═╝ ╚═════╝ ╚═════╝  ╚═════╝ ╚═╝  ╚═╝╚══════╝╚══════╝ ╚═════╝ ╚═════╝ ╚══════╝
//                                                                                                                                          
//
// @file Main.cpp
// @date Created: 15-06-2016
// @version 1.0
//
// @author Bouke Stam, Scott Mackay
//
// @section LICENSE
// License: newBSD
//
// Copyright © 2016, HU University of Applied Sciences Utrecht.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
// - Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
// - Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
// - Neither the name of the HU University of Applied Sciences Utrecht nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE HU UNIVERSITY OF APPLIED SCIENCES UTRECHT
// BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
// GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
// OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
////

#include <simpletools.h>
#include <stdio.h>            // Recommended over iostream for saving space
#include <propeller.h>        // Propeller-specific functions

#include "Qik.h"
#include "Commands.h"
#include "Uart.h"
#include "Encoder.h"
#include <time.h>

const int version = 1; 

Encoder * enc0;
Encoder * enc1;

volatile int motor_speed = 240;
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

// Checks if a value is inside a range of a target value
unsigned char in_range(int value, int target, int range){
  return value > target - range && value < target + range;
}  

double control_motor_power(double motor_power, int speed, int target_speed, double elapsed_seconds){
  // If the target speed is zero, just stop the motors
  if(target_speed == 0){
    return 0;
  }  
  
  // Variable that stores the amount of change in power
  double power_addition = 0.1;
  
  // Determine how much the power should be changed based on the difference between the target speed and current speed
  if(!in_range(speed, target_speed, 50)){
    power_addition = 20;
  }else if(!in_range(speed, target_speed, 30)){
    power_addition = 15;
  }else if(!in_range(speed, target_speed, 20)){
    power_addition = 10;
  }else if(!in_range(speed, target_speed, 10)){
    power_addition = 5;
  }else if(!in_range(speed, target_speed, 5)){
    power_addition = 2;
  }
  
  // Add or substract power to the current power and multiply by the delta time
  if(speed < target_speed){
    motor_power += power_addition * elapsed_seconds;
  }else if(speed > target_speed){
    motor_power -= power_addition * elapsed_seconds; 
  } 
  
  return motor_power;
}  

void controlMotors(void* obj){
  // Initialize QIK
  int transmit_pin_qik = 25;
  int receive_pin_qik = 26;
  int baud_rate = 115200;
  
  Qik qik(transmit_pin_qik, receive_pin_qik, baud_rate);
  
  // Stop motors at start
  qik.set_motor_speed(Qik::M0,0);
  qik.set_motor_speed(Qik::M1,0);
  
  // Variables to store the motor power
  double motor_power0 = 0;
  double motor_power1 = 0;
  
  // Variable to store the timestamp of the previous loop
  long previous_cnt = CNT;
  
  for(;;){
    // Calculate elapsed seconds since previous check
    long current_cnt = CNT;
    long elapsed_cnt = current_cnt - previous_cnt;
    previous_cnt = current_cnt;
    double elapsed_seconds = (double)elapsed_cnt / 80000000.0;
    
    // Store encoder speeds
    int encoder_speed0 = enc0->get_speed();
    int encoder_speed1 = enc1->get_speed();
    
    // Determine new motor power
    motor_power0 = control_motor_power(motor_power0, encoder_speed0, motor_speed * motor_direction0, elapsed_seconds); 
    motor_power1 = control_motor_power(motor_power1, encoder_speed1, motor_speed * motor_direction1, elapsed_seconds); 
    
    // Apply the new power values
    qik.set_motor_speed(Qik::M0, (int)motor_power0);
    qik.set_motor_speed(Qik::M1, (int)motor_power1);
    
    // Wait 250 milliseconds
    waitcnt(CNT + 20000000);
  }    
}

int main(){  
  // Initialize the encoders
  int left_wheel_pin = 0;
  int right_wheel_pin = 3;
  
  enc0 = new Encoder(left_wheel_pin, 0);
  enc1 = new Encoder(right_wheel_pin, 1);

  // Start the motor thread
  cog_run(controlMotors, 128);
  
  // Initialize UART
  Uart uart;
  
  // UART loop, blocks until a character is read
  while (1) {
    char c = uart.readChar();
    
    // Send the character and an OK signal back
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
        int speed = uart.readInt();
        set_motor_speed(speed);
      }break; 
      case uart_get_speed0:{
        uart.send(get_motor_speed0());
      break;}
      case uart_get_speed1:{
        uart.send(get_motor_speed1());
      break;}
      case uart_get_distance0:{
        //printf("\nDistance 0: %d\n", get_distance0());
        uart.send(get_distance0());
      break;}
      case uart_get_distance1:{
        //printf("\nDistance 1: %d\n", get_distance1());
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
    }      
  }  
  
  return 0;
}