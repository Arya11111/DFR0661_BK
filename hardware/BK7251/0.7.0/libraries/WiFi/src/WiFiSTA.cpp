#include "WiFi.h"
#include "WiFiSTA.h"

wl_status_t WiFiSTAClass::status()
{
  struct beken_wifi_info * wifi_info = (struct beken_wifi_info *)WiFi._bkWifiWlan->user_data;
  //wifi_info 
  wl_status_t status = WL_DISCONNECTED;
  //wifi_info->state = 1;
  switch((uint8_t)(wifi_info->state)){
      case 0:
          status = WL_CONNECTED;
          break;
      case 1:
          
          break;
      case 2:
          status = WL_CONNECT_FAILED;
          break;

  }
  if(status == WL_CONNECTED) WiFi.getIPStatus();
  return status;
}

wl_status_t WiFiSTAClass::begin(char* ssid, char *passphrase, int32_t channel, const uint8_t* bssid, bool connect){
  if(!WiFi.enableSTA(true)){
      WIFI_WLAN_DEBUG("STA enable failed!\r\n");
      return WL_CONNECT_FAILED;
  }
  if(WiFi._bkWifiWlan == NULL) return WL_CONNECT_FAILED;
  struct rt_wlan_info info;
  rt_wlan_info_init(&info, WiFi.getMode(), SECURITY_WPA2_MIXED_PSK, ssid);
  rt_wlan_connect(WiFi._bkWifiWlan, &info, passphrase);
  rt_wlan_info_deinit(&info);
  return status();
}
wl_status_t WiFiSTAClass::begin(){
  return WL_CONNECT_FAILED;
}



bool WiFiSTAClass::reconnect()
{
  if(WiFi._bkWifiWlan == NULL) return false;
  rt_wlan_disconnect(WiFi._bkWifiWlan);
  if(rt_wlan_connect(WiFi._bkWifiWlan, WiFi._bkWifiWlan->info, WiFi._bkWifiWlan->key) == 0) return true;
  else return false;
}

bool WiFiSTAClass::disconnect(bool wifioff, bool eraseap){
  if(WiFi._bkWifiWlan == NULL) return true;
  if(rt_wlan_disconnect(WiFi._bkWifiWlan) == 0){
       memset(&_ipStatus, 0x0, sizeof(IPStatusTypedef));
       return true;
  }else return false;
}

bool WiFiSTAClass::isConnected()
{
  return (status() == WL_CONNECTED);
}

uint8_t WiFiSTAClass::waitForConnectResult()
{
  int i = 0;
  while(status() != WL_CONNECTED && i++ < 100){
      delay(100);
  }
  return status();
}


IPAddress WiFiSTAClass::localIP()
{
  if(WiFi._bkWifiWlan == NULL) return IPAddress();
  getIPStatus();
  IPAddress ip;
  ip.fromString(_ipStatus.ip);
  return ip;
}

uint8_t* WiFiSTAClass::macAddress(uint8_t* mac)
{
  if(WiFi._bkWifiWlan == NULL) return NULL;
  getIPStatus();
  if(mac != NULL){
      for(uint8_t i = 0; i < 6; i++){
          mac[i] = (uint8_t)((unsigned char*)_ipStatus.mac)[i];
      }
  }
  return mac;
}

String WiFiSTAClass::macAddress(){
  if(WiFi._bkWifiWlan == NULL) return String();
  getIPStatus();
  String str="";
  //str += _ipStatus.mac[0] + ":";//+ _ipStatus.mac[1]+":"+_ipStatus.mac[2]+":"+_ipStatus.mac[3]+":"+_ipStatus.mac[4]+":"+_ipStatus.mac[5];
  for(int i = 0; i < 6; i++){
      str += decToHex((uint8_t)((unsigned char*)_ipStatus.mac)[i]);
      if(i != 5) str += ":";
  }
  
  return str;
}
// char * WiFiSTAClass::subnetMask(){
  // return _ipStatus.mask;
// }

