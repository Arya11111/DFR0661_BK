/*
 ESP8266WiFiGeneric.cpp - WiFi library for esp8266

 Copyright (c) 2014 Ivan Grokhotkov. All rights reserved.
 This file is part of the esp8266 core for Arduino environment.

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

 Reworked on 28 Dec 2015 by Markus Sattler

 */

#include "WiFi.h"
#include "WiFiGeneric.h"

#define WIFI_DEBUG rt_kprintf
extern "C" {
#include <portmacro.h>
#include <task.h>
#include <rw_pub.h>
}

//#include <vector>

bool WiFiGenericClass::_persistent = true;
bool WiFiGenericClass::_long_range = false;
rt_wlan_mode_t WiFiGenericClass::_forceSleepLastMode = WIFI_MODE_NULL;
struct rt_wlan_device *WiFiGenericClass::_bkWifiWlan = NULL;
struct rt_wlan_device WiFiGenericClass::_bkWlanDevice;
struct beken_wifi_info WiFiGenericClass::_bkWlanInfo;

static int bk_wlan_event_handle(enum rw_evt_type event, void *data){
  if ((event < 0) || (event > RW_EVT_MAX))
  {
      return -RT_ERROR;
  }
  switch(event)
  {
      case RW_EVT_STA_CONNECTED:
        break;
      case RW_EVT_STA_DISCONNECTED:
        break;
      case RW_EVT_STA_CONNECT_FAILED:
        break;
      case RW_EVT_AP_CONNECTED:
        break;
      case RW_EVT_AP_DISCONNECTED:
        break;
      case RW_EVT_AP_CONNECT_FAILED:
        break;
  }
  return RT_EOK;
}

WiFiGenericClass::WiFiGenericClass()
{
  memset(&_bkWlanDevice, 0, sizeof(struct rt_wlan_device));
  memset(&_bkWlanInfo, 0, sizeof(struct beken_wifi_info));
}

int WiFiGenericClass::setStatusBits(int bits){
  return 0;
}

int WiFiGenericClass::clearStatusBits(int bits){
  return 0;
}

int WiFiGenericClass::getStatusBits(){
  return 0;
}

int WiFiGenericClass::waitStatusBits(int bits, uint32_t timeout_ms){
  return 0;
}

/**
 * set callback function
 * @param cbEvent WiFiEventCb
 * @param event optional filter (WIFI_EVENT_MAX is all events)
 */
int WiFiGenericClass::onEvent(WiFiEventCb cbEvent, rt_wlan_event_t event)
{
  if(!cbEvent || !_bkWifiWlan) {
      return 0;
  }
  return rt_wlan_register_event_handler(_bkWifiWlan, event, cbEvent);
}

/**
 * removes a callback form event handler
 * @param cbEvent WiFiEventCb
 * @param event optional filter (WIFI_EVENT_MAX is all events)
 */
void WiFiGenericClass::removeEvent(WiFiEventCb cbEvent, rt_wlan_event_t event)
{
  if(!cbEvent || !_bkWifiWlan) {
      return;
  }
  rt_wlan_unregister_event_handler(_bkWifiWlan, event);
}

/**
 * callback for WiFi events
 * @param arg
 */

const char * system_event_names[] = { "WIFI_READY", "SCAN_DONE", "STA_START", "STA_STOP", "STA_CONNECTED", "STA_DISCONNECTED", "STA_AUTHMODE_CHANGE", "STA_GOT_IP", "STA_LOST_IP", "STA_WPS_ER_SUCCESS", "STA_WPS_ER_FAILED", "STA_WPS_ER_TIMEOUT", "STA_WPS_ER_PIN", "AP_START", "AP_STOP", "AP_STACONNECTED", "AP_STADISCONNECTED", "AP_STAIPASSIGNED", "AP_PROBEREQRECVED", "GOT_IP6", "ETH_START", "ETH_STOP", "ETH_CONNECTED", "ETH_DISCONNECTED", "ETH_GOT_IP", "MAX"};

int WiFiGenericClass::_eventCallback(void *arg, rt_wlan_event_t event)
{
  if(event < WIFI_EVT_MAX) {
      WIFI_DEBUG("Event: %d - %s\r\n", event, system_event_names[event]);
  }
  return 0;
}

/**
 * Return the current channel associated with the network
 * @return channel (1-13)
 */
int32_t WiFiGenericClass::channel(void)
{
  uint16_t primaryChan = 0;
  if(!_bkWifiWlan) return primaryChan;
  primaryChan = _bkWifiWlan->info->channel;
  return primaryChan;
}


/**
 * store WiFi config in SDK flash area
 * @param persistent
 */
void WiFiGenericClass::persistent(bool persistent)
{
  _persistent = persistent;
}


