/*!
 * @file player.ino
 * @brief 串口音乐播放器，通过发送串口命令实现音乐的播放/停止，暂停/恢复，上一首/下一首
 * @n 音量设置,模式切换(单曲播放/单曲循环/列表播放/列表循环/随机播放)
 *
 * @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @licence     The MIT License (MIT)
 * @author [Arya](xue.peng@dfrobot.com)
 * @version  V1.0
 * @date  2020-09-17
 * @get from https://www.dfrobot.com
 * @url
 */
#include <SD.h>
#include "DFRobot_Audio.h"

DFRobot_Audio audio;
//打印进度条 0x4033A8----0x403381

bool flag = false, flagButton = false;
uint8_t curIndex = 0;
uint8_t maxNum = 0;
uint8_t musicMode = 0;

typedef struct optParse{
  char action;
  char *optArg;
}sOptParse_t;

sOptParse_t options;

void func(int event){
  flag = true;
  Serial.println("00000");
}

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
  audio.setPlayMode(REPEATONE);
  audio.attachEventInterrupt(PLAYER_PLAYBACK_STOP, func);
  audio.attachEventInterrupt(PLAYER_AUDIO_CLOSED, func);
  audio.attachEventInterrupt(PLAYER_PLAYBACK_BREAK, func);
  audio.attachEventInterrupt(PLAYER_PLAYBACK_FAILED, func);
  audio.setPlayListPath("/sd");
  audio.updateMusicList();
  audio.getMusicListIndexAndNum(&maxNum, &curIndex);
}


void loop(){  
  while(!Serial.available()){
      if(flagButton){
          flagButton = false;
          flag = false;
      }
      if(flag){
          flag = false;
          musicMode = audio.getPlayMode();
          switch(musicMode){
              case REPEATONE:
                   
                   break;
              case REPEATALL:
                   curIndex++;
                   if(curIndex == maxNum) curIndex = 0;
                   break;
              case SHUFFLE:
                   curIndex = random(maxNum);
                   break;
              case ORDERLIST:
                   if(curIndex < maxNum) curIndex++;
                   break;

          }
          if((musicMode != SINGLE) && (maxNum > 0) && (curIndex < maxNum)) {
              audio.playMusic(audio.getMusicNameFromMusicList(curIndex));
          }
      }
  }
  char *serialCmd = (char *)malloc(Serial.available() + 1);
  int len = 0;
  while(Serial.available()){
      char c = Serial.read();
      if(c == '\r' || c == '\n') break;
      serialCmd[len++] = c;
  }
  serialCmd[len] = 0;
  if(serialCmd != NULL){
      memset(&options, 0, sizeof(sOptParse_t));
      parseCmd(serialCmd, &options);
      switch(options.action){
          case 'h':
              usage();
              break;
          case 's':
              audio.playMusic(options.optArg);
              flagButton = true;
              break;
          case 't':
              audio.playerControl(audio.stop);
              break;
          case 'p':
              audio.playerControl(audio.pause);
              break;
          case 'r':
              audio.playerControl(audio.resume);
              break;
          case 'v':
              audio.setSpeakersVolume(atoi(options.optArg));
              Serial.println(atoi(options.optArg));
              break;
          case 'd':
              printMusicInformation();
              break;
          case 'm':
              audio.setPlayMode(atoi(options.optArg));
              break;
          case 'l':
              printPlaylist();
              break;
          case 'u':
              if(options.optArg != NULL) audio.setPlayListPath(options.optArg);
              audio.updateMusicList();
              Serial.println("Update completed!");
              audio.getMusicListIndexAndNum(&maxNum, &curIndex);
              break;
          case 'b':
              audio.playerControl(audio.previous);
              flagButton = true;
              audio.getMusicListIndexAndNum(&maxNum, &curIndex);
              break;
          case 'n':
              audio.playerControl(audio.next);
              flagButton = true;
              audio.getMusicListIndexAndNum(&maxNum, &curIndex);
              break;
      }
  }
  free(serialCmd);
}

void parseCmd(char *cmd, sOptParse_t *opt){
  String str,str1,str2,str3;
  while(*cmd == ' ') cmd++;
  str = String(cmd);
  if(str.startsWith("player") && opt != NULL){
      int index = str.indexOf("-");
      if(index == -1) return;
      cmd += (index+1);
      opt->action = cmd[0];
      cmd++;
      while(*cmd == ' ') cmd++;
      opt->optArg = cmd;
  }
}

/*串口命令*/
void usage(void){
  Serial.println("usage: player [option] [target] ...");
  Serial.println("usage options:"); 
  Serial.println("\t-V,     --version                  Print player version message."); 
  Serial.println("\t-h,     --help                     Print defined help message."); 
  Serial.println("\t-s,     --start=URI                Play music with URI(network links or local files).");
  Serial.println("\t-t,     --stop                     Stop playing music.");
  Serial.println("\t-p,     --pause                    Pause the music.");
  Serial.println("\t-r,     --resume                   Resume the music.");
  Serial.println("\t-b,     --before                   previos music.");
  Serial.println("\t-n,     --next                     next music.");
  Serial.println("\t-v,     --volume=lvl               Change the volume(0~99).");
  Serial.println("\t-d,     --dump                     Dump play relevant information.");
  Serial.println("\t-m,     --mode=mode                Set player mode,0-SINGLE, 1-REPEATONE, 2-REPEATALL 3-SHUFFLE, 4-ORDERLIST.");
  Serial.println("\t-l,     --l                        Get playlist.");
  Serial.println("\t-u,     --u=NULL or DIRURL         Update playlist.");
  Serial.println("format: player[space][option][space][target]");
  Serial.println("examples:");
  Serial.println("\tplayer -h");
  Serial.println("\tplayer -s /sd/music/test.mp3");
  Serial.println("\tplayer -t");
  Serial.println("\tplayer -p");
  Serial.println("\tplayer -r");
  Serial.println("\tplayer -v 80");
  Serial.println("\tplayer -d");
  Serial.println("\tplayer -m 0");
  Serial.println("\tplayer -l");
  Serial.println("\tplayer -u");
  Serial.println("\tplayer -u /sd");
}

void printMusicInformation(){
  const char *state[] ={
        "STOPPED",
        "PLAYING",
        "PAUSED" 
  };
  char timeStr[6];
  Serial.println("Dump status:");
  Serial.print("\tStatus: ");Serial.println(state[audio.getPlayerState()]);
  if(audio.getPlayerState() != STOPPED){
      Serial.print("\tUrl: ");Serial.println(audio.getMusicUrl());
      Serial.print("\tvolume: ");Serial.println(audio.getSpeakersVolume());
      audio.getMusicDuration(timeStr);
      Serial.print("\tDuration: ");Serial.println(timeStr);
      audio.getPlayPosition(timeStr);
      Serial.print("\tPosition: ");Serial.println(timeStr);
      }
  Serial.println();
}

void printPlaylist(){
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