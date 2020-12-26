/*
 ESP8266WiFiScan.h - esp8266 Wifi support.
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

#ifndef BK7251WIFISCAN_H_
#define BK7251WIFISCAN_H_

#include <Arduino.h>
#include "WiFiType.h"
#include "WiFiGeneric.h"

#define wifi_auth_mode_t  rt_wlan_security_t
#define WIFI_AUTH_OPEN SECURITY_OPEN

class WiFiScanClass
{

public:
  WiFiScanClass():_scanResult(NULL){}
  /**
   * @brief Start scan WiFi networks available
   * @return Number of discovered networks
   */
  int8_t scanNetworks(bool async = false, bool show_hidden = false, bool passive = false, uint32_t max_ms_per_chan = 300);
  /**
   * @brief called to get the scan state in Async mode
   * @return scan result or status
   *          -1 if scan not fin
   *          -2 if scan not triggered
   */
  int8_t scanComplete();
  /**
   * @brief delete last scan result from RAM
   */
  void scanDelete();

  /**
   * @brief loads all infos from a scanned wifi in to the ptr parameters
   * @param networkItem uint8_t
   * @param ssid  const char**
   * @param encryptionType uint8_t *
   * @param RSSI int32_t *
   * @param BSSID uint8_t **
   * @param channel int32_t *
   * @return (true if ok)
   */
  bool getNetworkInfo(uint8_t networkItem, String &ssid, uint8_t &encryptionType, int32_t &RSSI, uint8_t* &BSSID, int32_t &channel);
  /**
   * @brief Return the SSID discovered during the network scan.
   * @param i     specify from which network item want to get the information
   * @return       ssid string of the specified item on the networks scanned list
   */
  String SSID(uint8_t networkItem);
  /**
   * @brief Return the encryption type of the networks discovered during the scanNetworks
   * @param i specify from which network item want to get the information
   * @return  encryption type (enum wl_enc_type) of the specified item on the networks scanned list
   */
  wifi_auth_mode_t encryptionType(uint8_t networkItem);//加密方式
  String getEncryptionTypeInfo(wifi_auth_mode_t secMode);//加密方式
  /**
   * @brief Return the RSSI of the networks discovered during the scanNetworks
   * @param i specify from which network item want to get the information
   * @return  signed value of RSSI of the specified item on the networks scanned list
   */
  int32_t RSSI(uint8_t networkItem);
  /**
   * @brief return MAC / BSSID of scanned wifi
   * @param i specify from which network item want to get the information
   * @return uint8_t * MAC / BSSID of scanned wifi
   */
  uint8_t * BSSID(uint8_t networkItem);
  /**
   * @brief return MAC / BSSID of scanned wifi
   * @param i specify from which network item want to get the information
   * @return String MAC / BSSID of scanned wifi
   */
  String BSSIDstr(uint8_t networkItem);
  int32_t channel(uint8_t networkItem);
  /**
   *
   * @param i specify from which network item want to get the information
   * @return bss_info *
   */
  //static void * getScanInfoByIndex(int i) { return _getScanInfoByIndex(i); }; 
  /**
   * private
   * @brief scan callback
   * @param result  void *arg
   * @param status STATUS
   */
  static void _scanDone();
protected:
  //static bool _scanAsync;
  static bool _scanStarted;
  static bool _scanComplete;

  //static uint32_t _scanTimeout;
  //static uint16_t _scanCount;
  //static void* _scanResult;
  //static void * _getScanInfoByIndex(int i);
  struct rt_wlan_scan_result *_scanResult;
  int16_t _num;

};


#endif /* ESP32WIFISCAN_H_ */
