/*!
 * @file WiFiUDPAPSendAndRecv.ino
 * @brief 用BK创建一个WiFi名为"wifiTest"的热点，启动UDP主机，向远程UDP主机发送信息,并接收解析远程的UDP主机的ip地址和端口号，并串口打印接收的数据。
 * @n 使用WiFi库examples文件下网络调试助手NetAssist.exe创建一个远程UDP主机,注意：该远程UDP主机需连接"wifiTest",使其与BK处于同一局域网。为了防止远
 * @n 程UDP主机接收不到数据，请关闭电脑防火墙。
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
const int myudpPort = 8080;

//Are we currently connected?
boolean connected = false;

//The udp library class
WiFiUDP udp;

void setup(){
  // Initilize hardware serial:
  Serial.begin(115200);
  while(!Serial){
      
  }
  
  //Create a AP node.
  WiFi.softAP(WIFI_SSID,WIFI_PASSWORD);
  delay(2);
  
  //initializes the UDP state
  //This initializes the transfer buffer
  if(udp.begin(WiFi.softAPIP(),myudpPort)){
      Serial.print("I'm a UDP host, my ip is ");
      Serial.print(WiFi.softAPIP());
      Serial.print(", my port is ");
      Serial.print(myudpPort);
      Serial.println(". Welcome to communicate with me!");
      connected = true;
  }

}

void loop(){
  //only send data when connected
  if(connected){
    Serial.println("\nWaiting for data from rempte UDP host...");
    while(1){
        udp.parsePacket();
        if(udp.available()) break;
        delay(1000);
    }
    Serial.print("[Receive from ");
    Serial.print(udp.remoteIP());
    Serial.print(" : ");
    Serial.print(udp.remotePort());
    Serial.print("]: ");
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
    Serial.print("Send left bytes to UDP Server " + String(udpRemoteIP) + " : ");
    Serial.println(udpRemotePort);
    udp.beginPacket(udpRemoteIP,udpRemotePort);
    udp.println(recvBuf);
    udp.endPacket();
    //Send a packet
    Serial.print("Send message to UDP Server "+String(udpRemoteIP) + " : ");
    Serial.println(udpRemotePort);
    udp.beginPacket(udpRemoteIP,udpRemotePort);
    udp.printf("Seconds since boot: %lu\n", millis()/1000);
    udp.println("Please send a message to me!!!");
    udp.print("My ip is ");
    udp.println(WiFi.localIP());
    udp.print("My port is ");
    udp.println(udpRemotePort);
    udp.endPacket();
    Serial.println("Send done!\n");

    Serial.print("Waiting for message from UDP Server "+String(udpRemoteIP) + " : ");
    Serial.println(udpRemotePort);
    Serial.println(".......");
  }
  //Wait for 1 second
  delay(1000);
}

