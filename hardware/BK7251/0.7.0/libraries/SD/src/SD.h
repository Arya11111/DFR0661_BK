/*

 SD - a slightly more friendly wrapper for sdfatlib

 This library aims to expose a subset of SD card functionality
 in the form of a higher level "wrapper" object.

 License: GNU General Public License V3
          (Because sdfatlib is licensed with this.)

 (C) Copyright 2010 SparkFun Electronics

 */

#ifndef __SD_H__
#define __SD_H__

#include <Arduino.h>

#ifndef O_DIRECTORY
#define O_DIRECTORY              0x0200000
#endif

extern "C" {
#include "drv_sdio_sd.h"
#include <dfs_elm.h>
#include <dfs_fs.h>
//#include <dfs_file.h>
#include <dfs_posix.h>
#include <dfs.h>
//#include <dfs_def.h>
}
#define FILE_READ O_RDONLY
#define FILE_WRITE   O_RDWR|O_CREAT

namespace SDLib {
/**
 * @brief 判断文件是文件还是目录
 * @param path  文件绝对路径
 * @return 是文件返回0，是目录返回1,既不是文件也不是目录即不存在返回-1
 */
int isDir(const char *path);
/**
 * @brief 补全文件或文件夹的绝对路径，如果没有/sd根目录会自动添加
 * @param filepath  文件
 * @return 返回带有/sd的文件路径
 */
String getFullpath(const char *filepath);

class File : public Stream {
private:
  char _name[50];
  int _length;
  struct dfs_fd _file;

protected:
  

public:
  File(void);      // 'empty' constructor
  File(dfs_fd fd, const char *name);
  virtual size_t write(uint8_t);
  virtual size_t write(const uint8_t *buf, size_t size);
  virtual int read();
  virtual int peek();
  virtual int available();
  virtual void flush();
  int read(void *buf, uint16_t nbyte);
  boolean seek(uint32_t pos);
  uint32_t position();
  uint32_t size();
  void close();
  operator bool();
  char * name();
  //0-file  1-dir -1 not file not dir
  int type(int fileType);

  boolean isDirectory(void);
  File openNextFile(uint8_t mode = O_RDONLY);
  void rewindDirectory(void);
  
  using Print::write;
};

class SDClass {

private:
  int root;
  /*获取父级目录*/
  /**
   * @brief 获取父级目录
   * @param filepath 文件目录
   * @param indx 父级目录的长度
   * @return 返回父级目录的文件描述符fd
   */
  int getParentDir(const char *filepath, int *indx);
  /**
   * @brief 获取挂载路径
   * @return 返回挂载路径的字符串
   */
  const char * getMountPath();
public:
  /**
   * @brief Check whether the disk is mounted successfully.
   * @return false mounted failed, true mounted sucess.
   */
  boolean begin();
  /**
   * @brief Open file.
   * @param filename  file name.
   * @param mode  File open mode.
   * @return File class object .
   */
  File open(const char *filename, int mode = O_RDONLY);
  File open(const String &filename, int mode = O_RDONLY) { return open( filename.c_str(), mode ); }

  /**
   * @brief 判断文件是否存在
   * @param filepath  文件具体路径
   * @return 存在返回true, 不存在返回false.
   */
  boolean exists(const char *filepath);
  boolean exists(const String &filepath) { return exists(filepath.c_str()); }
  /**
   * @brief 创建目录
   * @param filepath  目录绝对路径
   * @return 创建成功返回true, 失败返回false.
   */
  boolean mkdir(const char *filepath);
  boolean mkdir(const String &filepath) { return mkdir(filepath.c_str()); }
  
  /**
   * @brief 移除文件
   * @param filepath  文件所在路径
   * @return 移除成功返回true, 移除失败返回false.
   */
  boolean remove(const char *filepath);
  boolean remove(const String &filepath) { return remove(filepath.c_str()); }
  
  /**
   * @brief 修改文件名字
   * @param filepath  文件所在路径
   * @return 修改成功返回true, 修改失败返回false.
   */
  boolean rmdir(const char *filepath);
  boolean rmdir(const String &filepath) { return rmdir(filepath.c_str()); }
private:
  
  friend class File;
  //friend boolean callback_openPath(SdFile&, const char *, boolean, void *); 
};

extern SDClass SD;

};

using namespace SDLib;
typedef SDLib::File    SDFile;
typedef SDLib::SDClass SDFileSystemClass;
//#define SDFileSystem   SDLib::SD

#endif
