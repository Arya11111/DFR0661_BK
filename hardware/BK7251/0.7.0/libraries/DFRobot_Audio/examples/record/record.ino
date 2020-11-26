/*!
 * @file record.ino
 * @brief 录音，通过串口命令实现录音功能，并将录音文件存储在SD卡的record.pcm文件中。
 *
 * @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @licence     The MIT License (MIT)
 * @author [Arya](xue.peng@dfrobot.com)
 * @version  V1.0
 * @date  2020-09-16
 * @get from https://www.dfrobot.com
 * @url
 */
 
#include <SD.h>
#include "DFRobot_Audio.h"

enum Status{
    ePrepare,
    eRecording,
    eStop
}eState=ePrepare;
const char *state[]={
  "ePrepare",
  "eRecording",
  "eStop"
};

DFRobot_Audio audio;

void setup(){
  Serial.begin(115200);
  while(!Serial);
  Serial.print("Initializing SD Card device...");
  if(!SD.begin()){
      Serial.println("initialization failed!");
      return;
  }
  Serial.println("initialization done.");
  Serial.print("Initializing Audio device...");
  if(!audio.begin(SD)){
      Serial.println("initialization failed!");
      return;
  }
  Serial.println("initialization done.");
  
  audio.initRecorder();
  audio.record("/record.pcm");
  //audio.recorderControl(RECORD_BEGIN);
}
char cmd;
void loop(){
  while(!Serial.available());
  while(Serial.available()){
	  char c = Serial.read();
	  if(c == '\r' || c == '\n') continue;
	  cmd = c;
  }
  switch(cmd){
	  case 'b':
	      audio.recorderControl(RECORD_BEGIN);
		  break;
	  case 's':
	      audio.recorderControl(RECORD_STOP);
		  break;
	   case 'p':
       audio.playMusic("/record.pcm");
      // audio_device_set_rate(8000);
       break;
  }
  Serial.println(cmd);
}