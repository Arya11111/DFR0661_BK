/*!
 * @file Item-1.ino
 * @brief USB串口打印计数器实验。
 * @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @licence     The MIT License (MIT)
 * @author [Arya](xue.peng@dfrobot.com)
 * @version  V1.0
 * @date  2020-12-1
 * @get from https://www.dfrobot.com
 * @url
 */

void setup(){
  Serial.begin(115200);
  while(!Serial){
  }//等待串口监视器COM口打开
}

void loop(){
  static unsigned long i = 0;
  Serial.println(i++);
  delay(1000);
}

