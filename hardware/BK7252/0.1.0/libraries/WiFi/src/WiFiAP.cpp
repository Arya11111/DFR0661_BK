#include "WiFiAP.h"
#include "WiFi.h"
extern "C" {
#include "wlan_mgnt.h"
}
bool WiFiAPClass::softAP(char* ssid, char* passphrase, int channel, int ssid_hidden, int max_connection){
  int result;
  struct rt_wlan_info info;
  if(!WiFi.enableAP(true)){
      WIFI_WLAN_DEBUG("AP enable failed!\r\n");
      return false;
  }
  if(WiFi._bkWifiWlan == NULL) return false;

  if(passphrase == NULL)
     rt_wlan_info_init(&info, WIFI_AP, SECURITY_OPEN, ssid);
  else
      rt_wlan_info_init(&info, WIFI_AP, SECURITY_WPA2_AES_PSK, ssid);
  info.channel = channel;
  result = rt_wlan_init(WiFi._bkWifiWlan, WIFI_AP);
  result = rt_wlan_softap(WiFi._bkWifiWlan, &info, passphrase);
  if(result == 0) return true;
  else return false;
}

bool WiFiAPClass::softAPConfig(String local_ip, String gateway, String subnet){
  //_wlanAp.
  return true;
}
// bool softAPConfig(IPAddress local_ip, IPAddress gateway, IPAddress subnet){
  
// }

bool WiFiAPClass::softAPdisconnect(){
  if(WiFi._bkWifiWlan == NULL) return false;
  if(rt_wlan_disconnect(WiFi._bkWifiWlan) == 0) return true;
  else return false;
}

uint8_t WiFiAPClass::softAPgetStationNum()
{
  return rt_wlan_get_stationNums();
}

IPAddress WiFiAPClass::softAPIP(){
  IPStatusTypedef ipStatus;
  IPAddress ip;
  if(WiFi._bkWifiWlan == NULL) return IPAddress();
  memset(&ipStatus, 0x0, sizeof(IPStatusTypedef));
  bk_wlan_get_ip_status(&ipStatus, SOFT_AP);
  ip.fromString(ipStatus.ip);
  return ip;
}

IPAddress WiFiAPClass::softAPBroadcastIP(){
  IPStatusTypedef ipStatus;
  IPAddress ip,subnet;
  if(WiFi._bkWifiWlan == NULL) return IPAddress();
  memset(&ipStatus, 0x0, sizeof(IPStatusTypedef));
  bk_wlan_get_ip_status(&ipStatus, SOFT_AP);

  ip.fromString(ipStatus.ip);
  subnet.fromString(ipStatus.mask);
  return WiFi.calculateBroadcast(ip, subnet);
}
IPAddress WiFiAPClass::softAPNetworkID(){
  IPStatusTypedef ipStatus;
  IPAddress ip,subnet;
  if(WiFi._bkWifiWlan == NULL) return IPAddress();
  memset(&ipStatus, 0x0, sizeof(IPStatusTypedef));
  bk_wlan_get_ip_status(&ipStatus, SOFT_AP);
  ip.fromString(ipStatus.ip);
  subnet.fromString(ipStatus.mask);
  return WiFi.calculateNetworkID(ip, subnet);
}

uint8_t WiFiAPClass::softAPSubnetCIDR(){
  IPStatusTypedef ipStatus;
  IPAddress subnet;
  if(WiFi._bkWifiWlan == NULL) return IPAddress();
  memset(&ipStatus, 0x0, sizeof(IPStatusTypedef));
  bk_wlan_get_ip_status(&ipStatus, SOFT_AP);
  subnet.fromString(ipStatus.mask);
  return WiFi.calculateSubnetCIDR(subnet);
}

String WiFiAPClass::softAPmacAddress(void){
  uint8_t hwaddr[6];
  char mac[18];
  if(WiFi._bkWifiWlan == NULL) return String();
  if(rt_wlan_get_mac(WiFi._bkWifiWlan, hwaddr) != 0) return String();
  sprintf(mac, "%02x:%02x:%02x:%02x:%02x:%02x", hwaddr[0], hwaddr[1], hwaddr[2], hwaddr[3], hwaddr[4], hwaddr[5]);
  return String(mac);
}

uint8_t* WiFiAPClass::softAPmacAddress(uint8_t* mac){
  uint8_t hwaddr[6];
  if(WiFi._bkWifiWlan == NULL) return NULL;
  if(rt_wlan_get_mac(WiFi._bkWifiWlan, hwaddr) != 0) return NULL;
  return hwaddr;
}
