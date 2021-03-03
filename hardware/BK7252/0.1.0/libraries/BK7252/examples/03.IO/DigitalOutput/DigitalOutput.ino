/*!
 * @file DigitalOutput.ino
 * @brief digital io output. 通过输入串口命令来控制数字IO口
 * @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @licence     The MIT License (MIT)
 * @author [Arya](xue.peng@dfrobot.com)
 * @version  V1.0
 * @date  2021-1-29
 * @get from https://www.dfrobot.com
 * @url
 */
#include <Arduino.h>
#define PIN_NUMBERS_NONE         255
#define FUNCTION_DIGITAL 1
#define FUNCTION_ANALOG  2
#define FUNCTION_PWM     3


typedef struct{
  String cmd;
  uint8_t funNum;
}sFunCmd_t, *pFunCmd_t;

typedef struct{
  String des;
  uint8_t mode;
}sModeMap_t, *pModeMap_t;

typedef struct{
  String pin;
  uint8_t pinNum;
}sPinMap_t, *pPinMap_t;

char serialCmdBuf[50];
uint8_t cmdLen = 0;
sFunCmd_t funcCmds[]={
{"digital", FUNCTION_DIGITAL}
};

sModeMap_t modeMap[]={
{"OUTPUT", OUTPUT},//
{"INPUT", INPUT},
{"INPUT_PULLUP", INPUT_PULLUP},
{"INPUT_PULLDOWN", INPUT_PULLDOWN},
};
sPinMap_t pinMap[]={
{"RXD", 0}, {"TXD", 1}, {"SDA", SDA},{"SCL", SCL},{"MI", MISO},{"MISO", MISO},
{"MO", MOSI},{"MOSI", MOSI},{"SCK", SCK},{"VRE",0},
{"A0", A0},{"A1", A1},{"A2", A2},{"A3", A3},{"A4", A4},
{"A_N", 0},{"A_P", 0},{"M_N", 0},{"M_P", 0}
};
extern void usage(void);
extern void digitalPrase(char *cmd);
extern bool getKeyValue(char *cmd, char *key, String &value);
extern uint32_t getPinMode(String val);
extern uint32_t getPinNumbers(String val);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while(!Serial){
  ; // wait for serial port to connect. Needed for native USB port only
  }
  
  Serial.println("digital IO testing ...");
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("Please send cmd, I already ready...");
  cmdLen = 0;
  while(!Serial.available());
  Serial.println(Serial.available());
  Serial.print("CMD: ");
  memset(serialCmdBuf, 0, sizeof(serialCmdBuf));
  //保证接收的是一个正确的命令，将多余的空格去掉
  while(Serial.available()){
      serialCmdBuf[cmdLen] = Serial.read();
      //Serial.println(serialCmdBuf[cmdLen]);
      if(cmdLen == 0 && serialCmdBuf[cmdLen] == ' ') continue;
      if((cmdLen > 0)&&(serialCmdBuf[cmdLen] == ' ') && ((serialCmdBuf[cmdLen-1] == ' ') || (serialCmdBuf[cmdLen-1] == '='))) continue;
      if((cmdLen > 0) && (serialCmdBuf[cmdLen] == '=') && (serialCmdBuf[cmdLen-1] == ' ')){
          serialCmdBuf[cmdLen-1] = '=';
          continue;
      }
      cmdLen++;
      if(serialCmdBuf[cmdLen] == '\r') continue;
      if(serialCmdBuf[cmdLen] == '\n') break;
      
      if(cmdLen > sizeof(serialCmdBuf)) {
          cmdLen = 0;
          break;
      }
      //Serial.print(Serial.available());
      delay(2);
  }
  serialCmdBuf[cmdLen] = 0;
  Serial.println(serialCmdBuf);
  praseSerialCmd(serialCmdBuf);


}

void praseSerialCmd(char *cmd){
  while(cmd[0] == ' '){
      cmd++;
  }
  char *pcmd = cmd;
  int index = String(pcmd).indexOf(' ');
  sFunCmd_t funCmd;
  memset(&funCmd, 0, sizeof(sFunCmd_t));
  funCmd.cmd = String(cmd).substring(0, index);
  pcmd += (index + 1);
  for(int i = 0; i < sizeof(funcCmds)/sizeof(sFunCmd_t); i++){
      if(funCmd.cmd == funcCmds[i].cmd){
          funCmd.funNum = funcCmds[i].funNum;
          break;
      }
      if(i+1 == sizeof(funcCmds)/sizeof(sFunCmd_t)){
          Serial.println("error function cmd!");
          return;
      }
  }
  switch(funCmd.funNum){
      case FUNCTION_DIGITAL:
           digitalPrase(pcmd);
           break;
      default:
           Serial.println("error function cmd!");
           break;
          
  }
  
  
}

