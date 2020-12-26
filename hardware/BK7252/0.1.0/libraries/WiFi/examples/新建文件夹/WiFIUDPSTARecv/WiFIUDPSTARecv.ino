/*!
 * @file WiFiUDPSTARecv.ino
 * @brief 连接wifi，创建一个UDP主机，接收解析远程的UDP主机的ip地址和端口号，并串口打印发送的数据。
 * @n 使用WiFi库examples文件下网络调试助手NetAssist.exe创建一个远程UDP主机,注意：该UDP主机的IP地址必
 * @n 须和BK板子连接的WiFi处于同一局域网。
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
  
  //Connect to the WiFi network
  connectToWiFi(WIFI_SSID, WIFI_PASSWORD);
  
  //initializes the UDP state,This initializes the transfer buffer.
  if(udp.begin(WiFi.localIP(),myUdpPort)){
      Serial.print("I'm a UDP host, my ip is ");
      Serial.print(WiFi.localIP());
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
