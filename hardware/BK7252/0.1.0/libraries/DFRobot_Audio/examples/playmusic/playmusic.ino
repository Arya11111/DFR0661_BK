/*!
 * @file playmusic.ino
 * @brief Play the music named test.wav,test.mp3,test.pcm
 * @n  运行此demo之前, 需在SD卡的中存放名为test.wav,test.mp3,test.pcm 3首歌曲。
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

DFRobot_Audio audio;
//打印进度条 0x4033A8----0x403381
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
  
  Serial.println("Set Speakers Volume from 0 to 99: 80");
  audio.setSpeakersVolume(40);
  
  Serial.println("Play music: test.wav");
  audio.playMusic("test.mp3");
  printMusicInformation();
  
  Serial.println("Play music: test.mp3");
  audio.playMusic("test.wav");
  printMusicInformation();
  
  Serial.println("Play music: test.pcm");
  audio.playMusic("test.pcm");
  printMusicInformation();
}


void loop(){  
  
}

void printMusicInformation(){
  const char *state[] ={
        "STOPPED",
        "PLAYING",
        "PAUSED" 
  };
  int dur,intel,intel2, i = 0;
  int base = 0, base1 = 0;
  char timeStr[6];
  Serial.println("Dump status:");
  Serial.print("\tStatus: ");Serial.println(state[audio.getPlayerState()]);
  if(audio.getPlayerState() != STOPPED){
      Serial.print("\tUrl: ");Serial.println(audio.getMusicUrl());
      Serial.print("\tvolume: ");Serial.println(audio.getSpeakersVolume());
      dur = audio.getMusicDuration(timeStr);
      intel = dur/100 + (dur%100 ? 1: 0);
      intel2 = dur%100;
      Serial.print("\tDuration: ");Serial.println(timeStr);
      Serial.print("\tPosition: ");
      while(audio.getPlayerState() != STOPPED){
          if((base + (i-base1)*intel) <= audio.getPlayPosition()){
              Serial.write(0x4033A8);Serial.write(0x403381);
              if(i == intel2){
                  //i = 0;
                  base1 = intel2;
                  base = intel2 * intel;
                  intel -= (dur%100 ? 1: 0);
              }
              i++;
          }
      }
      Serial.print(i);
      for(;i < 100; i++){
          Serial.write(0x4033A8);Serial.write(0x403381);
      }
  }
  Serial.println("\n");
}