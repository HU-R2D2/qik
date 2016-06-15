////
//  ██████╗  ██████╗ ██████╗  ██████╗ ██████╗ ███████╗███████╗ ██████╗██╗   ██╗███████╗
//  ██╔══██╗██╔═══██╗██╔══██╗██╔═══██╗██╔══██╗██╔════╝██╔════╝██╔════╝██║   ██║██╔════╝
//  ██████╔╝██║   ██║██████╔╝██║   ██║██████╔╝█████╗  ███████╗██║     ██║   ██║█████╗  
//  ██╔══██╗██║   ██║██╔══██╗██║   ██║██╔══██╗██╔══╝  ╚════██║██║     ██║   ██║██╔══╝  
//  ██║  ██║╚██████╔╝██████╔╝╚██████╔╝██║  ██║███████╗███████║╚██████╗╚██████╔╝███████╗
//  ╚═╝  ╚═╝ ╚═════╝ ╚═════╝  ╚═════╝ ╚═╝  ╚═╝╚══════╝╚══════╝ ╚═════╝ ╚═════╝ ╚══════╝
//                                                                                                                                          
//
// @file Qik.cpp
// @date Created: 13-04-2015
// @version 1.0
//
// @author Nathan Schaaphuizen
//
// @section LICENSE
// License: newBSD
//
// Copyright © 2015, HU University of Applied Sciences Utrecht.
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
#include "Qik.h"
#include "Commands.h"

Qik::Qik(int temp_transmit_pin, int temp_receive_pin, int baud_rate):
transmit_pin{temp_transmit_pin},
receive_pin{temp_receive_pin},
bit_width{(US/baud_rate)*us}
{
  //Configure  pin transmit_pin as a output pin.
  set_direction(transmit_pin, OUTPUT);
  high(transmit_pin);
  //Configure  pin receive_pin as a input pin.
  set_direction(receive_pin, INPUT);
  //Send the synchronisation byte.
  //The Qik 2s12v10 expects this to be the first byte and uses it
  //to determine the baud rate used for now on.
  execute_command(command_sync);
}

void Qik::execute_command(unsigned char byte){ 
  //Start bit:
  //Make output pin low. The output pin is high in rest.
  low(transmit_pin);
  //Wait the length  of 1 bit.
  waitcnt(CNT+bit_width);
  //8 data bits:
  //Send 8 data bits LSB.
  for(int i=0;i<8;++i){
    //Check the LSB (most right bit).
    //If bit is 1:
    if (byte & 0x01 != 0){  
      //Make ouput pin high.
      //1 = high
      high(transmit_pin);
    }
    //If bit is 0:
    else{
      //Make ouput pin low.
      //0 = low
      low(transmit_pin);
    }
    //Shift all bits 1 position to the right.
    //This will give a new LSB to send.
    byte >>= 1;
    //Wait the length of 1 bit.
    waitcnt(CNT+bit_width);          
  }
  //Stop bit:
  //Make output pin high. The output pin is high in rest.
  high(transmit_pin);
  //Wait the length of 1 bit.
  waitcnt(CNT+bit_width);  
}

unsigned char Qik::command_response(){
  //Char to fill.
  unsigned char byte = 0;
  //Wait for the start bit to be over.
  waitcnt(CNT+bit_width); 
  //Read 8 data bits:    
  for(int i=0;i<8;++i){
    //Check the input pin.
    if (input(receive_pin) == 1){
      //Write a 1 on the MSB.
      byte |= 0x80;  
    }
    //Shift all bits 1 position to the right.      
    byte >>= 1;
    //Wait the length of 1 bit.
    waitcnt(CNT+bit_width);               
  }
  //Wait for the stop bit to be over.
  waitcnt(CNT+bit_width);
  //Return the newly filled byte.
  return byte;  
}

void Qik::set_motor_speed(int motor, int speed){
  //Flag holding the direction the motor needs to turn.
  //0x00 = CCW
  //0x02 = CW
  char dirFlag = 0x00;
  
  //Check if speed is negative.
  //If so we need to turn the motor the other way around.
  if (speed < 0){
    //Set motor direction clockwise.
    dirFlag = 0x02;  
  }
  
  //Make the speed absolute. Does nothing if it already was.
  //Speed is expected in a range from 0 to 100 included.
  speed = abs(speed);
  
  if(speed > 127) speed = 127;
  
  //Flag holding the motor that needs to change.
  //0x00 = motor 0
  //0x04 = motor 1
  char motorFlag = 0x00;
  
  //Check if motor is motor 1. If not we assume motor 0.
  if(motor == M1){
    //Set the motor to motor 1.
    motorFlag = 0x04;
    //Invert the motor direction.
    //Please note that the motors are placed as each other's mirror image.
    //Causing the motors set in the same direction to turn in each other's opposite direction instead.
    //To solve this problem we invert one of the directions provided by the user.
    if(dirFlag) dirFlag = 0x00;
    else dirFlag = 0x02; 
  }
  
  //The base motor command.
  char motorCommand = 0x88;
  
  //Add the direction flag.
  motorCommand |= dirFlag;
  
  //Add the motor flag.
  motorCommand |= motorFlag;    
  
  //Send the motor command byte.
  execute_command(motorCommand);
  
  //Send the motor speed byte.
  execute_command(speed);      
}


void Qik::set_brake_power(int motor, unsigned char strength){
  //Check if strength exceeds the maximum  if so set it
  //to the highest allowed value.
  //The motor brake strength is expected between 0 and 127 included.
  if(strength > 127) strength = 127;
  
  //Motor command.
  char motorCommand = command_brake_M0;
  
  //Check if motor is motor 1. If not we assume motor 0.
  if(motor == M1){
    //Set motor command to motor 1.
    motorCommand = command_brake_M1;
  }    
  
  //Send motor brake strength command byte.
  execute_command(motorCommand);
  
  //Send motor brake strength.
  execute_command(strength);  
}    
