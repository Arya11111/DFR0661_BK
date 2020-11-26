/*!
 * @file WiFiSTA.ino
 * @brief 连接wifi，创建一个WiFi Station.并串口打印该wifi站点的ip/mac/wifi热点名等信息。
 * @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @licence     The MIT License (MIT)
 * @author [Arya](xue.peng@dfrobot.com)
 * @version  V1.0
 * @date  2019-10-11
 * @get from https://www.dfrobot.com
 * @url
 */

#include <WiFi.h>
#include <WiFiSTA.h>

// WiFi network name and password:
const char * WIFI_SSID = "hitest";
const char * WIFI_PASSWORD = "12345678";

void setup() {
  // put your setup code here, to run once:
  // Initilize hardware serial:
  Serial.begin(115200);
  while(!Serial){
      
  }
  
  //Connect to the WiFi network
  connectToWiFi(WIFI_SSID, WIFI_PASSWORD);
  
  Serial.println("I.m a wifi station: ");
  
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  
  Serial.print("ip: ");
  Serial.println(WiFi.localIP());
  
  Serial.print("subnet: ");
  Serial.println(WiFi.subnetMask());
  
  Serial.print("gateway: ");
  Serial.println(WiFi.gatewayIP());
  
  Serial.print("mac: ");
  Serial.println(WiFi.macAddress());
  
  Serial.print("networkID: ");
  Serial.println(WiFi.networkID());
  
  Serial.print("subnetCIDR: ");
  Serial.println(WiFi.subnetCIDR());
  
  Serial.print("broadcastIP: ");
  Serial.println(WiFi.broadcastIP());
}

void loop() {
  // put your main code here, to run repeatedly:

}

void connectToWiFi(const char * ssid, const char * pwd){
  Serial.print("Connecting to ");

  // delete old config
  WiFi.disconnect();
  
  //Initiate connection
  WiFi.begin(ssid, pwd);
  while(WiFi.status() != WL_CONNECTED){
      delay(500);
      Serial.print(".");
  }
  Serial.println(ssid);
}

