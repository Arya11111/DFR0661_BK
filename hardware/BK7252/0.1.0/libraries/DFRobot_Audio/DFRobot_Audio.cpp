#include "DFRobot_Audio.h"
#include "USB/USBCore.h"
///* 
static sWavHeader_t defaultWaveHeader = {
  'R','I','F','F',
  0,
  'w','a','v','e',
 'f','m','t',' ',
  16,
  1,
  1,
  0,
  0,
  2,
  16,
  'd','a','t','a',
  0
};//*/


recordManager_t recorderManger;
uint8_t recorderMangerFlag = 0;
static rt_thread_t musicPlayTid;
static rt_thread_t recordThread;
static uint8_t audioPauseFlag = 0;


void playerMusic_thread_entry(void *param){
  while(1){
      if(USB_UD_ENABLE_FLAG == USB_UD_TEMPOARY_FLAG){
          if((audioPauseFlag == 0) && player_get_state() == 2){
              player_play();
          }
      }else{
          USB_UD_ENABLE_FLAG = USB_UD_TEMPOARY_FLAG;
          if(player_get_state() == 1) player_pause();
      }
      
      rt_thread_delay(2000);
  }
  
}

void playerEventFunction(int event, void *user_data){
  if(event == PLAYER_AUDIO_PLAYBACK){
      audioPauseFlag = 0;
      musicPlayTid = rt_thread_create("musicplay",playerMusic_thread_entry,RT_NULL,512,10,5);
      rt_thread_startup(musicPlayTid);
  }else if(event == PLAYER_PLAYBACK_STOP){
      rt_thread_delete(musicPlayTid);
  }
  playerCBConfig_t *intCB = (playerCBConfig_t *)user_data;
  if(intCB->cb[event] != NULL) (intCB->cb[event])(event);
}

#define RECORD_BUF_SIZE 1024*60
uint16_t _recordBuf_[1024*30];
uint8_t recordFlag = 0;
static void record_thread_entry(void *param){
  uint32_t actLen = 0,micReadLen = 0;
  int flag = 0;
  File _myFile;
  sWavHeader_t wavHead;
  
  while(1){
      if(recorderManger.action == 1){
          if(!flag){
              flag = 1;
              if(SD.exists(recorderManger.name.c_str())){
                  SD.remove(recorderManger.name.c_str());
              }
              _myFile = SD.open(recorderManger.name.c_str(),FILE_WRITE);
              
              memcpy(&wavHead, &defaultWaveHeader,sizeof(sWavHeader_t));
              wavHead.sampleRate = recorderManger.sampleRate;
              wavHead.bytesPerSecond = (wavHead.sampleRate * 16)/8;
              wavHead.blockAlign = (16 * 1)/8;
              wavHead.bitsPerSample = 16;
              _myFile.write((uint8_t *)&wavHead, sizeof(defaultWaveHeader));
              Serial.println("ok");
              audio_device_mic_set_channel(recorderManger.channel);       /*设置adc通道*/
              audio_device_mic_set_rate(recorderManger.sampleRate);       /*设置adc采样率*/
              audio_device_mic_open();
              recordFlag = 1;
          }
          if(micReadLen > RECORD_BUF_SIZE - 1024){
              _myFile.write((uint8_t *)_recordBuf_, micReadLen);
              micReadLen = 0;
          }
          actLen = audio_device_mic_read(((uint8_t *)_recordBuf_)+micReadLen,1024);
          micReadLen += actLen;
      }else if(recorderManger.action == 2){
          recorderManger.action = 0;
          if(micReadLen){
              _myFile.write((uint8_t *)_recordBuf_, micReadLen);
          }
          micReadLen = 0;
          if(flag){
              
              audio_device_mic_close();
              wavHead.riffSize = _myFile.size() - 8;
              wavHead.dataSize = wavHead.riffSize - 36;
              _myFile.seek(0);
              _myFile.write((uint8_t *)&wavHead, sizeof(sWavHeader_t));
              _myFile.close();
              flag = 0;
          }
          recordFlag = 0;
          Serial.println("recorderManger.action == 2");
          rt_thread_delete(recordThread);
      }
      //rt_thread_mdelay(5);
  }
}

