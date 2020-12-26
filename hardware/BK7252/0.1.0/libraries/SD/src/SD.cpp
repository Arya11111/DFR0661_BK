#include "SD.h"
//同时只能打开15个文件
namespace SDLib {

boolean SDClass::begin() {
  bool result = false;
  if(getMountPath() == "/sd") return true;
  if(rt_hw_sdcard_init() != 0) return result;
  elm_init();
  if(dfs_mount("sd0", "/sd", "elm", 0, 0) == 0)
      result = true;
  return result;
}

const char * SDClass::getMountPath(){
  rt_device_t sd0 = rt_device_find("sd0");
  return dfs_filesystem_get_mounted_path(sd0);
}

//规则1.父级目录一定存在
File SDClass::open(const char *filepath, int mode){
  struct dfs_fd d;
  int result, state;
  if(mode == O_RDONLY){
      state = isDir(getFullpath(filepath).c_str());
      if(state == 1) mode = O_DIRECTORY|O_RDONLY;
      //Serial.print("state=");Serial.println(state);
  }
  result = dfs_file_open(&d, getFullpath(filepath).c_str(), mode);
  if(result < 0) {
      rt_set_errno(result);
      return File();
  }
  return File(d, getFullpath(filepath).c_str());
}


boolean SDClass::exists(const char *filepath) {
  if(isDir(getFullpath(filepath).c_str()) == -1) return false;
  else return true;
}

boolean SDClass::mkdir(const char *filepath) {
  int fd;
  struct dfs_fd *d;
  int result;
  fd = fd_new();
  if (fd == -1)
  {
      rt_set_errno(-ENOMEM);
      return false;
  }
  d = fd_get(fd);
  result = dfs_file_open(d, getFullpath(filepath).c_str(), O_DIRECTORY | O_CREAT);
  if (result < 0)
  {
      fd_put(d);
      fd_put(d);
      rt_set_errno(result);
      return false;
  }
  dfs_file_close(d);
  fd_put(d);
  fd_put(d);
  return true;
}

boolean SDClass::rmdir(const char *filepath) {
  int result;
  result = dfs_file_unlink(getFullpath(filepath).c_str());
  if (result < 0)
  {
      rt_set_errno(result);
      return false;
  }
  return true;
}

boolean SDClass::remove(const char *filepath) {
  int result;
  result = dfs_file_unlink(getFullpath(filepath).c_str());
  if (result < 0)
  {
      rt_set_errno(result);
      return false;
  }
  return true;
}


// allows you to recurse into a directory
File File::openNextFile(uint8_t mode) {
  struct dfs_fd d;
  struct dirent dirent;
  int result;

  if(_length > 0) goto LOOP1;
LOOP1:
  _length = dfs_file_getdents(&_file, &dirent, sizeof(struct dirent));
  //Serial.print(_length);Serial.println("+1");
  if(_length > 0){
      
      char *fullpath = dfs_normalize_path(_name, dirent.d_name);
      //Serial.print("isDir = ");Serial.println(isDir(fullpath));
      int state = isDir(fullpath);
      //Serial.print("state* = ");Serial.println(state);
      memset(&d, 0, sizeof(dfs_fd));
      if (state == 1)
      {
          //Serial.println("<DIR>");
          result = dfs_file_open(&d, fullpath, O_DIRECTORY);
          if(result < 0){
              rt_set_errno(result);
          }
          return File(d, fullpath);
      }else if(state == 0){
          result = dfs_file_open(&d, fullpath, O_RDONLY);
          //Serial.println("file size");
          if(result < 0){
              rt_set_errno(result);
          }
          return File(d, fullpath);
      }
  }
  return File();
}

void File::rewindDirectory(void) {
  DIR *d;
  if(isDirectory()) {
      d = opendir(_name);
      rewinddir(d);
  }
}
/*遍历目录,获取父级目录*/
int SDClass::getParentDir(const char *filepath, int *indx){
  uint32_t result;
  struct dfs_fd *d;
  String fullpath = getFullpath(filepath);
  *indx = fullpath.lastIndexOf(filepath);
  char parentdirname[*indx+1];
  strncpy(parentdirname, fullpath.c_str(), (*indx+1));
  parentdirname[*indx] = 0;
  int fd = fd_new();
  if (fd < 0) return -1;
  d = fd_get(fd);
  result = dfs_file_open(d, parentdirname, O_DIRECTORY);
  if(result < 0){
      fd_put(d);
      fd_put(d);
      rt_set_errno(result);
      return -1;
  }
  fd_put(d);
  dfs_file_close(d);
  return fd;
}

String getFullpath(const char *filepath){
  String fullpath = "";
  if(String(filepath) == "/"){
      //Serial.println("+++++++++++++++");
      fullpath = "/sd";
      return fullpath;
  }
  if(filepath[0] == '/') filepath++;
  if(strchr(filepath,'/')){
      uint8_t idx = strchr(filepath, '/') - filepath;
      if (idx > 12)
      idx = 12;    // dont let them specify long names
      char subdirname[13];
      strncpy(subdirname, filepath, idx);
      subdirname[idx] = 0;
      //Serial.println(subdirname);
      if(String(subdirname) != "sd") fullpath = "sd/";
  }else{
      //Serial.println(filepath);
      if(String(filepath) != "sd") fullpath = "sd/";
  }
  fullpath = "/" + fullpath + String(filepath);
  return fullpath;
}

int isDir(const char *path){
  int fd = 3, result, flag = -1;
  struct dfs_fd d;
  memset(&d, 0, sizeof(dfs_fd));
  result = dfs_file_open(&d, path, O_DIRECTORY);
  //Serial.print("fd = ");Serial.println(d.magic);
  //Serial.println(result);
  //Serial.print("type = ");Serial.println(d.type);
  if(result < 0){
      rt_set_errno(result);
      result = dfs_file_open(&d, path, O_RDONLY);
      //Serial.print("fd1 = ");Serial.println(d.magic);
      //Serial.println(result);
      if(result < 0){
          rt_set_errno(result);
          return -1;
      }
      //Serial.print("type1 = ");Serial.println(d.type);
      dfs_file_close(&d);
      //Serial.print("type2 = ");Serial.println(d.type);
      return 0;//File
  }
  //Serial.print("type = ");Serial.println(d.type);
  dfs_file_close(&d);
  //Serial.print("type22 = ");Serial.println(d.type);
  return 1;
}

SDClass SD;
};
