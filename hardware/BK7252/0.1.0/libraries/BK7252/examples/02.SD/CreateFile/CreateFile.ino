/*!
 * @file CreateFile.ino
 * @brief 创建文件，并打印
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
File myFile;

void setup(){
  SerialUSB.begin(115200);
  while(!SerialUSB){
  ; // wait for serial port to connect. Needed for native USB port only
  }
  
  SerialUSB.print("Initializing BK7252 SDIO flash...");
/*如果使用BK7252板载的sdio flash，初始化如下,注意sdio是不需要片选引脚的，这里是为了兼容官方的spi sd库
  你可以随便填数字，也可以填参数FLASH_CHIP_SELECT_PIN*/
  if (!SD.begin(/*csPin = */FLASH_CHIP_SELECT_PIN, /*type = */TYPE_ONBOARD_FLASH_BK7252)) {
    SerialUSB.println("initialization failed!");
    while(1);
  }
/*如果外部连接spi SD卡模块，初始化如下*/
  //SerialUSB.print("Initializing SD card...");
  //if (!SD.begin(/*csPin = */NONBOARD_SD_MOUDLE_CS, /*type = */TYPE_NONBOARD_SD_MOUDLE)) {
  //  SerialUSB.println("initialization failed!");
  //  while(1);
  //}
  SerialUSB.println("initialization done.");
  
  myFile = SD.open("test.txt", FILE_WRITE);
  if (myFile) {
      SerialUSB.println("Create test.txt done.");
      myFile.close();
  } else {
      SerialUSB.println("error opening test.txt");
  }
  
  myFile = SD.open("/test1.txt", FILE_WRITE);
  if (myFile) {
      SerialUSB.println("Create /test1.txt done.");
      myFile.close();
  } else {
      SerialUSB.println("error opening /test1.txt");
  }
  
  myFile = SD.open("/sd/test.txt", FILE_WRITE);
  if (myFile) {
      SerialUSB.println("Create /test1.txt done.");
      myFile.close();
  } else {
    SerialUSB.println("error opening /sd/test.txt");
  }
  root = SD.open("/");
  printDirectory(root, 0);
  
  SerialUSB.println("done!");
}

void loop(){
  
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

