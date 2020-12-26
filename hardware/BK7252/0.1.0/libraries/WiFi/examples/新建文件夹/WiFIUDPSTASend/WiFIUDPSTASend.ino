/*!
 * @file WiFiUDPSTASend.ino
 * @brief 连接wifi，创建一个UDP主机，定时向远程UDP主机发送信息。使用WiFi库examples文件下网络
 * @n 调试助手NetAssist.exe创建一个远程UDP主机,注意：该UDP主机的IP地址必须和BK板子连接的WiFi
 * @n 处于同一局域网,为了防止远程UDP主机接收不到数据，请关闭防火墙。
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
const char * WIFI_SSID = "hitest";
const char * WIFI_PASSWORD = "12345678";

//IP address to send UDP data to:
// either use the ip address of the server or 
// a network broadcast address
const char * udpRemoteIP = "192.168.2.142";
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
  // put your setup code here, to run once:
  // Initilize hardware serial:
  Serial.begin(115200);
  while(!Serial){
      
  }
  
  //Connect to the WiFi network
  connectToWiFi(WIFI_SSID, WIFI_PASSWORD);
  
  //initializes the UDP state,This initializes the transfer buffer.
  if(udp.begin(WiFi.localIP(),myUdpPort)){
      Serial.print("I'm a UDP host, my ip is ");
      Serial.print(WiFi.localIP());
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
  Serial.print("WiFi connected!  IP address: ");
  Serial.println(WiFi.localIP());
}