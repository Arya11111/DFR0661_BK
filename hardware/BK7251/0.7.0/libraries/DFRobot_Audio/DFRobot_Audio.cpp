#include "DFRobot_Audio.h"


recordManager_t recorderManger;
uint8_t recorderMangerFlag = 0;
void playerEventFunction(int event, void *user_data){
  //rt_kprintf("+++++++%d---%d\r\n", event, *((uint8_t *)user_data));
  playerCBConfig_t *intCB = (playerCBConfig_t *)user_data;
  if(intCB->cb[event] != NULL) (intCB->cb[event])(event);
}

static rt_err_t record_msg_send(recordManager_t *record, void *buf, int type, int len)
{
    rt_err_t ret = RT_EOK;
    struct recordMsg msg;

    msg.type = type;
    msg.arg = (uint32_t)buf;
    msg.len = len;

    //ret = rt_mq_send(record->msg, (void *)&msg, sizeof(struct recordMsg));
    if (ret != RT_EOK)
        rt_kprintf("[record]:send msg failed \n");
  return ret;
}
#define RECORD_BUF_SIZE 1024*60
uint8_t recordBuf[1024*60], recordIndex = 0;
uint8_t recordBuf1[1024*60],recordIndex1 = 0;
uint8_t recordFlag = 0;
static void record_thread_entry(void *param){
  //sRecord_t recorder = *((pRecord_t)param);
  //int actualLen = 0, actSize;
  //uint8_t flag = 0;
  //File entry;
  //memset(buf, 0, sizeof(buf));
  //uint8_t *recordBuf;
  uint32_t actLen = 0,micReadLen = 0;
  uint8_t *mempool;
  int flag = 0;
  FILE *fp;
  
  //recorderManger.mpBlockSize = recorderManger.sampleRate/50*2;
  //recorderManger.mpCnt = 20;
  //mempool = (uint8_t *)rt_malloc(recorderManger.mpBlockSize * recorderManger.mpCnt);
  //rt_mp_init(&(recorderManger.mp), "record_mp", mempool, recorderManger.mpBlockSize * recorderManger.mpCnt, recorderManger.mpBlockSize);
  //recorderManger.msg = rt_mq_create("net_msg", sizeof(struct recordMsg), 12, RT_IPC_FLAG_FIFO);
  while(1){
      if(recorderManger.action == 1){
          
          if(!flag){
              flag = 1;
			  //remove(recorderManger.name.c_str());
			  if(SD.exists(recorderManger.name.c_str())){
				  Serial.println("++++");
				  SD.remove(recorderManger.name.c_str());
			  }
			  // if(SD.exists(recorderManger.name.c_str())){
				  // Serial.println("-----");
			  // }
              fp = fopen(recorderManger.name.c_str(), "wb");
			  if(fp == NULL) Serial.println("ABCD");
			  Serial.println("ok");
              audio_device_mic_set_channel(recorderManger.channel);        /*设置adc通道*/
              audio_device_mic_set_rate(recorderManger.sampleRate);       /*设置adc采样率*/
              audio_device_mic_open();
              wb_vad_enter();
			  recordFlag = 1;
              //Serial.println("&&&&&&&&&&&&&&&&&");
          }
          //recordBuf = (uint8_t *)rt_mp_alloc(&(recorderManger.mp), RT_WAITING_NO);
          if(!recordBuf) {rt_thread_mdelay(20);
             rt_kprintf("NULL\r\n");
          }
          else{
			  
              int chunk_size = wb_vad_get_frame_len();
			  if(micReadLen > RECORD_BUF_SIZE - chunk_size){
				  Serial.println(micReadLen);
				  fwrite(recordBuf, 1, micReadLen, fp);
				  micReadLen = 0;
			  }
              actLen = audio_device_mic_read(recordBuf+micReadLen,chunk_size);
              if(wb_vad_entry((char *)(recordBuf), actLen)){
                  //record_msg_send(&recorderManger, recordBuf, RECORD_MSG_DATA, actLen);
                  //int remainLen = actLen, wLen;
                  //uint8_t *pbuf = recordBuf;
				  //fwrite(pbuf, 1, actLen, fp);
                  //while(remainLen){
                      //wLen = remainLen > 1024 ? 1024 : remainLen;
                      //fwrite(pbuf, 1, wLen, fp);
                      //remainLen -= wLen;
                      //pbuf += wLen;
                      //Serial.println("+");
                  //}
				  micReadLen += actLen;
              }
			  
              
              //rt_kprintf("&&&&&&&&&&&&&&&&&\r\n");
          }
      }else if(recorderManger.action == 2){
         
          recorderManger.action = 0;
		  if(micReadLen != 0){
			  fwrite(recordBuf, 1, micReadLen, fp);
		  }
          if(flag){
              wb_vad_deinit();
              
              audio_device_mic_close();
              fclose(fp);
			  flag = 0;
			  fp = NULL;
          }
		  recordFlag = 0;
		   Serial.println("recorderManger.action == 2");
          //record_msg_send(&recorderManger, 0, RECORD_MSG_CMD, 1);
      }
      //rt_kprintf("fun=%s %s %d %d\r\n", __func__,recorder.name.c_str(),recorder.state,recorder.entry);
      // if(recorder.state && recorder.entry){
            // rt_thread_delay(20);
           // int chunk_size = wb_vad_get_frame_len();
           
           // while(chunk_size){
               // memset(buf, 0, sizeof(buf));
               // actSize = (chunk_size > 512) ? 512 : chunk_size;
               // actualLen = audio_device_mic_read(buf,actSize);
               // recorder.entry.write(buf, actualLen);
               // chunk_size -= actSize;
               //Serial.println(actualLen);
               //Serial.println(actSize);
           // }
      //}
  }
 // rt_free(mempool);
}

