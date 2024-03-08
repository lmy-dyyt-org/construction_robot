#include "equipment.h"
#include <Arduino.h>
#if 0
/* led的，没啥用*/
Equipment::Equipment(int equipment_pin){
  pin = equipment_pin;
  pinMode(pin, OUTPUT);
}

void Equipment::cmdOn(){
  digitalWrite(pin, HIGH);  
}

void Equipment::cmdOff(){
  digitalWrite(pin, LOW);  
}
#endif