#include "Arduino.h"
#include "HardwareSerial.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "uart_pub.h"
#include <rtthread.h>
#include "serial.h"

#ifdef __cplusplus
}
#endif

#define RT_DEVICE_VCOM_STATE          0x22

HardwareSerial::HardwareSerial(String name, volatile uint8_t format, volatile uint8_t mode, volatile uint8_t oder)
  :_ser(NULL){
  memset(&_config, 0, sizeof(_config));
  _name = name;
  //memcpy(&(_config.data_bits), &format, 1);
  _config.data_bits = SERIAL_8N1 & HARDSER_DATA_MASK;
  _config.stop_bits = SERIAL_8N1 & HARDSER_STOP_BIT_MASK;
  _config.parity = SERIAL_8N1 & HARDSER_PARITY_MASK;
  _config.bit_order= (SERIAL_ORDER_LSB & 0x01);
  _config.invert = (SERIAL_NRZ_NORMAL & 0x01);
  _config.bufsz = 128;
  rt_kprintf("HardwareSerial\r\n");
  
}
void HardwareSerial::begin(unsigned long baud){
  _config.baud_rate = baud;
  rt_kprintf("baud_rate=%d, data_bits=%d, stop_bits=%d, parity=%d, bit_order=%d, invert=%d, bufsz=%d, reserved=%d\r\n",_config.baud_rate,_config.data_bits,_config.stop_bits,_config.parity,_config.bit_order,_config.invert,_config.bufsz,_config.reserved);
  const char *name = _name.c_str();
  _ser = rt_device_find(name);
  if(!_ser) {
      rt_kprintf("_ser is null\r\n");
      return;
  }else{
      rt_kprintf("_ser is ok\r\n");
  }
  rt_device_open(_ser, RT_DEVICE_FLAG_INT_RX);
  rt_device_control(_ser, RT_DEVICE_CTRL_CONFIG, &_config);
  
}
//void HardwareSerial::begin(unsigned long baudrate, uint16_t config){}
void HardwareSerial::end(){
  if(!_ser) return;
  rt_device_close(_ser);
}
void HardwareSerial::flush(void){
  if(!_ser) return;
  int ret = 1;
  while((ret = rt_device_control(_ser, RT_SERIAL_FLUSH, RT_NULL))){
      rt_kprintf("ret = %d\r\n", ret);
      
  }
  //rt_kprintf("ret1 = %d\r\n", ret);
}
int HardwareSerial::available(void){
  if(!_ser) return 0;
  int count = 0;
  rt_device_control(_ser, FIONREAD, &count);
  //if(count) count -= 2;
  return count;
}
int HardwareSerial::peek(void){
  if(!_ser) return -1;
  rt_serial_peek peek_;

  int ret = 0, data;
  peek_.offset = 0;
  peek_.len = 1;
  peek_.ptr = &data;
  ret = rt_device_control(_ser, RT_SERIAL_PEEK, &peek_);
  if(ret) return data;
  return ret;
}
int HardwareSerial::read(void){
  if(!_ser) return 0;
  int data;
  rt_device_read(_ser, 0, &data, 1);
  return data;
}
size_t HardwareSerial::readBytes( char *buffer, size_t length){
  if(!_ser) return 0;
  size_t count = 0, remain = length;
  char *pBuf = buffer;
  while(remain){
      remain > (SERIAL_RX_BUFFER_SIZE - 1) ? length = SERIAL_RX_BUFFER_SIZE - 1 : length = remain;
      remain -= length;
      //rt_kprintf("length=%d\r\n",length);
      count += rt_device_read(_ser, 0, pBuf, length);
      pBuf += length;
  }
  return count;
}
size_t HardwareSerial::write(uint8_t data){
  if(!_ser) return 0;
  //rt_kprintf("write: data=%d ---------------\r\n", data);
  return rt_device_write(_ser, 0, &data, 1);
}

size_t HardwareSerial::write(const uint8_t *buffer, size_t size){
    //rt_kprintf("aaaaaaaaaaaaaaaaaaaaa\r\n");
  if(!_ser) return 0;
  //rt_kprintf("writebuf: data=%d ---------------\r\n", buffer[0]);
  return rt_device_write(_ser, 0, buffer, size);
}

HardwareSerial::operator bool()
{
  // this is here to avoid spurious opening after upload
  if(_name != "vcom") return true;
  if (millis() < 500)
      return false;

  bool result = false;
  result = rt_device_control(_ser, RT_DEVICE_VCOM_STATE, NULL);

  delay(10);
  return result;
}


HardwareSerial Serial(USB_NAME, SERIAL_8N1, SERIAL_NRZ_NORMAL, SERIAL_ORDER_LSB);
HardwareSerial Serial1(UART1_NAME, SERIAL_8N1, SERIAL_NRZ_NORMAL, SERIAL_ORDER_LSB);
//HardwareSerial1 Serial1(SERIAL_8N1, SERIAL_NRZ_NORMAL, SERIAL_ORDER_LSB);




//HardwareSerial Serial(USB_NAME, SERIAL_8N1, SERIAL_NRZ_NORMAL, SERIAL_ORDER_LSB);

//HardwareSerial Serial2(UART2_NAME, SERIAL_8N1, SERIAL_NRZ_NORMAL, SERIAL_ORDER_LSB);
