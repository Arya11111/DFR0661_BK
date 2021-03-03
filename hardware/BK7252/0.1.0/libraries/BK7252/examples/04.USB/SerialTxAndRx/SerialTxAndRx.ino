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
  
}

void loop(){
  while(SerialUSB.available()){
      SerialUSB.print((char)SerialUSB.read());
      //delay(1000);
  }
//SerialUSB.println("abcdefghijklmnopqrstuvwkyzABCDEFGHIJKLMNOPQRSTUVWKYZ0123456789!@#$%^&*()+=_-|{[]}:;""'<,>.?/~`ABCDEFGHIJKLMNOPQRSTUVWKYZ!@#$%^&*()+=_-|{[]}:;""'<,>.?/~`abcdefghijklmnopqrstuvwkyz0123456789abcdefghijklmnopqrstuvwkyzABCDEFGHIJKLMNOPQRSTUVWKYZ0123456789!@#$%^&*()+=_-|{[]}:;""'<,>.?/~`");
//delay(1000);
}

