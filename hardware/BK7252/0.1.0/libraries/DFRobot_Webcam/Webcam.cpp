#include "Webcam.h"

#define SEND_FRAME_EVENT        (1 << 0)
//#define SEND_FRAME_EVENT        (1 << 0)
#define SEND_FRAME_COMPLETE     (1 << 1)
#define CLIENT_CONNECT          (1 << 2)

static sMjpegSession_t _session;
static WiFiServer _webcamNetServer_(5000);
static rt_thread_t _webCamMjpegThreadTid_;
static rt_thread_t _webCamTCPServerThreadTid_;
static WiFiClient _webCamClient_;
static bool _netWebcamFlag_ = false;
static int _caPORT_ = 5000;


static char _boundaryData_[1024];

static const char resp_header[] = 
"HTTP/1.1 200 OK\r\n"
"Content-Type: multipart/x-mixed-replace; boundary=------Boundary-------\r\n\r\n";

static void webcamTcpServerThread(void* param){
  socklen_t sin_size;
  struct sockaddr_in server_addr, client_addr;
  int ret;
  
  if((_session.sock = lwip_socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
      rt_kprintf("socket error\n");
      goto __exit;
  }
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(_caPORT_); 
  server_addr.sin_addr.s_addr = INADDR_ANY;
  rt_memset(&(server_addr.sin_zero), 0, sizeof(server_addr.sin_zero));

  if(lwip_bind(_session.sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
  {
      rt_kprintf("unable to bind\n");
      goto __exit;
  }

  if(lwip_listen(_session.sock, 5) == -1)
  {
      rt_kprintf("listen error\n");
      goto __exit;
  }
  while(1)
  {
      sin_size = sizeof(struct sockaddr_in);
      _session.connected = lwip_accept(_session.sock, (struct sockaddr *)&client_addr, &sin_size);

      if(_session.connected < 0)
      {
          rt_kprintf("accept connection failed! errno = %d\n", errno);
          goto __exit;
      }

      rt_kprintf("I got a connection from (%s , %d)\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

      lwip_send(_session.connected, resp_header, strlen(resp_header), 0);

      //camera_start();
      rt_event_send(_session.event, CLIENT_CONNECT);
  }

__exit:
    if(_session.sock >= 0)
    {
        lwip_close(_session.sock);
        _session.sock = -1;
    }
    rt_kprintf("exit tcp server thread\n");
    return ;
  // while(1){
      // _webCamClient_ = _webcamNetServer_.available();
      // if(_webCamClient_){
          // _webCamClient_.write((uint8_t *)resp_header, strlen(resp_header));
          // rt_event_send(_session.event, CLIENT_CONNECT);
      // }
  // }
}

static void webcamMjpegThread(void* param){
  
  
  
  int ret;
  _webCamTCPServerThreadTid_ = rt_thread_create("webcamtcpsrv", webcamTcpServerThread, RT_NULL, 1024*4, 10, 10);
  rt_thread_startup(_webCamTCPServerThreadTid_);
  //rt_kprintf("++++++++++\n");
  rt_event_recv(_session.event, CLIENT_CONNECT, RT_EVENT_FLAG_OR|RT_EVENT_FLAG_CLEAR, RT_WAITING_FOREVER, RT_NULL);
  //rt_kprintf("=========\n");
  while(1){
      //rt_kprintf("@@@@@@@@@@@@@\n");
      tvideo_capture(1);
      rt_event_recv(_session.event, SEND_FRAME_EVENT, RT_EVENT_FLAG_OR|RT_EVENT_FLAG_CLEAR, RT_WAITING_FOREVER, RT_NULL);
      if(_session.connected >= 0){
          memset(_boundaryData_, 0, 1024);
          ret = rt_sprintf(_boundaryData_, "------Boundary-------\r\nContent-Type: image/jpeg\r\nContent-Length: %d\r\n\r\n", _session.total_len);
          ret = lwip_send(_session.connected, _boundaryData_, ret, 0);
          ret = lwip_send(_session.connected, _session.buf, _session.total_len, 0);
          if(ret <= 0) 
          {
              if(_session.connected >= 0) lwip_close(_session.connected);
              _session.connected = -1;
          }
          _session.old_tick = rt_tick_get();
          _session.total_len = 0;
      }
      //rt_kprintf("+++++++++++++++\n");
      /*if(_webCamClient_){
          memset(_boundaryData_, 0, 1024);
          ret = rt_sprintf(_boundaryData_, "------Boundary-------\r\nContent-Type: image/jpeg\r\nContent-Length: %d\r\n\r\n", _session.total_len);
          ret = _webCamClient_.write((uint8_t *)_boundaryData_, ret);
          ret = _webCamClient_.write(_session.buf, _session.total_len);
          //rt_kprintf("@@@@@@@@@@@@@%d, %d\n", ret, _session.total_len);
          if(ret <= 0){
              _webCamClient_.stop();
              //if(_webCamClient_) _webCamClient_.stop();
              //_session.old_tick = rt_tick_get();
              _session.total_len = 0;
          }
          _session.old_tick = rt_tick_get();
          _session.total_len = 0;
      }*/
      tvideo_capture(0);
  }
__exit:
    if(_session.buf != RT_NULL) rt_free(_session.buf);
 
}


static int videoCaptureCB(UINT8 *data, UINT32 pos, UINT32 len, UINT8 isStop){
  //rt_kprintf("isStop=%d, pos=%d, len=%d\n", isStop, pos, len);
  _session.total_len = 0;
  _session.buf = (char*)data;
  _session.total_len += len;
  rt_event_send(_session.event, SEND_FRAME_EVENT);
  return len;
}

static void pktHeaderCB(TV_HDR_PARAM_PTR param){
  HDR_PTR elem_tvhdr = (HDR_PTR)param->ptk_ptr;
  elem_tvhdr->id = (UINT8)param->frame_id;
  elem_tvhdr->is_eof = param->is_eof;
  elem_tvhdr->pkt_cnt = param->frame_len;
  elem_tvhdr->size = 0;
}

Webcam::Webcam()
  :_isStart(false),_sd(NULL),_ppi(PPI_TYPE_QVGA_320_240),_fps(EJPEG_TYPE_5FPS){
  memset(&_session, 0, sizeof(_session));
}

int Webcam::begin(SDClass &sd){
  _sd = &sd;
  if(_isStart) {
      rt_kprintf("camera is already start\n");
      return 0;
  }
  // if(_sd->begin()){
      // rt_kprintf("sd init failed!");
      // _sd = NULL;
      // return -1;
  // }
  //_sd = &sd;
  //创建摄像头接收一帧图片的事件
  _session.event = rt_event_create("vt_event", RT_IPC_FLAG_FIFO);
  _session.setup.send_type = TVIDEO_SND_INTF;
  _session.setup.send_func = videoCaptureCB;
  _session.setup.start_cb = NULL;
  _session.setup.end_cb = NULL;
  _session.setup.pkt_header_size = sizeof(HDR_ST);
  _session.setup.add_pkt_header = pktHeaderCB;
  video_transfer_init(&_session.setup);
  _isStart = true;
  return 0;
}

void Webcam::end(){
  video_transfer_deinit();
  _isStart = false;
}


//文件名格式不对，不是jpg图片 -1
//摄像头未启动 返回-2
//文件创建失败，返回-3

void Webcam::setPicturePPIType(uint8_t ppi){
  if(_isStart){
      _ppi = ppi;
      _fps = EJPEG_TYPE_5FPS;
      camera_intfer_set_video_param((UINT32)_ppi, (UINT32)_fps);
  }
  
}
void Webcam::setFramePerSeconds(uint8_t fps){
  if(_isStart){
      _fps = fps;
      camera_intfer_set_video_param((UINT32)_ppi, (UINT32)_fps);
  }
}

int Webcam::snapshot(const char *pictureFilename){
  String str = String(pictureFilename);
  str.toLowerCase();
  if(pictureFormatIsJPG(pictureFilename) != true) return -1; 
  if(_isStart){
      tvideo_capture(1);
      rt_event_recv(_session.event, SEND_FRAME_EVENT, RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR, RT_WAITING_FOREVER, RT_NULL);
      _sd->remove(pictureFilename);//不管有没有都移除
      File myFile = _sd->open(pictureFilename, FILE_WRITE);
      if(myFile){
          myFile.write(_session.buf, _session.total_len);
          myFile.close();
      }else return -3;
      tvideo_capture(0);
      if(_netWebcamFlag_) tvideo_capture(1);
      return 0;
  }else return -2;
}

void Webcam::getVideoData(char *buf, char *len){
  if(buf == NULL || len==NULL) return;
  if(_isStart){
      tvideo_capture(1);
      rt_event_recv(_session.event, SEND_FRAME_EVENT, RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR, RT_WAITING_FOREVER, RT_NULL);
      tvideo_capture(0);
      buf = _session.buf;
      *len = _session.total_len;
      tvideo_capture(0);
  }
}

bool Webcam::pictureFormatIsJPG(const char *pictureFilename){
  String str = String(pictureFilename);
  str.toLowerCase();
  if(str.endsWith(".jpg")) return true;
  else return false;
}

bool Webcam::enableWebcam(){
  //_webcamNetServer_.begin(5000);
  _netWebcamFlag_ = true;
  _webCamMjpegThreadTid_ = rt_thread_create("webcam_thread", webcamMjpegThread, RT_NULL, 1024*4, 10, 10);
  rt_thread_startup(_webCamMjpegThreadTid_);
  return true;
}

bool Webcam::disableWebcam(){
  _netWebcamFlag_ = false;
  rt_thread_delete(_webCamMjpegThreadTid_);
  rt_thread_delete(_webCamTCPServerThreadTid_);
  _session.connected = -1;
  return true;
}