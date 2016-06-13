#include "Encoder.h"
#include <simpletools.h>

Encoder::Encoder(int newPin, int side):
pin1{newPin}, 
side{side}
{   
  //Start execution of code on a new cog.
  //We can not directly call a member function.
  //So we pass the pointer of this object as the parameter to achieve the same.
  cogId = cogstart(&Encoder::run, this, stack,  sizeof(Encoder)); 
}

Encoder::~Encoder(){
  //Stop the cog execution and free it for other purposes.
  cogstop(cogId); 
}   
 
unsigned int Encoder::getEncoderCount(){
  //Return the amount of counted pulses.
  return pulseCount; 
}

int Encoder::getDirection(){
  //Return the direction which the encoder is turning. 
  return direction; 
}
 
int Encoder::getSpeed(){
  //Return the amount of counted pulses per second.
  return speed;  
}

void Encoder::run(void* obj){
  //Since we are sure the object is of type Encoder we can safely cast it back to a Encoder pointer. 
  Encoder* enc = reinterpret_cast<Encoder*>(obj);
  enc->direction = -1;
  int oldDirection = -1;
  //algoritme to set the pins to that for both wheels forwards and backwards is the same way. 
  if(enc->side == 0){
    enc->pin2 = enc->pin1 + 1;
  } 
  else if(enc->side == 1){
     enc->pin2 = enc->pin1 - 1;
  }
  
  //Var contains the data what was read from the pin.     
  int readpin1, readpin2;
  
  //Var contains the data from last reading.
  //We use this to make a comparison to the new reading.
  //To make sure we have the correct start value we already make a reading here. 
  int readlast1 = input(enc->pin1);  
  int readlast2 = input(enc->pin2);  
  
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
    readpin1 = input(enc->pin1);
    readpin2 = input(enc->pin2);
    
    //Used quadrature encoder.
    //reads two different pins. 
    //if the state of one of the pins changes increase PulseCount.  
    if(readlast1 != readpin1 || readlast2 != readpin2){
      enc->pulseCount++; 
    }
    
    //Used quadrature encoder.
    //reads two different pins. 
    //if both last reads were low[0,0] or both were high[1,1].
    if((readlast1 && readlast2) || (!readlast1 && !readlast2)){
      //if last read was low AND pin is high [0,x] -> [1,x]
      //direction is backwards.  
      if(!readlast1 && readpin1){
        enc->direction = -1;
      }
      //if last read was low AND pin is high [x,0] -> [x,1] 
      //direction is forwards.
      else if(!readlast2 && readpin2){
        enc->direction = 1;
      }               
    }
    
    //if the direction changes pulseCount resets. 
    else if(enc->direction != oldDirection){
      enc->pulseCount = 0;
      oldDirection = enc->direction;
    }
    
    //Update the last pin state with the new.      
    readlast1 = readpin1;
    readlast2 = readpin2;
    
    count += elapsed_cnt;
    
    if(count >= 16000000){
       //Restet the counter to 0.
       //So we can begin counting a new period.
       count = 0;
       
       //Update the speed.
       //The speed is calculated to millimeter each second.
       int pulses = enc->pulseCount - last_pulse_count;
       int mm_driven = (pulses * 1000) / 8553;
       int mm_per_second = enc->direction * mm_driven * 5;
       
       enc->speed = mm_per_second;
       
       //Update the pulse count from a period ago with the current pulse count.
       last_pulse_count = enc->pulseCount;
    }
    
    //count times how long the encoder has speed ZERO
    //if(enc->speed == 0){
      //timer ++;   
    //}else{
      //timer = 0; 
    //}  
    
    //if encoder speed is ZERO for 250 milliseconds.
    //pulseCount to ZERO. 
    //if(timer >= 125){
      //enc->pulseCount = 0;  
    //}  
           
    //Wait 100 microseconds aka 0.1 milliseconds.
    //waitcnt(CNT + 1 * us); 
  }   
} 