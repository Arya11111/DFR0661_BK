#include "WiFi.h"
#include "WiFiScan.h"


bool WiFiScanClass::_scanStarted = false;
bool WiFiScanClass::_scanComplete = false;

static const char *wlan_sec_type_string[] =
{
    "NONE",
    "WEP",
    "WPA-TKIP",
    "WPA-AES",
    "WPA2-TKIP",
    "WPA2-AES",
    "WPA2-MIX",
    "AUTO"
};


int8_t WiFiScanClass::scanNetworks(bool async, bool show_hidden, bool passive, uint32_t max_ms_per_chan){
  if(WiFiScanClass::_scanStarted) {
      return WIFI_SCAN_RUNNING;
  }
  WiFi.enableSTA(true);
  scanDelete();
  if(!WiFi._bkWifiWlan) return WIFI_SCAN_FAILED;
  if(rt_wlan_scan(WiFi._bkWifiWlan, &_scanResult) < 0) return WIFI_SCAN_FAILED;
  else return (int8_t)_scanResult->ap_num;
}

int8_t WiFiScanClass::scanComplete()
{
    if(_scanStarted) {
        return WIFI_SCAN_RUNNING;
    }
    if(_scanComplete && _scanResult) {
        return (int8_t)_scanResult->ap_num;
    }
    return WIFI_SCAN_FAILED;
}

void WiFiScanClass::scanDelete(){
  if(WiFiScanClass::_scanResult){
      WiFiScanClass::_scanResult = NULL;
  }
 WiFiScanClass::_scanComplete = false;
}

bool WiFiScanClass::getNetworkInfo(uint8_t i, String &ssid, uint8_t &encType, int32_t &rssi, uint8_t* &bssid, int32_t &channel)
{
  if(!_scanResult || i >=_scanResult->ap_num) return false;

  ssid = (const char*) _scanResult->ap_table[i].ssid;
  encType = (uint8_t)_scanResult->ap_table[i].security;
  rssi = _scanResult->ap_table[i].rssi;
  bssid = _scanResult->ap_table[i].bssid;
  channel = _scanResult->ap_table[i].channel;
  return true;
}


String WiFiScanClass::SSID(uint8_t networkItem){
  if(!_scanResult || (networkItem >= _scanResult->ap_num)) return String();
  return String(_scanResult->ap_table[networkItem].ssid);
}

wifi_auth_mode_t WiFiScanClass::encryptionType(uint8_t networkItem){
  if(!_scanResult || (networkItem >= _scanResult->ap_num)) return SECURITY_OPEN;
  uint8_t index = 0;
  return _scanResult->ap_table[networkItem].security;
  /*switch((uint32_t)(_scanResult->ap_table[networkItem].security)){
      case SECURITY_OPEN:
           index = 0;
           break;
      case SECURITY_WEP_PSK:
           index = 1;
           break;
      case SECURITY_WPA_TKIP_PSK:
           index = 2;
           break;
      case SECURITY_WPA_AES_PSK:
           index = 3;
           break;
      case SECURITY_WPA2_TKIP_PSK:
           index = 4;
           break;
      case SECURITY_WPA2_AES_PSK:
           index = 5;
           break;
      case SECURITY_WPA2_MIXED_PSK:
           index = 6;
           break;
      default:
           index = 7;
           break;

  }
  return wlan_sec_type_string[index];*/
  //String(wlan_sec_type_string[_scanResult->ap_table[networkItem].security]);
}
String WiFiScanClass::getEncryptionTypeInfo(wifi_auth_mode_t secMode){
  uint8_t index = 0;
  switch((uint32_t)secMode){
      case SECURITY_OPEN:
           index = 0;
           break;
      case SECURITY_WEP_PSK:
           index = 1;
           break;
      case SECURITY_WPA_TKIP_PSK:
           index = 2;
           break;
      case SECURITY_WPA_AES_PSK:
           index = 3;
           break;
      case SECURITY_WPA2_TKIP_PSK:
           index = 4;
           break;
      case SECURITY_WPA2_AES_PSK:
           index = 5;
           break;
      case SECURITY_WPA2_MIXED_PSK:
           index = 6;
           break;
      default:
           index = 7;
           break;

  }
  return wlan_sec_type_string[index];
}

int32_t WiFiScanClass::RSSI(uint8_t networkItem){
  if(!_scanResult || (networkItem >= _scanResult->ap_num)) return 0;
  return _scanResult->ap_table[networkItem].rssi;
}
uint8_t * WiFiScanClass::BSSID(uint8_t networkItem){
  if(!_scanResult || (networkItem >= _scanResult->ap_num)) return NULL;
  return _scanResult->ap_table[networkItem].bssid;
  
}
String WiFiScanClass::BSSIDstr(uint8_t networkItem){
  if(!_scanResult || (networkItem >= _scanResult->ap_num)) return String();
  char mac[18];
  sprintf(mac, "%02x:%02x:%02x:%02x:%02x:%02x", _scanResult->ap_table[networkItem].bssid[0], _scanResult->ap_table[networkItem].bssid[1], _scanResult->ap_table[networkItem].bssid[2], _scanResult->ap_table[networkItem].bssid[3], _scanResult->ap_table[networkItem].bssid[4], _scanResult->ap_table[networkItem].bssid[5]);

  return String(mac);
}
int32_t WiFiScanClass::channel(uint8_t networkItem){
  if(!_scanResult || (networkItem >= _scanResult->ap_num)) return 0;
  return _scanResult->ap_table[networkItem].channel;
}

void WiFiScanClass::_scanDone()
{
  WiFiScanClass::_scanComplete = true;
  WiFiScanClass::_scanStarted = false;
  
}
