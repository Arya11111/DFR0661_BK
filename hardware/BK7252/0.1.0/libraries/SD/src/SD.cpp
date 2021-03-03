/*!
 * @file SD.cpp
 * @brief SD库是有DFRobot库在Arduino标准SD库的基础上修改而来，它完全兼容Arduino的SD库，并在它的
 * 基础上扩展了W25Q128型号的flash芯片。
 * @n 这个被修改后的SD库既支持SPI SD卡，又支持DFRobot M0板载的SPI W25Q128 flash芯片,12M U盘，其他内存被其他占用
 * @n M0板载的W25Q128 flash芯片的cs引脚为32
 * @n 使用这个库你既可以使用主控的SPI连接SD卡，又可以使用SPI连接W25Q128 flash芯片
 * @n in the form of a higher level "wrapper" object.
 * @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @licence GNU General Public License V3
          (Because sdfatlib is licensed with this.)
 */


#include "SD.h"

namespace SDLib {

// Used by `getNextPathComponent`
#define MAX_COMPONENT_LEN 12 // What is max length?
#define PATH_COMPONENT_BUFFER_LEN MAX_COMPONENT_LEN+1

bool getNextPathComponent(const char *path, unsigned int *p_offset,
              char *buffer) {
  /*

    Parse individual path components from a path.

      e.g. after repeated calls '/foo/bar/baz' will be split
           into 'foo', 'bar', 'baz'.

    This is similar to `strtok()` but copies the component into the
    supplied buffer rather than modifying the original string.


    `buffer` needs to be PATH_COMPONENT_BUFFER_LEN in size.

    `p_offset` needs to point to an integer of the offset at
    which the previous path component finished.

    Returns `true` if more components remain.

    Returns `false` if this is the last component.
      (This means path ended with 'foo' or 'foo/'.)

   */

  // TODO: Have buffer local to this function, so we know it's the
  //       correct length?

  int bufferOffset = 0;

  int offset = *p_offset;

  // Skip root or other separator
  if (path[offset] == '/') {
    offset++;
  }
  
  // Copy the next next path segment
  while (bufferOffset < MAX_COMPONENT_LEN
     && (path[offset] != '/')
     && (path[offset] != '\0')) {
    buffer[bufferOffset++] = path[offset++];
  }

  buffer[bufferOffset] = '\0';

  // Skip trailing separator so we can determine if this
  // is the last component in the path or not.
  if (path[offset] == '/') {
    offset++;
  }

  *p_offset = offset;

  return (path[offset] != '\0');
}



boolean walkPath(const char *filepath, SdFile& parentDir,
         boolean (*callback)(SdFile& parentDir,
                     const char *filePathComponent,
                     boolean isLastComponent,
                     void *object),
         void *object = NULL) {
  /*
     
     When given a file path (and parent directory--normally root),
     this function traverses the directories in the path and at each
     level calls the supplied callback function while also providing
     the supplied object for context if required.

       e.g. given the path '/foo/bar/baz'
            the callback would be called at the equivalent of
        '/foo', '/foo/bar' and '/foo/bar/baz'.

     The implementation swaps between two different directory/file
     handles as it traverses the directories and does not use recursion
     in an attempt to use memory efficiently.

     If a callback wishes to stop the directory traversal it should
     return false--in this case the function will stop the traversal,
     tidy up and return false.

     If a directory path doesn't exist at some point this function will
     also return false and not subsequently call the callback.

     If a directory path specified is complete, valid and the callback
     did not indicate the traversal should be interrupted then this
     function will return true.

   */


  SdFile subfile1;
  SdFile subfile2;

  char buffer[PATH_COMPONENT_BUFFER_LEN]; 

  unsigned int offset = 0;

  SdFile *p_parent;
  SdFile *p_child;

  SdFile *p_tmp_sdfile;  
  
  p_child = &subfile1;
  
  p_parent = &parentDir;

  while (true) {

    boolean moreComponents = getNextPathComponent(filepath, &offset, buffer);

    boolean shouldContinue = callback((*p_parent), buffer, !moreComponents, object);

    if (!shouldContinue) {
      // TODO: Don't repeat this code?
      // If it's one we've created then we
      // don't need the parent handle anymore.
      if (p_parent != &parentDir) {
        (*p_parent).close();
      }
      return false;
    }
    
    if (!moreComponents) {
      break;
    }
    
    boolean exists = (*p_child).open(*p_parent, buffer, O_RDONLY);

    // If it's one we've created then we
    // don't need the parent handle anymore.
    if (p_parent != &parentDir) {
      (*p_parent).close();
    }
    
    // Handle case when it doesn't exist and we can't continue...
    if (exists) {
      // We alternate between two file handles as we go down
      // the path.
      if (p_parent == &parentDir) {
        p_parent = &subfile2;
      }

      p_tmp_sdfile = p_parent;
      p_parent = p_child;
      p_child = p_tmp_sdfile;
    } else {
      return false;
    }
  }
  
  if (p_parent != &parentDir) {
    (*p_parent).close(); // TODO: Return/ handle different?
  }

  return true;
}



/*

   The callbacks used to implement various functionality follow.

   Each callback is supplied with a parent directory handle,
   character string with the name of the current file path component,
   a flag indicating if this component is the last in the path and
   a pointer to an arbitrary object used for context.

 */

boolean callback_pathExists(SdFile& parentDir, const char *filePathComponent, 
                boolean /* isLastComponent */, void * /* object */) {
  /*

    Callback used to determine if a file/directory exists in parent
    directory.

    Returns true if file path exists.

  */
  SdFile child;

  boolean exists = child.open(parentDir, filePathComponent, O_RDONLY);
  
  if (exists) {
     child.close(); 
  }
  
  return exists;
}



boolean callback_makeDirPath(SdFile& parentDir, const char *filePathComponent, 
                 boolean isLastComponent, void *object) {
  /*

    Callback used to create a directory in the parent directory if
    it does not already exist.

    Returns true if a directory was created or it already existed.

  */
  boolean result = false;
  SdFile child;
  
  result = callback_pathExists(parentDir, filePathComponent, isLastComponent, object);
  if (!result) {
    result = child.makeDir(parentDir, filePathComponent);
  } 
  
  return result;
}


