/*
 ESP8266WiFiGeneric.h - esp8266 Wifi support.
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

#ifndef BKWIFIGENERIC_H_
#define BKWIFIGENERIC_H_

//#include <esp_err.h>
//#include <esp_event_loop.h>
//#include <functional>
#include "WiFiType.h"
extern "C"{
#include <wlan_dev.h>
#include <drv_wlan.h>
}

#define WiFiEventCb rt_wlan_event_handler
/*
typedef void (*rt_wlan_event_handler)(struct rt_wlan_device *device, rt_wlan_event_t event, void *user_data);
*/
typedef enum {
    WIFI_POWER_19_5dBm = 78,// 19.5dBm
    WIFI_POWER_19dBm = 76,// 19dBm
    WIFI_POWER_18_5dBm = 74,// 18.5dBm
    WIFI_POWER_17dBm = 68,// 17dBm
    WIFI_POWER_15dBm = 60,// 15dBm
    WIFI_POWER_13dBm = 52,// 13dBm
    WIFI_POWER_11dBm = 44,// 11dBm
    WIFI_POWER_8_5dBm = 34,// 8.5dBm
    WIFI_POWER_7dBm = 28,// 7dBm
    WIFI_POWER_5dBm = 20,// 5dBm
    WIFI_POWER_2dBm = 8,// 2dBm
    WIFI_POWER_MINUS_1dBm = -4// -1dBm
} wifi_power_t;


class WiFiGenericClass
{
public:
  WiFiGenericClass();

  int onEvent(WiFiEventCb cbEvent, rt_wlan_event_t event = WIFI_EVT_MAX);
  void removeEvent(WiFiEventCb cbEvent, rt_wlan_event_t event = WIFI_EVT_MAX);

  static int getStatusBits();
  static int waitStatusBits(int bits, uint32_t timeout_ms);

  int32_t channel(void);

  void persistent(bool persistent);//持久的，将wifi数据保存到flash中
  void enableLongRange(bool enable);

  static bool mode(rt_wlan_mode_t);
  static rt_wlan_mode_t getMode();

  bool enableSTA(bool enable = true);
  bool enableAP(bool enable = true);

  bool setSleep(bool enable);
  bool getSleep();

  bool setTxPower(wifi_power_t power);
  wifi_power_t getTxPower();

  static int _eventCallback(void *arg, rt_wlan_event_t event);

protected:
  static bool _persistent;
  static bool _long_range;
  static rt_wlan_mode_t _forceSleepLastMode;
  static struct rt_wlan_device *_bkWifiWlan;

  static int setStatusBits(int bits);
  static int clearStatusBits(int bits);
  static struct rt_wlan_device _bkWlanDevice;
  static struct beken_wifi_info _bkWlanInfo;

public:
  static int hostByName(const char *aHostname, IPAddress &aResult);

  static IPAddress calculateNetworkID(IPAddress ip, IPAddress subnet);
  static IPAddress calculateBroadcast(IPAddress ip, IPAddress subnet);
  static uint8_t calculateSubnetCIDR(IPAddress subnetMask);

protected:
  friend class WiFiSTAClass;
  friend class WiFiScanClass;
  friend class WiFiAPClass;
};

#endif /* BKWIFIGENERIC_H_ */
