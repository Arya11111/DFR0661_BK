
#include "WiFi.h"


struct rt_wlan_device *_wlan = NULL;

String decToHex(uint8_t num){
  char ch[]={'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
  String str = "";
  //uint8_t sum = num;
  //Serial.println(num);
  if(num < 16){
      str += "0";
      str += ch[num];
      goto LOOP1;
  };

  while(num/16){
      str += ch[num%16];
      num = num/16;
  }
  str += ch[num%16];
  for(uint8_t i = 0; i < str.length()/2; i++){
      char temp = str[i];
      str[i] = str[str.length() - i - 1];
      str[str.length() - i - 1] = temp;
  }
LOOP1:
  return str;
}


WiFiClass WiFi;
