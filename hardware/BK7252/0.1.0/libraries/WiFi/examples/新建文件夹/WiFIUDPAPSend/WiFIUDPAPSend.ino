/*!
 * @file WiFiUDPAPSend.ino
 * @brief 用BK创建一个WiFi名为"wifiTest"的热点，启动UDP主机，定时向远程UDP主机发送信息。使用WiFi库examples文件下网络
 * @n 调试助手NetAssist.exe创建一个远程UDP主机,注意：该UDP主机需连接"wifiTest",使其与BK处于同一局域网。为了防止远程UDP主
 * @n机接收不到数据，请关闭电脑防火墙。
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

//IP address to send UDP data to:
// either use the ip address of the server or 
// a network broadcast address
const char * udpRemoteIP = "192.168.1.100";
const int udpRemotePort = 3333;
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
  delay(2);
  //initializes the UDP state,This initializes the transfer buffer.
  if(udp.begin(WiFi.softAPIP(),myUdpPort)){
      Serial.print("I'm a UDP host, my ip is ");
      Serial.print(WiFi.softAPIP());
      Serial.print(", my port is ");
      Serial.print(myUdpPort);
      Serial.println(".");
      connected = true;
  }
  Serial.print("\nSend messages to "+String(udpRemoteIP)+" : ");
  Serial.println(udpRemotePort);
  
}

void loop() {
  if(connected){
    //Send a packet
    udp.beginPacket(udpRemoteIP,udpRemotePort);
    udp.printf("Seconds since boot: %lu\n", millis()/1000);
    udp.println("Hello!");
    udp.endPacket();
  }
  //Wait for 1 second
  delay(1000);
}
