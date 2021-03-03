/*!
 * @file IOTest.ino
 * @brief IO control test. You can input serial cmd by usb cdc. Speicfictd cmd please to view IOTest.txt
 * @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @licence     The MIT License (MIT)
 * @author [Arya](xue.peng@dfrobot.com)
 * @version  V1.0
 * @date  2021-1-29
 * @get from https://www.dfrobot.com
 * @url
 */
#include <Arduino.h>
#define FUNCTION_DIGITAL 1
#define FUNCTION_ANALOG  2
#define FUNCTION_PWM     3

uint8_t function = 0;
uint32_t pinNum = 0;
uint32_t mode = OUTPUT;
int dReadValue = 0; 
char serialCmdBuf[50];
uint8_t cmdLen = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while(!Serial){
  ; // wait for serial port to connect. Needed for native USB port only
  }
  
  Serial.println("IO testing ...");
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("Please send cmd, I already ready...");
  while(!Serial.available());
  Serial.print("CMD: ");
  memset(serialCmdBuf, 0 sizeof(serialCmdBuf));
  while(Serial.available()){
      serialCmdBuf[cmdLen++] = Serial.read();
      Serial.print()
      if(cmdLen > sizeof(serialCmdBuf)) {
          cmdLen = 0;
          break;
      }
  }
  Serial.println(serialCmdBuf);
  praseSerialCmd(serialCmdBuf);
  switch(function){
      case FUNCTION_DIGITAL:
        
        break;
      case FUNCTION_ANALOG:
        
        break;
      case FUNCTION_PWM:
        
        break;
      default:
        break;

  }
}

void praseSerialCmd(char *cmd){
  
}




