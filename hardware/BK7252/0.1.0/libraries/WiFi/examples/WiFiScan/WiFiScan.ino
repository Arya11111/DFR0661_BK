/*!
 * @file WiFiScan.ino
 * @brief 扫描热点信息
 * @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @licence     The MIT License (MIT)
 * @author [Arya](xue.peng@dfrobot.com)
 * @version  V1.0
 * @date  2019-10-11
 * @get from https://www.dfrobot.com
 * @url
 */
#include "WiFi.h"
#include "IPAddress.h"

void setup()
{
  // put your setup code here, to run once:
  // Initilize hardware serial:
  Serial.begin(115200);
  while(!Serial){

  }
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  Serial.println("Setup done");
}

void loop()
{
  Serial.println("scan start");
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0) {
      Serial.println("no networks found!");
  } else {
      Serial.print(n);
      Serial.println(" networks found");
      for (int i = 0; i < n; ++i) {
          // Print SSID and RSSI for each network found
          Serial.print(i + 1);
          Serial.print(": ");
          Serial.print(WiFi.SSID(i));
          Serial.print(" (");
          Serial.print(WiFi.RSSI(i));
          Serial.print(")");
          Serial.print(" (");
          Serial.print(WiFi.BSSIDstr(i));
          Serial.print(")");
          Serial.print(" (");
          Serial.print(WiFi.channel(i));
          Serial.print(")");
          Serial.print(" (");
          Serial.print(WiFi.getEncryptionTypeInfo(WiFi.encryptionType(i)));
          Serial.print(")");
          Serial.println();
         // Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
          delay(10);
      }
  }
  Serial.println("");
  delay(5000);
}