/**
 * enable WiFi long range mode
 * @param enable
 */
void WiFiGenericClass::enableLongRange(bool enable)
{
  _long_range = enable;
}


/**
 * set new mode
 * @param m WiFiMode_t
 */
bool WiFiGenericClass::mode(rt_wlan_mode_t m)
{
  if(!_bkWifiWlan) return false;
  rt_wlan_mode_t cm = getMode();
  
  if(cm != m) _bkWifiWlan->info->mode = m;
  rt_wlan_init(_bkWifiWlan, m);
  return true;
}

/**
 * get WiFi mode
 * @return WiFiMode
 */
rt_wlan_mode_t WiFiGenericClass::getMode()
{
  if(!_bkWifiWlan) return WIFI_MODE_NULL;
  return _bkWifiWlan->info->mode;
}

/**
 * control STA mode
 * @param enable bool
 * @return ok
 */
bool WiFiGenericClass::enableSTA(bool enable)
{
  if(rt_device_find("w0") == RT_NULL){
      beken_wlan_hw_init();
  }
  _bkWifiWlan = (struct rt_wlan_device *)rt_device_find("w0");
  rt_wlan_mode_t currentMode = getMode();
  
  bool isEnabled = (currentMode != WIFI_MODE_STATION);
  if(isEnabled && enable) {
      return mode(WIFI_MODE_STATION);
  }
  return true;
}

/**
 * control AP mode
 * @param enable bool
 * @return ok
 */
bool WiFiGenericClass::enableAP(bool enable)
{

  if(rt_device_find("ap") == RT_NULL){
      beken_wlan_hw_init();
  }
  _bkWifiWlan = (struct rt_wlan_device *)rt_device_find("ap");
  rt_wlan_mode_t currentMode = getMode();
  
  bool isEnabled = (currentMode != WIFI_AP);
  if(isEnabled && enable) {
      return mode(WIFI_AP);
  }
  return true;
}

/**
 * control modem sleep when only in STA mode
 * @param enable bool
 * @return ok
 */
bool WiFiGenericClass::setSleep(bool enable)
{
  return false;
}

/**
 * get modem sleep enabled
 * @return true if modem sleep is enabled
 */
bool WiFiGenericClass::getSleep()
{
  return false;
}

/**
 * control wifi tx power
 * @param power enum maximum wifi tx power
 * @return ok
 */
bool WiFiGenericClass::setTxPower(wifi_power_t power){
  return false;
}

wifi_power_t WiFiGenericClass::getTxPower(){
  return WIFI_POWER_19_5dBm;
}


/**
 * Resolve the given hostname to an IP address.
 * @param aHostname     Name to be resolved
 * @param aResult       IPAddress structure to store the returned IP address
 * @return 1 if aIPAddrString was successfully converted to an IP address,
 *          else error code
 */
int WiFiGenericClass::hostByName(const char* aHostname, IPAddress& aResult)
{
  ip_addr_t addr;
  aResult = static_cast<uint32_t>(0);
  err_t err = dns_gethostbyname(aHostname, &addr, RT_NULL, &aResult);
  if(err == ERR_OK && addr.u_addr.ip4.addr) {
        aResult = addr.u_addr.ip4.addr;
  }
  //Serial.println(aResult);
  return (uint32_t)aResult != 0;
}

IPAddress WiFiGenericClass::calculateNetworkID(IPAddress ip, IPAddress subnet) {
    IPAddress networkID;

    for (size_t i = 0; i < 4; i++)
        networkID[i] = subnet[i] & ip[i];

    return networkID;
}

IPAddress WiFiGenericClass::calculateBroadcast(IPAddress ip, IPAddress subnet) {
    IPAddress broadcastIp;
    
    for (int i = 0; i < 4; i++)
        broadcastIp[i] = ~subnet[i] | ip[i];

    return broadcastIp;
}

uint8_t WiFiGenericClass::calculateSubnetCIDR(IPAddress subnetMask) {
    uint8_t CIDR = 0;

    for (uint8_t i = 0; i < 4; i++) {
        if (subnetMask[i] == 0x80)  // 128
            CIDR += 1;
        else if (subnetMask[i] == 0xC0)  // 192
            CIDR += 2;
        else if (subnetMask[i] == 0xE0)  // 224
            CIDR += 3;
        else if (subnetMask[i] == 0xF0)  // 242
            CIDR += 4;
        else if (subnetMask[i] == 0xF8)  // 248
            CIDR += 5;
        else if (subnetMask[i] == 0xFC)  // 252
            CIDR += 6;
        else if (subnetMask[i] == 0xFE)  // 254
            CIDR += 7;
        else if (subnetMask[i] == 0xFF)  // 255
            CIDR += 8;
    }

    return CIDR;
}
