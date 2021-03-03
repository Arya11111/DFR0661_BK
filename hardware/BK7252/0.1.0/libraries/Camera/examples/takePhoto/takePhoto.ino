#include "Arduino.h"
#include "DFCamera.h"
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while(!Serial){
      
  }
show_photo_test();
}

void loop() {
  // put your main code here, to run repeatedly:
  char c;
  while(!Serial.available());
  while(Serial.available()){
      if(Serial.peek() != '\r' || Serial.peek() != '\n')
          c = Serial.read();
      else Serial.println(Serial.read());
  }
  switch(c){
      case 's':
          Serial.println("start");
          start_photo();
          break;
      case 't':
          Serial.println("stop");
          exit_photo();
          break;
      default:
          Serial.println("cmd error!");
  }
}
