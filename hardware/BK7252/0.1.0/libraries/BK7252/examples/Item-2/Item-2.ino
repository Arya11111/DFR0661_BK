/*!
 * @file Item-2.ino
 * @brief USB串口数据收发实验。
 * @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @licence     The MIT License (MIT)
 * @author [Arya](xue.peng@dfrobot.com)
 * @version  V1.0
 * @date  2020-12-2
 * @get from https://www.dfrobot.com
 * @url
 */


void setup() {
  Serial.begin(115200);

  /*等待串口监视器COM口打开*/
  while(!Serial){
  }
}

void loop() {
  /*如果未检测到USB串口有数据，则等待*/
  while(!Serial.available());

  while(Serial.available()){
      /*打印USB串口接收到的数据*/
      Serial.print((char)Serial.read());
  }
}
