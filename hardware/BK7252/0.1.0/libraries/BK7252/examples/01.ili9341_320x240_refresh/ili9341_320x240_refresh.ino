#include <SPI.h>

#define TFT_DC  GDL_PIN_DC
#define TFT_CS  GDL_PIN_CS
#define TFT_RST GDL_PIN_RES

void sendCommand(uint8_t cmd){
  digitalWrite(TFT_CS, LOW);
  digitalWrite(TFT_DC, LOW);
  SPI.transfer(cmd);
  digitalWrite(TFT_CS, HIGH);
  digitalWrite(TFT_DC, HIGH);
  }
void sendData(uint8_t data){
  digitalWrite(TFT_CS, LOW);
  SPI.transfer(data);
  digitalWrite(TFT_CS, HIGH);
 }
 void sendData16(uint16_t data){
  digitalWrite(TFT_CS, LOW);
  SPI.transfer(data >> 8);
  SPI.transfer(data);
  digitalWrite(TFT_CS, HIGH);
 }



void setup() {
  // put your setup code here, to run once:
  gdlInterfaceInit();
  Serial.begin(115200);
  SPI.begin();
  while(!Serial);
  
  displayInit();
  fillScreen1(0x780F);
}

void loop() {
  // put your main code here, to run repeatedly:
  uint32_t t= millis();
  fillScreen1(0x780F);
  Serial.println(millis()-t);
  t= millis();
  fillScreen1(0x0000);
  Serial.println(millis()-t);
  
}
void displayInit(){
  sendCommand(0xCF);
  sendData(0x00);
  sendData(0xc1);
  sendData(0x30);
  sendCommand(0xED);
  sendData(0x64);
  sendData(0x03);
  sendData(0x12);
  sendData(0x81);

  sendCommand(0xe8);
  sendData(0x85);
  sendData(0x00);
  sendData(0x78);
  sendCommand(0xcb);
  sendData(0x39);
  sendData(0x2c);
  sendData(0x00);
  sendData(0x34);
  sendData(0x02);
  sendCommand(0xf7);
  sendData(0x20);
  sendCommand(0xea);
  sendData(0x00);
  sendData(0x00);

  sendCommand(0xc0);
  sendData(0x10);

  sendCommand(0xc1);
  sendData(0x00);
  sendCommand(0xc5);
  sendData(0x30);
  sendData(0x30);
  
  sendCommand(0xc7);
  sendData(0xb7);
  
  sendCommand(0x3a);
  sendData(0x55);
  sendCommand(0x36);
  sendData(0x08);
  sendCommand(0xb1);
  sendData(0x00);
  sendData(0x1a);
  
  sendCommand(0xb6);
  sendData(0x0a);
  sendData(0xa2);
  sendCommand(0xf2);
  sendData(0x00);
  sendCommand(0x26);
  sendData(0x01);
  
  sendCommand(0xe0);
  sendData(0x0f);
  sendData(0x2a);
  sendData(0x28);
  sendData(0x08);
  sendData(0x0e);
  sendData(0x08);
  sendData(0x54);
  sendData(0xa9);
  sendData(0x43);
  sendData(0x0a);
  sendData(0x0f);
  sendData(0x00);
  sendData(0x00);
  sendData(0x00);
  sendData(0x00);
  
  sendCommand(0xe1);
  sendData(0x00);
  sendData(0x15);
  sendData(0x17);
  sendData(0x07);
  sendData(0x11);
  sendData(0x06);
  sendData(0x2b);
  sendData(0x56);
  sendData(0x3c);
  sendData(0x05);
  sendData(0x10);
  sendData(0x0f);
  sendData(0x3f);
  sendData(0x3f);
  sendData(0x0f);
  
  sendCommand(0x2b);
  sendData(0x00);
  sendData(0x00);
  sendData(0x01);
  sendData(0x3f);
  
  sendCommand(0x2a);
  sendData(0x00);
  sendData(0x00);
  sendData(0x00);
  sendData(0xef);
  sendCommand(0x11);
  delay(120);
  sendCommand(0x29);
}

void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color){

  //rt_kprintf("temp11=0x%x, 0x%x\n", temp1111[0], temp1111[1]);
  sendCommand(0x2A);
  sendData16(x);
  sendData16( x + w -1);
  sendCommand(0x2B);
  sendData16(y);
  sendData16(y+h-1);
  sendCommand(0x2C);
  digitalWrite(TFT_CS, LOW);
  
  for(uint16_t i = 0; i < w; i++){
      for(uint16_t j = 0; j < h; j++){
          SPI.transfer16(color);
      }
  }

  digitalWrite(TFT_CS, HIGH);
  
}

void fillScreen1(uint16_t color){
  fillRect(0,0,240,320,color);
}
