#ifndef __DFROBOT_CAMERA_H
#define __DFROBOT_CAMERA_H
extern "C"{
#include "mjpeg.h"
}


/*
#include "Arduino.h"
#include <SD.h>
#include <WiFi.h>

struct mjpeg_session
{
  int sock;
  int connected;
  char* buf;
  int total_len;
  int last_frame_id;
  int recv_frame_flag;
  rt_tick_t old_tick;
  rt_event_t event;
  TVIDEO_SETUP_DESC_ST setup;
};

extern struct mjpeg_session session;

#define PIC_BUF_SIZE            (1024 * 50)
#define SEND_FRAME_EVENT        (1 << 0)
#define SEND_FRAME_COMPLETE     (1 << 1)
#define CLIENT_CONNECT          (1 << 2)
#define PORT                    (5000)

class DFRobot_Camera{
public:
  DFRobot_Camera();
  ~DFRobot_Camera();
  void begin();
  void camera_start(void);
  
protected:
  void hexdump(const rt_uint8_t *p, rt_size_t len);
  void get_ip(char* buf);
  void tcpserv(void *parameter);
private:
  
};

extern DFRobot_Camera Camera;
*/
#endif