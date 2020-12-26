#include <SD.h>

File::File(void){
  _name[0] = 0;
  _length = 0;
  memset(&_file, 0, sizeof(struct dfs_fd));
}

File::File(dfs_fd fd, const char *name){
  _file = fd;
  strncpy(_name, name, 49);
  _name[49] = 0;
  _length = 0;
}

// returns a pointer to the file name
char *File::name(void) {
  return _name;
}

// a directory is a special type of file
//在没打开之前判断它是不是目录
boolean File::isDirectory(void) {
  if(_file.type == 2) return true;
  else return false;
}


size_t File::write(uint8_t val) {
  return write(&val, 1);
}

size_t File::write(const uint8_t *buf, size_t size){
  int result;
  if(_file.path == NULL) return 0;
  result = dfs_file_write(&_file, buf, size);
  if (result < 0)
  {
      return 0;
  }
  return result;
}

int File::peek() {
  int c = read();
  uint32_t pos = _file.pos;
  if(c != -1) dfs_file_lseek(&_file, pos);
  return true;
}

int File::read() {
  uint8_t b;
  return (read(&b, 1) == 1) ? b : -1;
}

int File::read(void *buf, uint16_t nbyte) {
  int result;
  if(_file.path == NULL) return -1;
  result = dfs_file_read(&_file, buf, nbyte);
  if (result < 0)
  {
      rt_set_errno(result);
      return -1;
  }
  return result;
}

int File::available() {
  uint32_t n = size() - position();
  return n > 0X7FFF ? 0X7FFF : n;
}

void File::flush() {
  dfs_file_flush(&_file);
}

boolean File::seek(uint32_t pos) {
  if(dfs_file_lseek(&_file, pos) < 0) return false;
  else return true;
}

uint32_t File::position() {
  uint32_t pos = _file.pos;
  return pos;
}

uint32_t File::size(){
  return _file.size;
}

void File::close() {
  int result = dfs_file_close(&_file);
  if(result >= 0) _file.flags &= ~(0x01000000);
}
/*只有/sd用fd_is_open是不能判定是否打开的*/
File::operator bool() {
  return (_file.flags & 0x01000000) ? true : false;
}

