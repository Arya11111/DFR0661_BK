/*!
 * @file WiFiAP.ino
 * @brief 创建一个名为wifiTest的热点(AP),并串口打印该热点的ip/mac/等信息。
 * @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @licence     The MIT License (MIT)
 * @author [Arya](xue.peng@dfrobot.com)
 * @version  V1.0
 * @date  2019-10-11
 * @get from https://www.dfrobot.com
 * @url
 */

#include <WiFi.h>
#include <WiFiAP.h>

// WiFi network name and password:
const char * WIFI_SSID = "wifiTest";
const char * WIFI_PASSWORD = "12345678";

void setup() {
  // put your setup code here, to run once:
  // Initilize hardware serial:
  Serial.begin(115200);
  while(!Serial){
      
  }
  
  //Create a AP node.
  WiFi.softAP(WIFI_SSID,WIFI_PASSWORD);
  //WiFi.softAP(WIFI_SSID)//创建开放的wifi，没有密码的wifi
  
  Serial.println("I.m a wifi AP: ");
  
  Serial.print("SSID: ");
  Serial.println(WIFI_SSID);
  Serial.print("PASSWORD: ");
  Serial.println(WIFI_PASSWORD);
  
  Serial.print("ip: ");
  Serial.println(WiFi.softAPIP());
  
  Serial.print("mac: ");
  Serial.println(WiFi.softAPmacAddress());
  
  Serial.print("networkID: ");
  Serial.println(WiFi.softAPNetworkID());
  
  Serial.print("subnetCIDR: ");
  Serial.println(WiFi.softAPSubnetCIDR());
  
  Serial.print("broadcastIP: ");
  Serial.println(WiFi.softAPBroadcastIP());
}

void loop() {
  // put your main code here, to run repeatedly:
  while(!Serial.available());
  while(Serial.available()){
      Serial.read();
  }
  Serial.println(WiFi.softAPgetStationNum()); 
}


