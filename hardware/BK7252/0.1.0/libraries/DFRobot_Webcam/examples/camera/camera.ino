/*!
 * @file camera.ino
 * @brief take a photo and save jpg file in sdio flash.
 * @n  
 *
 * @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @licence     The MIT License (MIT)
 * @author [Arya](xue.peng@dfrobot.com)
 * @version  V1.0
 * @date  2021-2-24
 * @get from https://www.dfrobot.com
 * @url
 */
#include <SD.h>
#include "DFRobot_Audio.h"
#include "Webcam.h"

DFRobot_Audio audio;
Webcam camera;
const int buttonPin = 4;
int flag = 0;
//打印进度条 0x4033A8----0x403381
void setup(){
  pinMode(buttonPin, INPUT_PULLUP);
  Serial.begin(115200);
  while(!Serial);
  Serial.print("Initializing SD Card device...");
  if(!SD.begin()){
      Serial.println("initialization failed!");
      return;
  }
  Serial.println("initialization done.");
  
  Serial.print("Initializing camera device...");
  if(camera.begin(SD) < 0){
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
  
  Serial.println("Set Speakers Volume from 0 to 99: 80");
  audio.setSpeakersVolume(60);
  
  Serial.println("Ready to take photo");
}


void loop(){  
  if(!digitalRead(buttonPin)&&flag==0){  //Press user key to take photo
      while((!digitalRead(buttonPin))){  //Eliminate shaking
          delay(10);
      }
      if(camera.snapshot("/photo1.jpg") < 0){
          Serial.println("take photo1 fail.");
      }else{
          Serial.println("take photo1.");
          audio.playMusic("test.mp3");
          flag=1;
      }
  }
  if(!digitalRead(buttonPin)&&flag==1){  //Press user key to take photo
      while((!digitalRead(buttonPin))){  //Eliminate shaking
          delay(10);
      }
      if(camera.snapshot("/photo2.jpg") < 0){
          Serial.println("take photo1 fail.");
      }else{
          Serial.println("take photo2.");
          audio.playMusic("test.mp3");
          flag=0;
      }
  }
}