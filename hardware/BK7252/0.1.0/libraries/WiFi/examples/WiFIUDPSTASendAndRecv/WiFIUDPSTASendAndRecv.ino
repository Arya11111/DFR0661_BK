/*!
 * @file WiFiUDPSTASendAndRecv.ino
 * @brief 连接wifi，创建一个UDP主机，向远程UDP主机发送信息,并接收解析远程的UDP主机的ip地址和端口号，并串口打印接收的数据。
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

//IP address to send UDP data to:
// either use the ip address of the server or 
// a network broadcast address
const char * udpAddress = "192.168.2.142";
const int udpPort = 3333;

//Are we currently connected?
boolean connected = false;

//The udp library class
WiFiUDP udp;

void setup(){
  // Initilize hardware serial:
  Serial.begin(115200);
  while(!Serial){
      
  }
  
  //Connect to the WiFi network
  connectToWiFi(WIFI_SSID, WIFI_PASSWORD);
  
  //initializes the UDP state
  //This initializes the transfer buffer
  udp.begin(WiFi.localIP(),udpPort);
  connected = true;

}

void loop(){
  //only send data when connected
  if(connected){
    //Send a packet
    Serial.print("Send message to UDP Server "+String(udpAddress) + " : ");
    Serial.println(udpPort);
    udp.beginPacket(udpAddress,udpPort);
    udp.printf("Seconds since boot: %lu\n", millis()/1000);
    udp.println("Please send a message to me!!!");
    udp.print("My ip is ");
    udp.println(WiFi.localIP());
    udp.print("My port is ");
    udp.println(udpPort);
    udp.endPacket();
    Serial.println("Send done!\n");

    Serial.print("Waiting for message from UDP Server "+String(udpAddress) + " : ");
    Serial.println(udpPort);
    Serial.println(".......");
    udp.parsePacket();
    while(1){
        udp.parsePacket();
        if(udp.available()) break;
        delay(1000);
    }
    int totalBytes = udp.available();
    Serial.print("Total bytes: ");
    Serial.println(totalBytes);

    char readC = udp.read();
    Serial.print("Read char: ");
    Serial.println(readC);
    totalBytes = udp.available();
    Serial.print("Left bytes: ");
    Serial.println(totalBytes);

    char peekC = udp.peek();
    Serial.println("Peek char: "+String(&peekC));
    totalBytes = udp.available();
    Serial.print("Left bytes: ");
    Serial.println(totalBytes);

    char recvBuf[totalBytes+1];
    udp.read(recvBuf, totalBytes);
    recvBuf[totalBytes] = '\0';
    Serial.print("Send left bytes to UDP Server " + String(udpAddress) + " : ");
    Serial.println(udpPort);
    udp.beginPacket(udpAddress,udpPort);
    udp.println(recvBuf);
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


