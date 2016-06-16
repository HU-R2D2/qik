////
//  ██████╗  ██████╗ ██████╗  ██████╗ ██████╗ ███████╗███████╗ ██████╗██╗   ██╗███████╗
//  ██╔══██╗██╔═══██╗██╔══██╗██╔═══██╗██╔══██╗██╔════╝██╔════╝██╔════╝██║   ██║██╔════╝
//  ██████╔╝██║   ██║██████╔╝██║   ██║██████╔╝█████╗  ███████╗██║     ██║   ██║█████╗  
//  ██╔══██╗██║   ██║██╔══██╗██║   ██║██╔══██╗██╔══╝  ╚════██║██║     ██║   ██║██╔══╝  
//  ██║  ██║╚██████╔╝██████╔╝╚██████╔╝██║  ██║███████╗███████║╚██████╗╚██████╔╝███████╗
//  ╚═╝  ╚═╝ ╚═════╝ ╚═════╝  ╚═════╝ ╚═╝  ╚═╝╚══════╝╚══════╝ ╚═════╝ ╚═════╝ ╚══════╝
//                                                                                                                                          
//
// @file Encoder.cpp
// @date Created: 15-06-2016
// @version 1.0
//
// @author Bouke Stam, Scott Mackay, Nathan Schaaphuizen, Robert Zegers, Kevin Damen
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

#include "Encoder.h"
#include <simpletools.h>

Encoder::Encoder(int newPin, int side):
pin1{newPin}, 
side{side}
{   
  //Start execution of code on a new cog.
  //We can not directly call a member function.
  //So we pass the pointer of this object as the parameter to achieve the same.
  cog_id = cogstart(&Encoder::run, this, stack,  sizeof(Encoder)); 
}

Encoder::~Encoder(){
  //Stop the cog execution and free it for other purposes.
  cogstop(cog_id); 
}   
 
unsigned int Encoder::get_encoder_count(){
  //Return the amount of counted pulses.
  return pulse_count; 
}

int Encoder::get_direction(){
  //Return the direction which the encoder is turning. 
  return direction; 
}
 
int Encoder::get_speed(){
  //Return the amount of counted pulses per second.
  return speed;  
}

void Encoder::run(void* obj){
  //Since we are sure the object is of type Encoder we can safely cast it back to a Encoder pointer. 
  Encoder* enc = reinterpret_cast<Encoder*>(obj);
  enc->direction = -1;
  
  //algoritme to set the pins to that for both wheels forwards and backwards is the same way. 
  if(enc->side == 0){
    enc->pin2 = enc->pin1 + 1;
  }else if(enc->side == 1){
     enc->pin2 = enc->pin1 - 1;
  }
  
  //Var contains the data what was read from the pin.     
  int read_pin1, read_pin2;
  
  //Var contains the data from last reading.
  //We use this to make a comparison to the new reading.
  //To make sure we have the correct start value we already make a reading here. 
  int read_last1 = input(enc->pin1);  
  int read_last2 = input(enc->pin2);  
  
  //Var used to count the time for speed.
  int count = 0;
  
  //Var used to the calculate the speed.
  int last_pulse_count = 0;
  
  //Var used to count the time for if the speed is ZERO. 
  int timer = 0;
  
  long begin_cnt = CNT;
  
  //Start the reading, this will never end.
  while(true){
    long current_cnt = CNT;
    long elapsed_cnt = current_cnt - begin_cnt;
    begin_cnt = current_cnt;
    
    //Read the state of the pin.
    //1 = endocer blocked
    //0 = encoder can see the led
    read_pin1 = input(enc->pin1);
    read_pin2 = input(enc->pin2);
    
    //Used quadrature encoder.
    //reads two different pins. 
    //if the state of one of the pins changes increase PulseCount.  
    if(read_last1 != read_pin1 || read_last2 != read_pin2){
      enc->pulse_count++; 
    }
    
    //Used quadrature encoder.
    //reads two different pins. 
    //if both last reads were low[0,0] or both were high[1,1].
    if((read_last1 && read_last2) || (!read_last1 && !read_last2)){
      //if last read was low AND pin is high [0,x] -> [1,x]
      //direction is backwards.  
      if(!read_last1 && read_pin1){
        enc->direction = -1;
      }
      //if last read was low AND pin is high [x,0] -> [x,1] 
      //direction is forwards.
      else if(!read_last2 && read_pin2){
        enc->direction = 1;
      }               
    }
    
    //Update the last pin state with the new.      
    read_last1 = read_pin1;
    read_last2 = read_pin2;
    
    count += elapsed_cnt;
    
    if(count >= 16000000){
       count = 0;
       
       int pulses = enc->pulse_count - last_pulse_count;
       
       int mm_driven = (pulses * 1000) / 8553;
       
       // Calculate the absolute distance, the distance counter always counts upwards
       // like a car, if you drive backwards the counter doesn't go down
       int abs_distance = mm_driven < 0 ? mm_driven * -1 : mm_driven;
       
       enc->distance += abs_distance;
       
       // Multiply the distance by the direction and 5 (because this is the distance 
       // traveled in 1/5 second) and we want to know the distance per second
       int mm_per_second = enc->direction * mm_driven * 5;
       
       enc->speed = mm_per_second;
       
       last_pulse_count = enc->pulse_count;
    }
  }   
} 

int Encoder::get_distance(){
  return distance;
}  