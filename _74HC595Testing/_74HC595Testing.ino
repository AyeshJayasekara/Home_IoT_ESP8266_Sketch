//int SER_Pin = 5;   //pin 14 on the 75HC595
//int RCLK_Pin = 4;  //pin 12 on the 75HC595
//nt SRCLK_Pin = 0; //pin 11 on the 75HC595
/*
  ShiftRegister74HC595.h - Library for easy control of the 74HC595 shift register.
  Created by Timo Denk (www.simsso.de), Nov 2014.
  Additional information are available on http://shiftregister.simsso.de/
  Released into the public domain.
*/

#include "ShiftRegister74HC595.h"

// create shift register object (number of shift registers, data pin, clock pin, latch pin)
ShiftRegister74HC595 sr (3, 5, 4, 0); 
 
void setup() { 
  
}

void loop() {

  //sr.setAllHigh(); // set all pins HIGH
  //delay(500);
  
  sr.setAllLow(); // set all pins LOW
  delay(500); 
  
  //sr.set(5,HIGH);
  //delay(1000);
  for (int i = 0; i < 24; i++) {
    
    sr.set(i, HIGH); // set single pin HIGH
    delay(1250); 
  }
    for (int i = 0; i < 24; i++) {
    
    sr.set(i,LOW); // set single pin HIGH
    delay(1250); 
  }
  
  // set all pins at once
  //uint8_t pinValues[] = { B10101010 }; 
  //sr.setAll(pinValues); 
  //delay(1000);
  
  // read pin (zero based)
  //uint8_t stateOfPin5 = sr.get(5);
  
}
