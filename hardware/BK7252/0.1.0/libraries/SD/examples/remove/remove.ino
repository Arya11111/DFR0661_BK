/*!
 * @file remove.ino
 * @brief 移除所有文件
 * @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @licence     The MIT License (MIT)
 * @author [Arya](xue.peng@dfrobot.com)
 * @version  V1.0
 * @date  2020-1-27
 * @get from https://www.dfrobot.com
 * @url
 */
#include <SD.h>

#define NONBOARD_SD_MOUDLE_CS 2

File root;

void setup(){
  SerialUSB.begin(115200);
  while(!SerialUSB){
  ; // wait for serial port to connect. Needed for native USB port only
  }
  
  //如果使用BK7252板载的sdio flash，初始化如下,注意sdio是不需要片选引脚的，这里是为了兼容官方的spi sd库
  //你可以随便填数字，也可以填参数FLASH_CHIP_SELECT_PIN
  SerialUSB.print("Initializing BK7252 SDIO flash...");
  if (!SD.begin(/*csPin = */FLASH_CHIP_SELECT_PIN, /*type = */TYPE_ONBOARD_FLASH_BK7252)) {
    SerialUSB.println("initialization failed!");
    while(1);
  }
  //如果外部连接spi SD卡模块，初始化如下
  //SerialUSB.print("Initializing SD card...");
  //if (!SD.begin(/*csPin = */NONBOARD_SD_MOUDLE_CS, /*type = */TYPE_NONBOARD_SD_MOUDLE)) {
  //  SerialUSB.println("initialization failed!");
  //  while(1);
  //}
  SerialUSB.println("initialization done.");
  
  //01.打印目录
  SerialUSB.println("============print=============");
  root = SD.open("/");
  printDirectory(root, 0);
  
  //02.返回目录的第1个文件，并依次删除
  SerialUSB.println("============remove============");
  root.rewindDirectory();
  removeDirectory(root);
  
  //02.打印目录
  SerialUSB.println("============print=============");
  root = SD.open("/");
  root.rewindDirectory();
  SD.mkdir("/sd");
  printDirectory(root, 0);
  SerialUSB.println("=============end==============");
  root.close();
}

void loop(){
  
}
//06移除文件夹和文件夹里面的文件
void removeDirectory(File dir) {
  while (true) {
    File entry =  dir.openNextFile();
    if (!entry) {
      SerialUSB.print("remove [" + String(dir.name()) +"] floder...");
      dir.close();
      if(SD.rmdir(dir.name())){
          SerialUSB.println("done.");
      }else{
          SerialUSB.println("failed.");
      }
      break;
    }

    if (entry.isDirectory()) {
      removeDirectory(entry);
    } else {
      // files have sizes, directories do not
      SerialUSB.print("remove [" + String(entry.name()) +"] file...");
      entry.close();
      if(SD.remove(entry.name())){
          SerialUSB.println("done.");
      }else{
          SerialUSB.println("failed.");
      }
    }
  }
}
void printDirectory(File dir, int numTabs) {
  while (true) {
    File entry =  dir.openNextFile();
    if (!entry) {
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      SerialUSB.print('\t');
    }
    SerialUSB.print(entry.name());
    if (entry.isDirectory()) {
      SerialUSB.println("/");
      printDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      SerialUSB.print("\t\t");
      SerialUSB.println(entry.size(), DEC);
    }
    entry.close();
  }
}