// static void record_pcm_entry(void *param){
  // struct recordMsg msg;
  // int flag = 0;
  // FILE *fp;
  // uint16_t *buf1;
  // int bufInsex = 0;
  // while(1){
      // Serial.println("++++++++++++++++");
      // if(rt_mq_recv(recorderManger.msg, &msg, sizeof(struct recordMsg), RT_WAITING_FOREVER) == RT_EOK){
          //Serial.println("1111111111111");
          // if(!flag){
              // flag = 1;
              // remove(recorderManger.name.c_str());
              // fp = fopen(recorderManger.name.c_str(), "wb");
              // fseek(fp, sizeof(char), 2);//指针移动到末尾
              // Serial.println("+++++++++++123");
              // Serial.println(recorderManger.name.c_str());
              // audio_device_open();
              // audio_device_set_rate(8000);
          // }
          // if(msg.type == RECORD_MSG_DATA){
              //memcpy(recorderManger.saveBuf + recorderManger.saveLen, (void *)msg.arg, msg.len);
              //recorderManger.saveLen += msg.len;
              //rt_mp_free((void *)msg.arg);
              //audio_device_write((uint8_t *)recorderManger.saveBuf, recorderManger.saveLen);
              //Serial.print("ac=");Serial.println(msg.len);
              //fwrite(recorderManger.saveBuf, 1, recorderManger.saveLen, fp);
             // if(recorderManger.saveLen >= RECORD_SAVE_BUF_SIZE - recorderManger.mpBlockSize)
             // {
                  /*send data*/
                  //Serial.println("0000000000000");
               //   bufInsex= 0;
                  //Serial.println(recorderManger.saveLen);
                  //fwrite(recorderManger.saveBuf, 1, recorderManger.saveLen, fp);
                  // int remainLen = recorderManger.saveLen, wLen;
                  // uint8_t *pbuf = recordBuf;
                  // while(remainLen){
                      // wLen = remainLen > 1024 ? 1024 : remainLen;
                      // fwrite(pbuf, 1, wLen, fp);
                      // remainLen -= wLen;
                      // pbuf += wLen;
                      // buf1 = (uint16_t *)audio_device_get_buffer(RT_NULL);
                      // if(bufInsex >= recorderManger.saveLen){
                          // audio_device_put_buffer(buf1);
                          // break;
                      // }
                      // memcpy(buf1,recorderManger.saveBuf+bufInsex,1024);
                      // bufInsex += 1024;
                      //audio_device_write((uint8_t *)buf1, 1024);
                      // Serial.println(bufInsex);
                      //Serial.println("+");
                      
                  //}
                 // audio_device_write((uint8_t *)recorderManger.saveBuf, recorderManger.saveLen);
                  //recorderManger.saveLen = 0;
              //}
          // }else if(msg.type == RECORD_MSG_CMD){
              // if(flag){
                  // flag = 0;
                  // fclose(fp);
                  // recorderMangerFlag = 0;
                  // Serial.println("file ok!");
              // }
              
          // }
      // }
  // }
// }

DFRobot_Audio::DFRobot_Audio(){
  _musicPath = "/sd/";
  memset(&_recorder, 0, sizeof(_recorder));
  _tidRecorder = NULL;
  memset(&recorderManger, 0, sizeof(_recorder));
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
  if(isDir(fullpath.c_str()) != 1) return;
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
  //_musicPath = "/sd";
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
        player_pause();
        break;
      case resume:
        player_play();
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
void DFRobot_Audio::initRecorder(uint32_t sampleRate, int channel){
  //audio_device_mic_open();
  //audio_device_mic_set_channel(channel);        /*设置adc通道*/
  //audio_device_mic_set_rate(sampleRate);       /*设置adc采样率*/
  //_recorder.state = RECORD_STOP;
  //_recorder.time = 0;
  //_recorder.name = "";
  //_tidRecorder = rt_thread_create("recorder",record_thread_entry,NULL,1024*60,27,10);
  //rt_thread_startup(_tidRecorder);
  Serial.println("initRecorder");
  memset(&recorderManger, 0, sizeof(recordManager_t));
  recorderManger.sampleRate = sampleRate;
  recorderManger.channel = channel;
  rt_thread_t tid1 = rt_thread_create("recorder",record_thread_entry,NULL,1024*12,10,10);
  if(tid1 == NULL){
      Serial.println("AAAAAAAAAAAAAA");
  }
  rt_thread_startup(tid1);
  // tid1 = rt_thread_create("record_pcm",record_pcm_entry,NULL,1024*32,10,10);
  // if(tid1 == NULL){
     // Serial.println("BBBBBBBBBBBB");
  // }
  // rt_thread_startup(tid1);
}

void DFRobot_Audio::record(const char *Filename){
  recorderManger.name = getFullpath(Filename);
}

void DFRobot_Audio::recorderControl(uint8_t cmd){
  switch(cmd){
      case RECORD_BEGIN:
         recorderManger.action = RECORD_BEGIN;
         // if(_sd->exists((_recorder.name).c_str()))
             // _sd->remove((_recorder.name).c_str());
         
         // _recorder.entry = _sd->open((_recorder.name).c_str(), FILE_WRITE|O_BINARY);
         // wb_vad_enter();
         // _recorder.state = cmd;
         break;
      case RECORD_STOP:
         recorderManger.action = 2;
		 audio_device_set_rate(8000);
		 audio_device_open();
         //recorderMangerFlag = 1;
         //while(recordFlag);
         // _recorder.state = cmd;
         // _recorder.entry.close();
         // wb_vad_deinit(); 
         // rt_kprintf("++++++++++++++++\r\n");
         break;
  }
  
}
