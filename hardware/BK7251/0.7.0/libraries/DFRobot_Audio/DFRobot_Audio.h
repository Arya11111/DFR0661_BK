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
#define RECORD_MSG_DATA 0
#define RECORD_MSG_CMD 1
#define RECORD_SAVE_BUF_SIZE (320 * 20)

typedef void(*DFRobot_Audio_INT_CB)(int event);
typedef struct playerCBConfig{
  uint8_t mode;
  String musicPath[MUSICLISTMAX];
  uint8_t index;
  uint8_t num;
  DFRobot_Audio_INT_CB cb[9];
}playerCBConfig_t;


typedef struct recordMsg
{
    uint32_t type;
    uint32_t arg;
    uint32_t len;
} recordMsg_t;

typedef struct recordManager{
  //rt_mq_t msg;
  int action;
 // struct rt_mempool mp;
  int sampleRate;
  int channel;
  String name;
  //int mpBlockSize;
  //int mpCnt;
  //char *saveBuf;
  //int saveLen;
}recordManager_t;



typedef struct record{
  File entry;
  uint8_t state;
  String name;
  uint32_t time;
}sRecord_t, *pRecord_t;

void playerEventFunction(int event, void *user_data);

extern recordManager_t recorderManger;
extern uint8_t recorderMangerFlag;


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
  
  void initRecorder(uint32_t sampleRate = 16000, int channel = 1);
  /**
   * @brief 录音，并保存文件
   * @param Filename 录音文件名
   */
  void record(const char *Filename);
  /**
   * @brief 录音控制
   * @param cmd 控制命令
   */
  void recorderControl(uint8_t cmd);
  
  
protected:
  //int micRead(void *buf, int size);
  
  
private:
  String _musicPath;
  uint8_t _playMode;
  uint32_t _recordLen;
  sRecord_t _recorder;
  rt_thread_t _tidRecorder;
  File _musicDirRoot;
  SDClass *_sd;
  char *_musicDir;
  playerCBConfig_t _cbConfig;
  char **_musicFullPath;
};










#endif