DFRobot_Audio::DFRobot_Audio(){
  memset(&recorderManger, 0, sizeof(recorderManger));
  _recordCode = RECORD_CODE_ERR_NAME;
}

DFRobot_Audio::~DFRobot_Audio(){
  if(_musicDir != NULL) free(_musicDir);
}

void DFRobot_Audio::initPlayer(){
  rt_audio_codec_hw_init();
  player_codec_helixmp3_register();
  player_codec_beken_aac_register();
  player_codec_beken_m4a_register();
  player_codec_opencore_amr_register();
  player_system_init();
}

bool DFRobot_Audio::begin(SDClass &sd){
  _sd = &sd;
  //if(!SD.begin()) return false;
  //initPlayer();
  player_codec_helixmp3_register();
  player_codec_beken_aac_register();
  player_codec_beken_m4a_register();
  player_codec_opencore_amr_register();
  player_system_init();
  audio_device_init();
  /*初始化 sound mic设备*/
  //audio_device_mic_open();
  
  //audio_device_mic_set_rate(16000);
  audio_device_mic_open();
  //audio_device_open();
  //audio_device_set_rate(16000);
  player_set_event_callback(playerEventFunction, &_cbConfig);
  return true;
}

void DFRobot_Audio::setPlayListPath(const char *dirpath){
  if(!(_sd->exists(dirpath))){
      if(!_sd->mkdir(dirpath)) return;
  }
  String fullpath = getFullpath(dirpath);
  if(bkIsDir(fullpath.c_str()) != 1) return;
  if(_musicDir != NULL) free(_musicDir);
  if((_musicDir = (char *)malloc(fullpath.length() + 1)) == NULL) return;
  strncpy(_musicDir, fullpath.c_str(), fullpath.length());
  _musicDir[fullpath.length()] = 0;
}

const char* DFRobot_Audio::getPlayListPath(){
  if(_musicDir != NULL) return _musicDir;
  else return NULL;
}

bool DFRobot_Audio::updateMusicList(){
  if(_musicDir == NULL){
      _cbConfig.num = 0;
      _cbConfig.index = 0;
      return false;
  }
  _cbConfig.num = 0;
  _cbConfig.index = 0;
  File root = _sd->open(_musicDir);
  if(!root) return false;
  while(true){
      File entry = root.openNextFile();
      if(!entry) break;
      if(!entry.isDirectory()){
          if(entry.size() == 0) continue;
          String str =String(entry.name()) ;//(String(entry.name())).toLowerCase();
          str.toLowerCase();
          if(str.endsWith(".mp3")||str.endsWith(".wav")||str.endsWith(".pcm")){
              if(_cbConfig.num > MUSICLISTMAX) {
                  entry.close();
                  return true;
              }
              _cbConfig.musicPath[_cbConfig.num++] = String(entry.name());
          }
      }
      entry.close();
  }
  root.close();
  if(_cbConfig.num != 0) return true;
  else return false;
}

void DFRobot_Audio::getMusicListIndexAndNum(uint8_t *num, uint8_t *index){
  if(index != NULL) *index = _cbConfig.index;
  if(num != NULL) *num = _cbConfig.num;
}

const char *DFRobot_Audio::getMusicNameFromMusicList(int index){
  if(index < _cbConfig.num) return (char *)(_cbConfig.musicPath[index]).c_str();
  else return NULL;
}

void DFRobot_Audio::setPlayMode(uint8_t mode){
  _playMode = mode;
  //if(mode == single) return;
  _cbConfig.mode = mode;
}

uint8_t DFRobot_Audio::getPlayMode(){
  return _playMode;
}

void DFRobot_Audio::playMusic(const char *Filename){
  _musicPath = getFullpath(Filename);
  if(getPlayerState() != STOPPED)
      player_stop(); 
  player_set_uri(_musicPath.c_str());
  player_play();
}

void DFRobot_Audio::setSpeakersVolume(uint8_t volume){
  player_set_volume(volume);
}