  /*

boolean callback_openPath(SdFile& parentDir, char *filePathComponent, 
              boolean isLastComponent, void *object) {

    Callback used to open a file specified by a filepath that may
    specify one or more directories above it.

    Expects the context object to be an instance of `SDClass` and
    will use the `file` property of the instance to open the requested
    file/directory with the associated file open mode property.

    Always returns true if the directory traversal hasn't reached the
    bottom of the directory heirarchy.

    Returns false once the file has been opened--to prevent the traversal
    from descending further. (This may be unnecessary.)

  if (isLastComponent) {
    SDClass *p_SD = static_cast<SDClass*>(object);
    p_SD->file.open(parentDir, filePathComponent, p_SD->fileOpenMode);
    if (p_SD->fileOpenMode == FILE_WRITE) {
      p_SD->file.seekSet(p_SD->file.fileSize());
    }
    // TODO: Return file open result?
    return false;
  }
  return true;
}
  */



boolean callback_remove(SdFile& parentDir, const char *filePathComponent, 
            boolean isLastComponent, void * /* object */) {
  if (isLastComponent) {
    return SdFile::remove(parentDir, filePathComponent);
  }
  return true;
}

boolean callback_rmdir(SdFile& parentDir, const char *filePathComponent, 
            boolean isLastComponent, void * /* object */) {
  if (isLastComponent) {
    SdFile f;
    if (!f.open(parentDir, filePathComponent, O_READ)) return false;
    return f.rmDir();
  }
  return true;
}



/* Implementation of class used to create `SDCard` object. */



boolean SDClass::begin(uint8_t csPin, uint8_t type) {
  /*

    Performs the initialisation required by the sdfatlib library.

    Return true if initialization succeeds, false otherwise.

   */

  return card.init(SPI_HALF_SPEED, csPin, type) &&
         volume.init(card) &&
         root.openRoot(volume);
}

boolean SDClass::begin(uint32_t clock, uint8_t csPin, uint8_t type) {
  if(type == TYPE_SDIO_FLASH){
      if(root.isOpen()) root.close();
      return card.init(SPI_HALF_SPEED, csPin, type, clock) &&
         volume.init(card) &&
         root.openRoot(volume);
  }
  
  return card.init(SPI_HALF_SPEED, csPin, type) &&
         card.setSpiClock(clock) &&
         volume.init(card) &&
         root.openRoot(volume);
}

// this little helper is used to traverse paths
/*获取父级目录，如/,*/
SdFile SDClass::getParentDir(const char *filepath, int *index) {
  // get parent directory
  SdFile d1 = root; // start with the mostparent, root!
  SdFile d2;

  // we'll use the pointers to swap between the two objects
  SdFile *parent = &d1;
  SdFile *subdir = &d2;
  
  const char *origpath = filepath;
  //rt_kprintf("fun=%s, %s %d\n", __func__, filepath, strchr(filepath, '/'));

  while (strchr(filepath, '/')) {
    // get rid of leading /'s
    if (filepath[0] == '/') {
      filepath++;
      continue;
    }
    
    if (! strchr(filepath, '/')) {
      // it was in the root directory, so leave now
      break;
    }

    // extract just the name of the next subdirectory
    uint8_t idx = strchr(filepath, '/') - filepath;
    if (idx > 12)
      idx = 12;    // dont let them specify long names
    char subdirname[13];
    strncpy(subdirname, filepath, idx);
    subdirname[idx] = 0;

    // close the subdir (we reuse them) if open
    subdir->close();
    if (! subdir->open(parent, subdirname, O_READ)) {
      // failed to open one of the subdirectories
      return SdFile();
    }
    // move forward to the next subdirectory
    filepath += idx;

    // we reuse the objects, close it.
    parent->close();

    // swap the pointers
    SdFile *t = parent;
    parent = subdir;
    subdir = t;
  }

  *index = (int)(filepath - origpath);
  // parent is now the parent diretory of the file!
  return *parent;
}


File SDClass::open(const char *filepath, uint8_t mode) {
  /*

     Open the supplied file path for reading or writing.

     The file content can be accessed via the `file` property of
     the `SDClass` object--this property is currently
     a standard `SdFile` object from `sdfatlib`.

     Defaults to read only.

     If `write` is true, default action (when `append` is true) is to
     append data to the end of the file.

     If `append` is false then the file will be truncated first.

     If the file does not exist and it is opened for writing the file
     will be created.

     An attempt to open a file for reading that does not exist is an
     error.

   */
  SdFile file;
  if(((root.volume())->sdCard())->_type == TYPE_ONBOARD_FLASH_BK7252){
      if(String(filepath)== "/"){
          //File myFile = File(root, "/");
          //myFile.rewindDirectory();
          //rt_kprintf("***fun=%s, %s, %d\n", __func__, myFile.name(),myFile);
          //if(!myFile) root.openRoot(volume);
          root.openRoot(volume);
          return File(root, "/");
      } 
      if(file.open(&root, filepath, mode)) return File(file, file._bkFd.path);
      else return File();
  }
  int pathidx;

  // do the interative search
  SdFile parentdir = getParentDir(filepath, &pathidx);
  // no more subdirs!
  
  filepath += pathidx;
  
  if (! filepath[0]) {
    // it was the directory itself!
    return File(parentdir, "/");
  }
  
  // failed to open a subdir!
  if (!parentdir.isOpen())
    return File();
  
  // there is a special case for the Root directory since its a static dir
  if (parentdir.isRoot()) {
    if ( ! file.open(root, filepath, mode)) {
      // failed to open the file :(
      return File();
    }
    // dont close the root!
  } else {
    if ( ! file.open(parentdir, filepath, mode)) {
      return File();
    }
    // close the parent
    parentdir.close();
  }
  
  if ((mode & (O_APPEND | O_WRITE)) == (O_APPEND | O_WRITE))
    file.seekSet(file.fileSize());
  return File(file, filepath);
  
}





boolean SDClass::exists(const char *filepath) {
  /*
     Returns true if the supplied file path exists.
   */
  if(((root.volume())->sdCard())->_type == TYPE_ONBOARD_FLASH_BK7252){
      if(bkIsDir(getFullpath(filepath).c_str()) == -1) return false;
      else return true;
  }
  return walkPath(filepath, root, callback_pathExists);
}


//boolean SDClass::exists(char *filepath, SdFile& parentDir) {
//  /*
//
//     Returns true if the supplied file path rooted at `parentDir`
//     exists.
//
//   */
//  return walkPath(filepath, parentDir, callback_pathExists);
//}


boolean SDClass::mkdir(const char *filepath) {
  /*
    Makes a single directory or a heirarchy of directories.
    A rough equivalent to `mkdir -p`.
   */
  if(exists(filepath)) return false;
  if(((root.volume())->sdCard())->_type == TYPE_ONBOARD_FLASH_BK7252){
      return root.makeDir(&root, filepath);
  }
  return walkPath(filepath, root, callback_makeDirPath);
}

boolean SDClass::rmdir(const char *filepath) {
  /*
  
    Remove a single directory or a heirarchy of directories.

    A rough equivalent to `rm -rf`.
  
   */
  if(((root.volume())->sdCard())->_type == TYPE_ONBOARD_FLASH_BK7252){
      int result;
      result = dfs_file_unlink(getFullpath(filepath).c_str());
      if (result < 0)
      {
          rt_set_errno(result);
          return false;
      }
      return true;
  }
  return walkPath(filepath, root, callback_rmdir);
}

boolean SDClass::remove(const char *filepath) {
  if(((root.volume())->sdCard())->_type == TYPE_ONBOARD_FLASH_BK7252){
      int result;
      result = dfs_file_unlink(getFullpath(filepath).c_str());
      if (result < 0)
      {
          rt_set_errno(result);
          return false;
      }
      return true;
  }
  return walkPath(filepath, root, callback_remove);
}


// allows you to recurse into a directory
File File::openNextFile(uint8_t mode) {
  if(((_file->volume())->sdCard())->_type == TYPE_ONBOARD_FLASH_BK7252){
      struct dfs_fd d;
      SdFile f;
      struct dirent dirent;
      File myf;
      int result = _file->readDir(&dirent);
      //rt_kprintf("%s1, %s, result=%d\n", __func__, _name, result);
      if(result  > 0){
          //rt_kprintf("123 %s  %d\n", _name, result);
          char *fullpath = dfs_normalize_path(_name, dirent.d_name);
          
          int state = bkIsDir(fullpath);
          //rt_kprintf("fun=%s, state=%d  %s, %x  %x\n", __func__, state, fullpath, O_DIRECTORY,O_RDONLY);
          memset(&d, 0, sizeof(dfs_fd));
          if (state == 1){
              if (f.open(_file, fullpath, O_DIRECTORY)) {
                  //rt_kprintf("ok1\n");
                  myf = File(f, fullpath);
                  free(fullpath);
                return myf;//File(f, fullpath);    
              } else {
                //Serial.println("ugh");
                //rt_kprintf("fail1\n");
                free(fullpath);
                return File();
              }
          }else if(state == 0){
              if (f.open(_file, fullpath, O_RDONLY)) {
                //rt_kprintf("ok2\n");
                myf = File(f, fullpath);
                free(fullpath);
                
                return myf;    
              } else {
                //rt_kprintf("fail2\n");
                free(fullpath);
                return File();
              }
          }
      }
      return File();
  }else{
      dir_t p;

      //Serial.print("\t\treading dir...");
      while (_file->readDir(&p) > 0) {
      
        // done if past last used entry
        if (p.name[0] == DIR_NAME_FREE) {
          //Serial.println("end");
          return File();
        }
      
        // skip deleted entry and entries for . and  ..
        if (p.name[0] == DIR_NAME_DELETED || p.name[0] == '.') {
          //Serial.println("dots");
          continue;
        }
      
        // only list subdirectories and files
        if (!DIR_IS_FILE_OR_SUBDIR(&p)) {
          //Serial.println("notafile");
          continue;
        }
      
        // print file name with possible blank fill
        SdFile f;
        char name[13];
        _file->dirName(p, name);
        //Serial.print("try to open file ");
        //Serial.println(name);
      
        if (f.open(_file, name, mode)) {
          //Serial.println("OK!");
          return File(f, name);    
        } else {
          //Serial.println("ugh");
          return File();
        }
      }
      
      //Serial.println("nothing");
      return File();
  }
  
  
}

void File::rewindDirectory(void) {  
  if (isDirectory())
    _file->rewind();
}

SDClass SD;

};
