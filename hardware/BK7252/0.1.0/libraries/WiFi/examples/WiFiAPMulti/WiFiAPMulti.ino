/*!
 * @file WiFiAPMulti.ino
 * @brief 试根据给定的热点列表连接到最佳的AP
 * @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @licence     The MIT License (MIT)
 * @author [Arya](xue.peng@dfrobot.com)
 * @version  V1.0
 * @date  2019-10-11
 * @get from https://www.dfrobot.com
 * @url
 */

#include <WiFi.h>
#include <WiFiMulti.h>

WiFiMulti wifiMulti;
int flag = 0;

void setup()
{
  Serial.begin(115200);
  while(!Serial){
      
  }
  delay(10);

  //wifiMulti.addAP("ssid_from_AP_1", "your_password_for_AP_1");
  //wifiMulti.addAP("ssid_from_AP_2", "your_password_for_AP_2");
  //wifiMulti.addAP("ssid_from_AP_3", "your_password_for_AP_3");
  
  wifiMulti.addAP("hitest", "12345678");
  wifiMulti.addAP("hello", "123456789");
  wifiMulti.addAP("wifiTest", "12345678");

  Serial.println("Connecting Wifi...");
  if(wifiMulti.run() == WL_CONNECTED) {
      Serial.println("");
      Serial.println("WiFi connected");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
  }
}

void loop()
{
  uint8_t state = wifiMulti.run();
  if(state == WL_CONNECTED && flag == 0) {
      flag = 1;
      Serial.println("");
      Serial.println("WiFi connected");
      Serial.println("SSID: ");
      Serial.println(WiFi.SSID());
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
  }
  
  if(state != WL_CONNECTED){
      Serial.println("WiFi not connected!");
      flag = 0;
      delay(1000);
  }
}
