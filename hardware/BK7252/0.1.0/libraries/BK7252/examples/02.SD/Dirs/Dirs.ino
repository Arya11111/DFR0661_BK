/*!
 * @file CreateFile.ino
 * @brief 创建文件夹，在文件夹内创建一个test.txt的文件，写入testing 1, 2, 3.内容并读取打印。
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

File myFile;
File root;

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
  bool result;
  //01.判断/sd文件夹是否存在，如果存在，则移除
  if(SD.exists("/sd")){
      SerialUSB.println("/sd folder already exists.");
      //01.1 如果移除成功，则创建，如果失败则跳到LOOP1
      if(SD.rmdir("/sd")){
          SerialUSB.println("/sd folder already remove.");
      }else{
          SerialUSB.println("/sd folder remove failed.");
          goto LOOP1;
      }
  }else{
      SerialUSB.println("/sd folder is not exists.");
  }
  //02.创建文件夹/sd，创建成功，则在该目录下创建一个文件，否则则调到LOOP2
  result = SD.mkdir("/sd");

  //03.在/sd目录下创建test.txt，并写入内容:testing 1, 2, 3.
  if (result) {
      SerialUSB.println("Create dir done.");
LOOP1:
      //03.1判断/sd/test.txt文件是否存在，如果存在，则移除
      if(SD.exists("/sd/test.txt")){
          SerialUSB.println("/sd/test.txt file already exists.");
          if(SD.remove("/sd/test.txt")){
              SerialUSB.println("/sd/test.txt file already remove.");
          }else{
              SerialUSB.println("/sd/test.txt file remove fail.");
          }
      }else{
          SerialUSB.println("/sd/test.txt file is not exists.");
      }
      //03.2打开/sd/test.txt文件，如果成功，则写入内容。
      myFile = SD.open("/sd/test.txt", FILE_WRITE);
      if (myFile) {
          SerialUSB.print("Writing to test.txt...");
          myFile.println("testing 1, 2, 3.");
          myFile.close();
          SerialUSB.println("done.");
      } else {
          SerialUSB.println("error opening /test1.txt");
          goto LOOP2;
      }
  } else {
      SerialUSB.println("error create /sd");
      goto LOOP2;
  }

  //04. 判断/sd/test.txt是否被打开，如果打开则读取该文件内的所有内容
  myFile = SD.open("/sd/test.txt");
  if (myFile) {
    Serial.print("test.txt: ");
    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      SerialUSB.write(myFile.read());
    }
    SerialUSB.println();
    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    SerialUSB.println("error opening test.txt");
  }
  //05.打印文件目录结构
LOOP2:
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

