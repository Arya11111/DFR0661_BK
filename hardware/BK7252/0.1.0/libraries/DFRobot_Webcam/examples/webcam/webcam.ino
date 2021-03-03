/*!
 * @file webcam.ino
 * @brief 网络摄像头拍照，设置照片显示大小，设置帧传输率，启动网络摄像头。打开web浏览器，输入服务器访问地址
 * @n  移动摄像头可看到移动的画面，确定好画面后，按用户键~4拍照，拍照完成后，会播放咔嚓声。
 *
 * @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @licence     The MIT License (MIT)
 * @author [Arya](xue.peng@dfrobot.com)
 * @version  V1.0
 * @date  2021-3-1
 * @get from https://www.dfrobot.com
 * @url
 */
#include <WiFi.h>
#include "DFRobot_Audio.h"
#include "Webcam.h"

#define PORT 5000

DFRobot_Audio audio;
Webcam camera;

const char* ssid     = "hitest";
const char* password = "12345678";

const int buttonPin = 4;
int flag = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(buttonPin, INPUT_PULLUP);
  
  Serial.begin(115200);
  while(!Serial)

  SD.begin();
  camera.begin(SD);
  audio.begin(SD);
  
  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
  }
  Serial.println("WiFi connected.");
  
  camera.setPicturePPIType(PPI_TYPE_QVGA_640_480);
  camera.setFramePerSeconds(EJPEG_TYPE_20FPS);
  camera.enableWebcam();
  
  Serial.print("TCPServer Waiting for client on ");
  Serial.print(WiFi.localIP());
  Serial.print(":");
  Serial.println(PORT);
  
  audio.setSpeakersVolume(60);
}

void loop() {
  // put your main code here, to run repeatedly:
 if(!digitalRead(buttonPin)&&flag==0){  //Press user key to take photo
      while((!digitalRead(buttonPin))){  //Eliminate shaking
          delay(10);
      }
      if(camera.snapshot("/photo1.jpg") < 0){
          Serial.println("take photo1 fail.");
      }else{
          Serial.println("take photo1.");
          audio.playMusic("kaca.mp3");
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
          audio.playMusic("kaca.mp3");
          flag=0;
      }
  }
}