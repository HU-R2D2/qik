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
  int lastSpeed = 0;
  
  //Var used to count the time for if the speed is ZERO. 
  int timer = 0;
  
  //Start the reading, this will never end.
  while(true){
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
    
    //Add 1 tick the the time counter.
    //This is equal to 2 ms.
    count++;
    //Check if we are 100 tick further.
    //This is equal to 200 ms.
    if(count >= 100){
       //Restet the counter to 0.
       //So we can begin counting a new period.
       count = 0;
       //Update the speed.
       //The speed is equal to the current pulse count minus the pulse count a period ago.
       enc->speed = (enc->pulseCount - lastSpeed) * enc->direction;
       //Update the pulse count from a period ago with the current pulse count.
       lastSpeed = enc->pulseCount;        
    }
    
    //count times how long the encoder has speed ZERO
    if (enc->speed == 0){
      timer ++;   
    } 
    else {
      timer = 0; 
    }  
    //if encoder speed is ZERO for 250 milliseconds.
    //pulseCount to ZERO. 
    if (timer >= 125){
      enc->pulseCount = 0;  
    }         
    //Wait 2000 microseconds aka 2 milliseconds.
    waitcnt(CNT + 20 * us);      
  }   
} 