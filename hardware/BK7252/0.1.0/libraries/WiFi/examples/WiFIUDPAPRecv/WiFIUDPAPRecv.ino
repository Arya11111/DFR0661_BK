/*!
 * @file WiFiUDPAPRecv.ino
 * @brief 用BK创建一个WiFi名为"wifiTest"的热点，启动UDP主机，接收并解析远程的UDP主机的ip地址和端口号，并串口打印接收的数据。
 * @n 注意远程UDP主机需连接"wifiTest",使其与BK处于同一局域网。
 *
 * @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @licence     The MIT License (MIT)
 * @author [Arya](xue.peng@dfrobot.com)
 * @version  V1.0
 * @date  2019-10-11
 * @get from https://www.dfrobot.com
 * @url
 */

#include <WiFi.h>
#include <WiFiUdp.h>

// WiFi network name and password:
const char * WIFI_SSID = "wifiTest";
const char * WIFI_PASSWORD = "12345678";

const int myUdpPort = 3333;

//Are we currently connected?
boolean connected = false;

//The udp library class
WiFiUDP udp;

void setup() {
  // put your setup code here, to run once:
  // Initilize hardware serial:
  Serial.begin(115200);
  while(!Serial){
      
  }
  
  //Create a AP node.
  WiFi.softAP(WIFI_SSID,WIFI_PASSWORD);
  
  Serial.println("I.m a wifi AP: ");
  
  Serial.print("SSID: ");
  Serial.println(WIFI_SSID);
  Serial.print("PASSWORD: ");
  Serial.println(WIFI_PASSWORD);
   Serial.print(WiFi.softAPIP());
  
  //initializes the UDP state,This initializes the transfer buffer.
  if(udp.begin(WiFi.softAPIP(),myUdpPort)){
      Serial.print("I'm a UDP host, my ip is ");
      Serial.print(WiFi.softAPIP());
      Serial.print(", my port is ");
      Serial.print(myUdpPort);
      Serial.println(". Welcome to communicate with me!");
      connected = true;
  }
  Serial.println("\nWaiting for data from rempte UDP host...");
}

void loop() {
  //only received data when connected
  if(connected){
      while(1){
          udp.parsePacket();
          if(udp.available()) break;
          //Wait for 1 second
          delay(1000);
      }
      Serial.print("[Receive from ");
      Serial.print(udp.remoteIP());
      Serial.print(" : ");
      Serial.print(udp.remotePort());
      Serial.print("]: ");
      int totalBytes = udp.available();
      char recvBuf[totalBytes+1];
      udp.read(recvBuf, totalBytes);
      recvBuf[totalBytes]='\0';
      Serial.println(recvBuf);
  }
  //Wait for 1 second
  delay(1000);
}