/*串口命令*/
void usage(void){
  Serial.println("usage: digital [key]=[value] [key]=[value] ...");
  Serial.println("examples:");
  Serial.println("digital mode=OUTPUT pin=0 level=HIGH");
  Serial.println("digital mode=INPUT pin=0");
}

void digitalPrase(char *cmd){
  while(cmd[0] == ' '){
      cmd++;
  }
  char *pcmd = cmd;
  String cmdVal;
  uint32_t pin, mode, level;
  uint8_t len = 0;
  if(getKeyValue(pcmd, "mode=", cmdVal)){
      mode = getPinMode(cmdVal);
  }else{
      Serial.println("mode error.");
      return;
  }
  
  if(getKeyValue(pcmd, "pin=", cmdVal)){
      pin = getPinNumbers(cmdVal);
  }else{
      Serial.println("pin error.");
      return;
  }
  Serial.print("pin=");Serial.println(pin);
  Serial.print("mode=");Serial.println(mode);
  pinMode(pin, mode);
  if(mode == OUTPUT){
      if(getKeyValue(pcmd, "level=", cmdVal)){
          char c = cmdVal.charAt(0);
		  Serial.print("pin:");Serial.print(cmdVal);Serial.print(" "); Serial.println(cmdVal.length());
          if(c >= '0' && c <= '1'){
              level = cmdVal.toInt();
          }else{
              if(cmdVal.equalsIgnoreCase("HIGH")){
                  level = HIGH;
              }else if(cmdVal.equalsIgnoreCase("LOW")){
                  level = LOW;
              }else{
                  Serial.println("error level");
                  return;
              }
          }
      }else{
          Serial.println("error level");
          return;
      }
	  Serial.print("level=");Serial.println(level);
      digitalWrite(pin, level);
  }else{
      Serial.print("Read digital value: ");
      for(int i = 0; i < 10; i++){
          Serial.println(digitalRead(pin));
      }
  }
}

bool getKeyValue(char *cmd, char *key, String &value){
  char *pcmd = cmd;
  int idx = String(pcmd).indexOf(String(key));
  if(idx < 0) return false;
  pcmd += idx;
  idx = String(pcmd).indexOf(' ');
  if(idx < 0){
      idx = String(pcmd).indexOf('\r');
      if(idx < 0) return false;
  }
  String strCmd = String(pcmd).substring(0, idx);
  if(strCmd.length() <= String(key).length()) return false;
  idx = strCmd.indexOf('=');
  value = strCmd.substring(idx+1);
  return true;
}

uint32_t getPinMode(String val){
  uint32_t mode;
  char c = val.charAt(0);
  Serial.print("mode:");Serial.print(val);Serial.print(" "); Serial.println(val.length());
  if(c >= '0' && c <= '1'){
      mode = val.toInt();
  }else{
      for(int i = 0; i < sizeof(modeMap)/sizeof(sModeMap_t); i++){
          if(val.equalsIgnoreCase(modeMap[i].des)){
              return (uint32_t) modeMap[i].mode;
          }
          if(i+1 == sizeof(modeMap)/sizeof(sModeMap_t)){
              mode = PIN_NUMBERS_NONE;
          }
      }
  }
  return mode;
}

uint32_t getPinNumbers(String val){
  uint32_t pin;
  char c = val.charAt(0);
  Serial.print("pin:");Serial.print(val);Serial.print(" "); Serial.println(val.length());
  if(c >= '0' && c <= '9'){
      pin = val.toInt();
  }else{
      for(int i = 0; i < sizeof(pinMap)/sizeof(sPinMap_t); i++){
          if(val.equalsIgnoreCase(pinMap[i].pin)){
              return (uint32_t) pinMap[i].pinNum;
          }
          if(i+1 == sizeof(pinMap)/sizeof(sPinMap_t)){
              pin = PIN_NUMBERS_NONE;
          }
      }
  }
  return pin;
}




