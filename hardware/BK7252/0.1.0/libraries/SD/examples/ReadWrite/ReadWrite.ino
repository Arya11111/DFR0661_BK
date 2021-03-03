/*!
 * @file ReadWrite.ino
 * @brief SD read/write
 * @n This example shows how to read and write data to and from an UD disk file
 *
 * @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @licence     The MIT License (MIT)
 * @version  V1.0
 * @date  2019-03-5
 * @get from https://www.dfrobot.com
 */
#include <SD.h>

#define NONBOARD_SD_MOUDLE_CS 2//spi 模块的cs引脚连接到M0的数字2引脚

File myFile;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!SerialUSB) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  //如果使用BK7252板载的sdio flash，初始化如下,注意sdio是不需要片选引脚的，这里是为了兼容官方的spi sd库
  //你可以随便填数字，也可以填参数FLASH_CHIP_SELECT_PIN
  Serial.print("Initializing BK7252 SDIO flash...");
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

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  myFile = SD.open("test.txt", FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile) {
    SerialUSB.print("Writing to test.txt...");
    myFile.println("testing 1, 2, 3.");
    // close the file:
    myFile.close();
    SerialUSB.println("done.");
  } else {
    // if the file didn't open, print an error:
    SerialUSB.println("error opening test.txt");
  }

  // re-open the file for reading:
  myFile = SD.open("test.txt");
  if (myFile) {
    Serial.print("test.txt:");
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
}

void loop() {
  // nothing happens after setup
}


