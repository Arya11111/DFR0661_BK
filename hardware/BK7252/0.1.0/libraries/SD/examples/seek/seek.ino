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

File myFile;

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
  
  //01.Check to see if the file exists:
  if (SD.exists("example.txt")) {
    SerialUSB.println("example.txt exists.");
  } else {
    SerialUSB.println("example.txt doesn't exist.");
  }
  
  //01.创建或打开example.txt文件，并打印其的信息
  SerialUSB.println("Openning or Creating example.txt...");
  myFile = SD.open("example.txt", FILE_WRITE);
  printFileInfo(myFile);
  
  //02.在文件头写入10个字符'A';
  myFile.seek(0);
  myFile.print("AAAAAAAAAA");
  printFileInfo(myFile);
  
  //03.在文件尾写入10个字符'C'
  myFile.seek(myFile.size());
  myFile.print("CCCCCCCCCC");
  printFileInfo(myFile);
  
  //04.将第5个字符改为'B'
  SerialUSB.print("Changing the fifth char to 'B'...");
  myFile.seek(5);
  myFile.print('B');
  myFile.seek(myFile.position() - 1);
  if(myFile.peek() == 'B'){
      SerialUSB.println("done.");
  }else{
      SerialUSB.println("failed.");
  }
  printFileInfo(myFile);

  myFile.close();
  printFileInfo(myFile);
}

void loop(){
  
}

void printFileInfo(File f){
  SerialUSB.println("=========print file info=========");
  SerialUSB.print("name: ");
  SerialUSB.println(f.name());
  SerialUSB.print("state: ");
  if(f){
      SerialUSB.println("open");
      SerialUSB.print("position: ");
      SerialUSB.println(f.position());
      SerialUSB.print("size: ");
      SerialUSB.println(f.size());
      f.seek(0);
      if(f.available()){
          SerialUSB.print("content: ");
          while(f.available()){
              SerialUSB.write(f.read());
          }
          SerialUSB.println();
      }
      
  }else{
      SerialUSB.println("close");
  }
}

