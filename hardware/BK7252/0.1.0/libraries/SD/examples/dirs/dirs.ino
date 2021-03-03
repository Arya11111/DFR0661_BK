/*!
 * @file dirs.ino
 * @brief 创建、移除文件夹
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

File floder;

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
  
  bool result;

  //01.Check to see if the floder exists(判断文件夹/sd是否存在)
  if(SD.exists("/sd")){
      SerialUSB.println("/sd floder exists.");
  }else{
      SerialUSB.println("/sd floder doesn't exist.");
  }
  //02.创建/sd文件夹，并判断是否创建成功
  SerialUSB.print("Creating /sd floder...");
  if(SD.mkdir("/sd")){
      SerialUSB.println("done.");
  }else{
      SerialUSB.println("failed.");
  }
  //03.Check to see if the floder exists
  if(SD.exists("/sd")){
      SerialUSB.println("/sd floder exists.");
  }else{
      SerialUSB.println("/sd floder doesn't exist.");
  }
  //04.移除文件
  SerialUSB.print("Removing /sd floder...");
  if(SD.rmdir("/sd")){
      SerialUSB.println("done.");
  }else{
      SerialUSB.println("failed.");
  }
  //05.Check to see if the floder exists
  if(SD.exists("/sd")){
      SerialUSB.println("/sd floder exists.");
      floder = SD.open("/sd");
      removeDirectory(floder);
  }else{
      SerialUSB.println("/sd floder doesn't exist.");
  }
}

void loop(){
  
}
//06移除文件夹和文件夹里面的文件（rmdir无法直接删除有内容的文件夹）
void removeDirectory(File dir) {
  while (true) {
    File entry =  dir.openNextFile();
    if (!entry) {
      SerialUSB.print("remove dir: ");
      SerialUSB.println(dir.name());
      dir.close();
      SD.rmdir(dir.name());
      break;
    }

    if (entry.isDirectory()) {
      removeDirectory(entry);
    } else {
      // files have sizes, directories do not
      SerialUSB.print("remove file: ");
      SerialUSB.println(entry.name());
      entry.close();
      SD.remove(entry.name());
    }
  }
}

