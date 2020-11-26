/*
 ESP8266WiFiSTA.h - esp8266 Wifi support.
 Based on WiFi.h from Ardiono WiFi shield library.
 Copyright (c) 2011-2014 Arduino.  All right reserved.
 Modified by Ivan Grokhotkov, December 2014
 Reworked by Markus Sattler, December 2015

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef ESP32WIFISTA_H_
#define ESP32WIFISTA_H_

#include <Arduino.h>
#include "WiFiType.h"
//#include "WiFiGeneric.h"
extern "C"{
#include "drv_wlan.h"
#include "dns.h"
#include "lwip/ip_addr.h"
}

class WiFiSTAClass
{
public:
  wl_status_t begin(char* ssid, char *passphrase = NULL, int32_t channel = 0, const uint8_t* bssid = NULL, bool connect = true);
  wl_status_t begin(const char* ssid, const char *passphrase = NULL, int32_t channel = 0, const uint8_t* bssid = NULL, bool connect = true){
      return begin((char*) ssid, (char*) passphrase, channel, bssid, connect);
  }
  //wl_status_t begin(const char* ssid, const char *passphrase = NULL){ return begin((char*) ssid, (char*) passphrase);}
  //wl_status_t begin(char* ssid, char *passphrase = NULL);
  wl_status_t begin();
  bool reconnect();
  uint8_t waitForConnectResult();
  /**
   * @brief Disconnect from the network.
   * @return one value of wl_status_t enum
   */
  bool disconnect(bool wifioff = false, bool eraseap = false);
  /**
   * @brief is STA interface connected?
   * @return true if STA is connected to an AD
   */
  bool isConnected();
  /**
   * @brief Get the station interface IP address.
   * @return IP address,pointer to char *
   */
  //char * localIP();
  IPAddress localIP();
  /**
   * @brief Get the station interface MAC address.
   * @return String mac
   */
  uint8_t * macAddress(uint8_t* mac);
  String macAddress();
  /**
   * @brief Get the interface subnet mask address.
   * @return subnetMask, pointer to char *
   */
  //char * subnetMask();
  IPAddress subnetMask();
  /**
   * @brief Get the gateway ip address.
   * @return gatewayIP, pointer to char *
   */
  //char * gatewayIP();
  IPAddress gatewayIP();
  
  IPAddress dnsIP(uint8_t dns_no = 0);
  IPAddress broadcastIP();
  IPAddress networkID();
  uint8_t subnetCIDR();
  /**
   * @brief Return the current SSID associated with the network
   * @return SSID
   */
  String  SSID();
  /**
   * @brief Return the current pre shared key associated with the network
   * @return  psk string
   */
  String psk();
  /**
   * @brief Return the current bssid / mac associated with the network if configured
   * @return bssid uint8_t *
   */
  uint8_t * BSSID();
  /**
   * @brief Return the current bssid / mac associated with the network if configured.
   * @return String bssid mac
   */
  String BSSIDstr();
  /**
   * @brief Return the current network RSSI.
   * @return  RSSI value
   */
  int8_t RSSI();
  static wl_status_t status();

protected:
  void getIPStatus();

protected:
  IPStatusTypedef _ipStatus;

};



#endif /* ESP32WIFISTA_H_ */