IPAddress WiFiSTAClass::subnetMask()
{
  if(WiFi._bkWifiWlan == NULL) return IPAddress();
  getIPStatus();
  IPAddress mask;
  mask.fromString(_ipStatus.mask);
  return mask;
}

// char * WiFiSTAClass::gatewayIP(){
  // return _ipStatus.gate;
// }

IPAddress WiFiSTAClass::gatewayIP()
{
  if(WiFi._bkWifiWlan == NULL) return IPAddress();
  getIPStatus();
  IPAddress gw;
  gw.fromString(_ipStatus.gate);
  return gw;
}

IPAddress WiFiSTAClass::dnsIP(uint8_t dns_no)
{
  ip_addr_t *dns_ip = (ip_addr_t *)dns_getserver(dns_no);
  return IPAddress(dns_ip->u_addr.ip4.addr);
}

IPAddress WiFiSTAClass::broadcastIP()
{
  if(WiFi._bkWifiWlan == NULL) return IPAddress();
  getIPStatus();
  IPAddress ip = gatewayIP();
  IPAddress subnet = subnetMask();
  return WiFi.calculateBroadcast(ip, subnet);
}

IPAddress WiFiSTAClass::networkID()
{
  if(WiFi._bkWifiWlan == NULL) return IPAddress();
  getIPStatus();
  IPAddress ip = gatewayIP();
  IPAddress subnet = subnetMask();
  return WiFi.calculateNetworkID(ip, subnet);
}

uint8_t WiFiSTAClass::subnetCIDR()
{
  if(WiFi._bkWifiWlan == NULL) return 0;
  getIPStatus();
  IPAddress subnet = subnetMask();
  return WiFi.calculateSubnetCIDR(subnet);
}

String WiFiSTAClass::SSID(){
  if(WiFi._bkWifiWlan == NULL) return String();
  getIPStatus();
  String str = "";
  struct wlan_fast_connect ap_info;
  memset(&ap_info, 0, sizeof(struct wlan_fast_connect));
  if (wlan_fast_connect_info_read(&ap_info) == RT_EOK){
       for(uint8_t i = 0; i < 32; i++){
           if((char )ap_info.ssid[i] == '\0') break;
           str += (char )ap_info.ssid[i];
       }
       
  }
  return str;
}

String WiFiSTAClass::psk(){
  if(WiFi._bkWifiWlan == NULL) return String();
  String str = "";
  struct wlan_fast_connect ap_info;
   memset(&ap_info, 0, sizeof(struct wlan_fast_connect));
   if (wlan_fast_connect_info_read(&ap_info) == RT_EOK){
       for(uint8_t i = 0; i < 32; i++){
           str += decToHex((uint8_t)ap_info.psk[i]);
       }
       
   }
   return str;
}
uint8_t * WiFiSTAClass::BSSID(){
  if(WiFi._bkWifiWlan == NULL) return NULL;
  struct wlan_fast_connect ap_info;
  memset(&ap_info, 0, sizeof(struct wlan_fast_connect));
  if (wlan_fast_connect_info_read(&ap_info) == RT_EOK){
      return (uint8_t *)ap_info.bssid;
  }else return NULL;
}

int8_t WiFiSTAClass::RSSI(){
  if(WiFi._bkWifiWlan == NULL) return 0;
  return rt_wlan_get_rssi(WiFi._bkWifiWlan);
}

String WiFiSTAClass::BSSIDstr(){
  if(WiFi._bkWifiWlan == NULL) return String();
  String str = "";
  struct wlan_fast_connect ap_info;
  memset(&ap_info, 0, sizeof(struct wlan_fast_connect));
  if (wlan_fast_connect_info_read(&ap_info) == RT_EOK){
       for(uint8_t i = 0; i < 6; i++){
           str += decToHex((uint8_t)ap_info.bssid[i]);
       }
  }
  return str;
}

void WiFiSTAClass::getIPStatus(){
  if(WiFi._bkWifiWlan == NULL) return;
  memset(&_ipStatus, 0x0, sizeof(IPStatusTypedef));
  bk_wlan_get_ip_status(&_ipStatus, STATION);
}
