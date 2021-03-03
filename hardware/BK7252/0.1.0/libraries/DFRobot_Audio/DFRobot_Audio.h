/*!
 * @file DFRobot_Audio.h
 * @brief Define the basic structure of class DFRobot_Audio 
 * @n 这是一个集录放和播放于一体的音乐播放库
 *
 * @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @licence     The MIT License (MIT)
 * @author [Arya](xue.peng@dfrobot.com)
 * @version  V1.0
 * @date  2019-07-30
 */
#ifndef __DFROBOT_AUDIO_H
#define __DFROBOT_AUDIO_H
#include <Arduino.h>
#include <SD.h>
#include <SPI.h>
extern "C" {
#include "audio_device.h"
#include "player.h"
#include <dfs_fs.h>
#include "codec_helixmp3.h"
#include "format_m4a.h"
#include "codec_helixmp3.h"
#include "codec_opencore_amr.h"
#include "player_system.h"
#include "player_init.h"
#include "audio_pub.h"
#include "audio_device.h"
#include "vad.h"
//#include "DFRobot_queue.h"
}
//#include "USB/USBAPI.h"

#define RECORD_AUDIO_FILE_MAX  0xffffffff
#define RECORD_SAMPLERATE_8000_HZ   8000  //Hz
#define RECORD_SAMPLERATE_11025_HZ  11025 //Hz
#define RECORD_SAMPLERATE_12000_HZ  12000 //Hz
#define RECORD_SAMPLERATE_16000_HZ  16000 //Hz
#define RECORD_SAMPLERATE_22050_HZ  22050 //Hz
#define RECORD_SAMPLERATE_24000_HZ  24000 //Hz
#define RECORD_SAMPLERATE_32000_HZ  32000 //Hz
#define RECORD_SAMPLERATE_44100_HZ  44100 //Hz
#define RECORD_SAMPLERATE_48000_HZ  48000 //Hz


#define RECORD_BEGIN   1
#define RECORD_STOP   0


#define STOPPED       0
#define PLAYING       1
#define PAUSED        2

#define SINGLE       0
#define REPEATONE    1
#define REPEATALL    2
#define SHUFFLE      3
#define ORDERLIST    4

#define MUSICLISTMAX  50

#define RECORD_CODE_SUCESS     0
#define RECORD_CODE_ERR_NAME   1
#define RECORD_CODE_THREADENABLE_FAILED   2
#define RECORD_CODE_DISK_FULL   3
#define RECORD_CODE_ERR_SAMPLERATE   4


typedef void(*DFRobot_Audio_INT_CB)(int event);
typedef struct playerCBConfig{
  uint8_t mode;
  String musicPath[MUSICLISTMAX];
  uint8_t index;
  uint8_t num;
  DFRobot_Audio_INT_CB cb[9];
}playerCBConfig_t;

typedef struct recordManager{
  int action;
  int sampleRate;
  int channel;
  String name;
}recordManager_t;

void playerEventFunction(int event, void *user_data);

extern recordManager_t recorderManger;
extern uint8_t recorderMangerFlag;

typedef struct
{
  char      riffType[4];//4字节 'RIFF'标志
  uint32_t  riffSize;  //4字节
  char      waveType[4];//4字节 'WAVE'标志
  char      formatType[4];//4字节 'fmt'标志
  uint32_t  formatSize; //4字节，过渡字节(不定) 16,0,0,0
  uint16_t  compressionCode;//2字节，格式类别 0x01,0x00
  uint16_t  numChannels;//2字节，声道数
  uint32_t  sampleRate;//4字节，采样率
  uint32_t  bytesPerSecond;//4字节，位速
  uint16_t  blockAlign;//2字节，一个采样多声道数据块大小
  uint16_t  bitsPerSample;//2字节，一个采样占的位数
  char      dataType1[4];//4字节，数据标记符"data"
  uint32_t  dataSize;//4bytes,语音数据的长度，比文件长度小42字节（一般）
  //44
  //char      test[800];
}sWavHeader_t, *pWavHeader_t;