void DFRobot_Audio::playerControl(ePlayerCmd_t cmd){
  switch(cmd){
      case play: 
        if(_cbConfig.num > 0){
            playMusic(_cbConfig.musicPath[_cbConfig.index].c_str());
        }
        break;
      case stop:
        player_stop();
        break;
      case pause:
        audioPauseFlag = 1;
        player_pause();
        break;
      case resume:
        player_play();
        audioPauseFlag = 0;
        break;
      case previous:
        if(_cbConfig.num > 0){
            if(_cbConfig.index == 0) _cbConfig.index = _cbConfig.num -1;
            else _cbConfig.index -= 1;
            playMusic(_cbConfig.musicPath[_cbConfig.index].c_str());
        }
        break;
      case next:
        if(_cbConfig.num > 0){
            _cbConfig.index++;
            if(_cbConfig.index >= _cbConfig.num) _cbConfig.index = 0;
            playMusic(_cbConfig.musicPath[_cbConfig.index].c_str());
        }
        break;
  }
}

void DFRobot_Audio::setPlaybackProgress(uint32_t second){
  player_do_seek(second);
}
uint8_t DFRobot_Audio::getPlayerState(){
  return player_get_state();
}
uint8_t DFRobot_Audio::getSpeakersVolume(){
  return player_get_volume();
}
int DFRobot_Audio::getPlayPosition(char *t){
  int value;
  String str;
  if(!player_get_state()){
      if(t != NULL){
          str = "00:00";
          memcpy(t, str.c_str() ,5);
          t[5] = 0;
      }
      return 0;
  }
  value = player_get_position() / 1000;
  if(t != NULL){
      if(value/60 < 9) str = "0";
      str += value/60;
      str += ':';
      if(value%60 < 9) str += "0";
      str += value%60;
      memcpy(t, str.c_str() ,5);
      t[5] = 0;
  }
  return value;
}

int DFRobot_Audio::getMusicDuration(char *t){
  int value;
  String str;
  if(!player_get_state()){
      if(t != NULL){
          str = "00:00";
          memcpy(t, str.c_str() ,5);
          t[5] = 0;
      }
      return 0;
  }
  value = player_get_duration();
  if(t != NULL){
      if(value/60 < 9) str = "0";
      str += value/60;
      str += ':';
      if(value%60 < 9) str += "0";
      str += value%60;
      memcpy(t, str.c_str() ,5);
      t[5] = 0;
  }
  return value;
}

const char * DFRobot_Audio::getMusicUrl(){
  return (player_get_uri() != NULL) ? player_get_uri() : NULL;
}

void DFRobot_Audio::attachEventInterrupt(int event, DFRobot_Audio_INT_CB cb){
  if(event < 0 || event > 8) return;
  _cbConfig.cb[event] = cb;
}

void DFRobot_Audio::detachEventInterrupt(int event){
   if(event < 0 || event > 8) return;
   _cbConfig.cb[event] = NULL;
}
void DFRobot_Audio::initRecorder(uint32_t sampleRate){
  memset(&recorderManger, 0, sizeof(recordManager_t));
  recorderManger.sampleRate = sampleRate;
  recorderManger.channel = 1;
}
/*对文件进行过滤，只支持录音为WAV的文件*/
void DFRobot_Audio::record(const char *Filename){
  String str = String(Filename);
  str.toLowerCase();
  if(str.endsWith(".wav")){
      recorderManger.name = getFullpath(Filename);
      _recordCode = RECORD_CODE_SUCESS;
  }else{
      _recordCode = RECORD_CODE_ERR_NAME;
  }
}

uint8_t DFRobot_Audio::recorderControl(uint8_t cmd){
  if(_recordCode != RECORD_CODE_ERR_NAME){
      switch(cmd){
          case RECORD_BEGIN:
             recorderManger.action = RECORD_BEGIN;
             recordThread = rt_thread_create("recorder",record_thread_entry,NULL,1024*12,10,10);
             if(recordThread){
                 rt_thread_startup(recordThread);
             }else{
                _recordCode = RECORD_CODE_THREADENABLE_FAILED;
             }
             break;
          case RECORD_STOP:
             if(_recordCode != RECORD_CODE_THREADENABLE_FAILED){
                 recorderManger.action = 2;
             }
             break;
      }
  }
  return _recordCode;
}
