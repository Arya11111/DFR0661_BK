/*
 ESP8266WiFiAP.h - esp8266 Wifi support.
 Based on WiFi.h from Arduino WiFi shield library.
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

#ifndef ESP32WIFIAP_H_
#define ESP32WIFIAP_H_


#include <Arduino.h>
#include "WiFiType.h"

class WiFiAPClass
{
public:
  /**
   * @brief 建立一个AP热点
   * @param ssid              SSID账号 (max 63 char，最大63个字符).
   * @param passphrase        密码(对于WPA2加密类型最少8个字符，对于开放网络设置为NULL)
   * @param channel           WiFi 通道数字, 1 - 13.默认是1
   * @param ssid_hidden       WiFI是否需要隐藏 (0 = broadcast SSID, 1 = hide SSID)，通过它设置别人是否能看到你的WiFi网络
   * @param max_connection    最大的同时连接数  1 - 4.当超过这个数，再多的station想连接也只能等待
   * @param bool 返回设置soft-AP的结果
   */
  bool softAP(const char* ssid, const char* passphrase = NULL, int channel = 11, int ssid_hidden = 0, int max_connection = 4){
      softAP((char*)ssid, (char*)passphrase, channel, ssid_hidden, max_connection);
  }
  bool softAP(char* ssid, char* passphrase = NULL, int channel = 11, int ssid_hidden = 0, int max_connection = 4);
  /**
   * @brief Configure access point
   * @param local_ip      access point IP
   * @param gateway       gateway IP
   * @param subnet        subnet mask
   */
  //bool softAPConfig(IPAddress local_ip, IPAddress gateway, IPAddress subnet);
  bool softAPConfig(String local_ip, String gateway, String subnet);
  /**
   * @brief Disconnect from the network (close AP)
   * @param wifioff disable mode?
   * @return one value of wl_status_t enum
   */
  bool softAPdisconnect();
  /**
   * @brief Get the count of the Station / client that are connected to the softAP interface
   * @return Stations count
   */
  uint8_t softAPgetStationNum();
  /**
   * @brief Get the softAP interface IP address.
   * @return IPAddress softAP IP
   */
  IPAddress softAPIP();
  /**
   * @brief Get the softAP broadcast IP address.
   * @return IPAddress softAP broadcastIP
   */
  IPAddress softAPBroadcastIP();
  /**
   * @brief Get the softAP network ID.
   * @return IPAddress softAP networkID
   */
  IPAddress softAPNetworkID();
  /**
   * @brief Get the softAP subnet CIDR.
   * @return uint8_t softAP subnetCIDR
   */
  uint8_t softAPSubnetCIDR();
  /**
   * @brief Enable IPv6 on the softAP interface.
   * @return true on success
   */
  //bool softAPenableIpV6();
  /**
   * @brief Get the softAP interface IPv6 address.
   * @return IPv6Address softAP IPv6
   */
  //IPv6Address softAPIPv6();

 /**
  * @brief Get the softAP interface Host name.
  * @return char array hostname
  */
  //const char * softAPgetHostname();
  /**
   * @brief Set the softAP    interface Host name.
   * @param  hostname  pointer to const string
   * @return true on   success
   */
  //bool softAPsetHostname(const char * hostname);
  /**
   * @brief Get the softAP interface MAC address.
   * @return String mac
   */
  uint8_t* softAPmacAddress(uint8_t* mac);
  /**
   * @brief Get the softAP interface MAC address.
   * @param mac   pointer to uint8_t array with length WL_MAC_ADDR_LENGTH
   * @return      pointer to uint8_t*
   */
  String softAPmacAddress(void);

protected:
  struct rt_wlan_device *_wlanAp;

};

#endif /* ESP32WIFIAP_H_*/
