/*!
 * @file setAndGetPlaylist.ino
 * @brief 设置并获取音乐列表
 * @n  串口打印播放列表里的歌曲路径url
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
  
  audio.setPlayListPath("/sd/music");
  audio.updateMusicList();
  
  uint8_t MusicNums, index;
  audio.getMusicListIndexAndNum(&MusicNums, &index);
  Serial.print("\nMusicList name: ");Serial.println(audio.getPlayListPath());
  Serial.print("Music numbers: ");Serial.println(MusicNums);
  Serial.print("play position: ");Serial.println(index);
  Serial.println("Music List:");
  for(uint8_t i = 0; i < MusicNums; i++){
      Serial.print('\t');
      if(i < 10) Serial.print(0);
      Serial.print(i);Serial.print(". ");
      Serial.println(audio.getMusicNameFromMusicList(i));
  }
}


void loop(){  
  
}