class DFRobot_Audio{
public:
  typedef enum playerCmd{
      play,//播放,从头开始播放
      stop,//停止播放
      pause,//暂停播放
      resume,//从暂停中恢复播放
      previous,//上一首
      next //下一首
  }ePlayerCmd_t, *pPlayerCmd_t;
  
  typedef enum playerMode{
      single,//单曲播放
      repeatOne,//单曲循环
      repeatAll,//循环播放
      shuffle,//随机播放
      orderList,//顺序播放
  }ePlayerMode_t, *pPlayerMode_t;

  DFRobot_Audio();
  ~DFRobot_Audio();
  bool begin(SDClass &sd = SD);
  void initPlayer();
  /**
   * @brief 设置播放列表路径
   * @param dirpath 音乐文件名
   */
  void setPlayListPath(const char *dirpath);
  
  /**
   * @brief 获取播放列表路径
   * @return 返回播放列表的名字
   */
  const char* getPlayListPath();
  
  /**
   * @brief 更新音乐列表，该操作会清空已有的音乐播放列表
   */
  bool updateMusicList();
  /**
   * @brief 获取音乐列表当前播放的歌曲所在的顺序和列表中的音乐的数目
   * @param num 音乐列表中所有音乐的数目
   * @param index 选中音乐所在音乐列表的位置
   */
  void getMusicListIndexAndNum(uint8_t *num, uint8_t *index = NULL);
  /**
   * @brief 获取音乐播放列表中index所在的位置的音乐名字
   * @param index 音乐列表中某项音乐的位置
   */
  const char *getMusicNameFromMusicList(int index);
  /**
   * @brief 设置音乐播放模式
   * @param mode 播放模式 ePlayerMode_t
   */
  void setPlayMode(uint8_t mode = SINGLE);
  uint8_t getPlayMode();
  /**
   * @brief 播放音乐
   * @param Filename 音乐文件名
   */
  void playMusic(const char *Filename);
  /**
   * @brief 获取正在播放的音乐的名字路径
   * @return 返回正在播放的音乐的名字路径，没有播放的音乐则返回NULL
   */
  const char * getMusicUrl();
  /**
   * @brief 获取当前播放音乐的时间
   * @param t 以00:00的方式显示歌曲时间
   * @return 返回音乐播放的时间，单位s
   */
  int getMusicDuration(char *t = NULL);
  
  /**
   * @brief 获取当前音乐播放位置
   * @param t 以00:00的方式显示当前歌曲播放到那个位置
   * @return 返回当前播放歌曲的播放位置
   */
  int getPlayPosition(char *t = NULL);
  
  /**
   * @brief 设置扬声器音量
   * @param volume 音量，范围0~99
   */
  void setSpeakersVolume(uint8_t volume);
  /**
   * @brief 获取扬声器音量
   * @return 返回扬声器音量
   */
  uint8_t getSpeakersVolume();
  
  /**
   * @brief 播放音乐控制
   * @param cmd 控制命令
   * @param second 录音多少秒
   */
  void playerControl(ePlayerCmd_t cmd);
  /**
   * @brief 设置音乐播放进度
   * @param second 从第几秒开始播放，单位秒
   */
  void setPlaybackProgress(uint32_t second);
  
  /**
   * @brief 获取播放器状态
   * @return 返回播放器的状态，0-STOPPED，1-PLAYING，2-PAUSED
   */
  uint8_t getPlayerState();
  
  void attachEventInterrupt(int event, DFRobot_Audio_INT_CB cb);
  void detachEventInterrupt(int event);
  
  void initRecorder(uint32_t sampleRate = 8000);
  /**
   * @brief 录音，并保存文件
   * @param Filename 录音文件名,是wav格式的音乐文件名
   */
  void record(const char *Filename);
  /**
   * @brief 录音控制
   * @param cmd 控制命令
   */
  uint8_t recorderControl(uint8_t cmd);
  
  
protected:
  
  
private:
  String _musicPath;
  uint8_t _playMode;
  uint8_t _recordCode;//
  rt_thread_t _tidRecorder;
  SDClass *_sd;
  char *_musicDir;
  playerCBConfig_t _cbConfig;

};










#endif