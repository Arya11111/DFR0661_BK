/*!
 * @file Webcam.h
 * @brief 这是一个摄像头库，集成了网络视频，本地拍照等功能
 * @n 网络摄像头
 *
 * @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @licence     The MIT License (MIT)
 * @author [Arya](xue.peng@dfrobot.com)
 * @version  V1.0
 * @date  2019-07-30
 */
#ifndef __WEBCAM_H
#define __WEBCAM_H
#include <Arduino.h>
#include <SD.h>
#include <WiFi.h>

extern "C" {
#include "video_transfer.h"
#include "camera_intf_pub.h"
#include "app_demo_softap.h"
}

#ifdef close
#undef close
#endif

#define PPI_TYPE_QVGA_320_240  0
#define PPI_TYPE_QVGA_640_480  1

#define EJPEG_TYPE_5FPS   0
#define EJPEG_TYPE_10FPS  1
#define EJPEG_TYPE_20FPS  2

typedef struct mjpeg_session
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
}sMjpegSession_t, *pMjpegSession_t;

class Webcam{
public:
  Webcam();
  /**
   * @brief 摄像头初始化，
   * @return 初始化成功返回0，否则返回-1
   */
  int begin(SDClass &sd);
  /**
   * @brief 摄像头注销
   */
  void end();
  /**
   * @brief 快照，快速照一张照片
   * @params pictureFilename, 照片文件名，后缀名为.jpg,目前支持.jpg格式的存储
   * @return 初始化成功返回0，否则返回-1
   */
  int snapshot(const char *pictureFilename);
  /**
   * @brief 设置图片的尺寸
   * @params ppi, 图片尺寸参数，可填PPI_TYPE_QVGA_320_240、PPI_TYPE_QVGA_640_480
   */
  void setPicturePPIType(uint8_t ppi);
  /**
   * @brief 设置图片帧传输率
   * @params fps, 帧传输率， 可使用EJPEG_TYPE_5FPS/EJPEG_TYPE_10FPS/EJPEG_TYPE_20FPS
   */
  void setFramePerSeconds(uint8_t fps);
  
  /**
   * @brief 启动网络摄像头
   */
  bool enableWebcam();
  /**
   * @brief 启动网络摄像头
   */
  bool disableWebcam();
  /**
   * @brief 获取一帧图片的数据
   * @params buf, 存放图像数据
   * @params len, 存放图像数据长度
   */
  void getVideoData(char *buf, char *len);
  
  
  
  
protected:
  bool pictureFormatIsJPG(const char *pictureFilename);
  
private:
  bool _isStart;
  uint8_t _ppi;
  uint8_t _fps;
  SDClass *_sd;
  
};










#endif