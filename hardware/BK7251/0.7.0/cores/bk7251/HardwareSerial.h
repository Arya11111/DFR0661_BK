/*
  Copyright (c) 2015 Arduino LLC.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef HardwareSerial_h
#define HardwareSerial_h

#include <Arduino.h>
#include <inttypes.h>
#include "Stream.h"
#include "RingBuffer.h"

#define HARDSER_PARITY_EVEN  0<<6
#define HARDSER_PARITY_ODD   1<<6
#define HARDSER_PARITY_NONE  2<<6
#define HARDSER_PARITY_MASK  0xC0

#define HARDSER_STOP_BIT_1      0<<4
#define HARDSER_STOP_BIT_2      1<<4
#define HARDSER_STOP_BIT_3      2<<4
#define HARDSER_STOP_BIT_4      3<<4
#define HARDSER_STOP_BIT_MASK   0x30

#define HARDSER_DATA_5      5
#define HARDSER_DATA_6      6
#define HARDSER_DATA_7      7
#define HARDSER_DATA_8      8
#define HARDSER_DATA_9      9
#define HARDSER_DATA_MASK   0x0F

#define SERIAL_ORDER_LSB       0
#define SERIAL_ORDER_MSB       1

#define SERIAL_NRZ_NORMAL      0
#define SERIAL_NRZ_INVERTED    1 

#define SERIAL_5N1  (HARDSER_STOP_BIT_1 | HARDSER_PARITY_NONE | HARDSER_DATA_5)
#define SERIAL_6N1  (HARDSER_STOP_BIT_1 | HARDSER_PARITY_NONE | HARDSER_DATA_6)
#define SERIAL_7N1  (HARDSER_STOP_BIT_1 | HARDSER_PARITY_NONE | HARDSER_DATA_7)
#define SERIAL_8N1  (HARDSER_STOP_BIT_1 | HARDSER_PARITY_NONE | HARDSER_DATA_8)
#define SERIAL_9N1  (HARDSER_STOP_BIT_1 | HARDSER_PARITY_NONE | HARDSER_DATA_9)
#define SERIAL_5N2  (HARDSER_STOP_BIT_2 | HARDSER_PARITY_NONE | HARDSER_DATA_5)
#define SERIAL_6N2  (HARDSER_STOP_BIT_2 | HARDSER_PARITY_NONE | HARDSER_DATA_6)
#define SERIAL_7N2  (HARDSER_STOP_BIT_2 | HARDSER_PARITY_NONE | HARDSER_DATA_7)
#define SERIAL_8N2  (HARDSER_STOP_BIT_2 | HARDSER_PARITY_NONE | HARDSER_DATA_8)
#define SERIAL_9N2  (HARDSER_STOP_BIT_2 | HARDSER_PARITY_NONE | HARDSER_DATA_9)
#define SERIAL_5N3  (HARDSER_STOP_BIT_3 | HARDSER_PARITY_NONE | HARDSER_DATA_5)
#define SERIAL_6N3  (HARDSER_STOP_BIT_3 | HARDSER_PARITY_NONE | HARDSER_DATA_6)
#define SERIAL_7N3  (HARDSER_STOP_BIT_3 | HARDSER_PARITY_NONE | HARDSER_DATA_7)
#define SERIAL_8N3  (HARDSER_STOP_BIT_3 | HARDSER_PARITY_NONE | HARDSER_DATA_8)
#define SERIAL_9N3  (HARDSER_STOP_BIT_3 | HARDSER_PARITY_NONE | HARDSER_DATA_9)
#define SERIAL_5N4  (HARDSER_STOP_BIT_4 | HARDSER_PARITY_NONE | HARDSER_DATA_5)
#define SERIAL_6N4  (HARDSER_STOP_BIT_4 | HARDSER_PARITY_NONE | HARDSER_DATA_6)
#define SERIAL_7N4  (HARDSER_STOP_BIT_4 | HARDSER_PARITY_NONE | HARDSER_DATA_7)
#define SERIAL_8N4  (HARDSER_STOP_BIT_4 | HARDSER_PARITY_NONE | HARDSER_DATA_8)
#define SERIAL_9N4  (HARDSER_STOP_BIT_4 | HARDSER_PARITY_NONE | HARDSER_DATA_9)
#define SERIAL_5E1  (HARDSER_STOP_BIT_1 | HARDSER_PARITY_EVEN | HARDSER_DATA_5)
#define SERIAL_6E1  (HARDSER_STOP_BIT_1 | HARDSER_PARITY_EVEN | HARDSER_DATA_6)
#define SERIAL_7E1  (HARDSER_STOP_BIT_1 | HARDSER_PARITY_EVEN | HARDSER_DATA_7)
#define SERIAL_8E1  (HARDSER_STOP_BIT_1 | HARDSER_PARITY_EVEN | HARDSER_DATA_8)
#define SERIAL_9E1  (HARDSER_STOP_BIT_1 | HARDSER_PARITY_EVEN | HARDSER_DATA_9)
#define SERIAL_5E2  (HARDSER_STOP_BIT_2 | HARDSER_PARITY_EVEN | HARDSER_DATA_5)
#define SERIAL_6E2  (HARDSER_STOP_BIT_2 | HARDSER_PARITY_EVEN | HARDSER_DATA_6)
#define SERIAL_7E2  (HARDSER_STOP_BIT_2 | HARDSER_PARITY_EVEN | HARDSER_DATA_7)
#define SERIAL_8E2  (HARDSER_STOP_BIT_2 | HARDSER_PARITY_EVEN | HARDSER_DATA_8)
#define SERIAL_9E2  (HARDSER_STOP_BIT_2 | HARDSER_PARITY_EVEN | HARDSER_DATA_9)
#define SERIAL_5E3  (HARDSER_STOP_BIT_3 | HARDSER_PARITY_EVEN | HARDSER_DATA_5)
#define SERIAL_6E3  (HARDSER_STOP_BIT_3 | HARDSER_PARITY_EVEN | HARDSER_DATA_6)
#define SERIAL_7E3  (HARDSER_STOP_BIT_3 | HARDSER_PARITY_EVEN | HARDSER_DATA_7)
#define SERIAL_8E3  (HARDSER_STOP_BIT_3 | HARDSER_PARITY_EVEN | HARDSER_DATA_8)
#define SERIAL_9E3  (HARDSER_STOP_BIT_3 | HARDSER_PARITY_EVEN | HARDSER_DATA_9)
#define SERIAL_5E4  (HARDSER_STOP_BIT_4 | HARDSER_PARITY_EVEN | HARDSER_DATA_5)
#define SERIAL_6E4  (HARDSER_STOP_BIT_4 | HARDSER_PARITY_EVEN | HARDSER_DATA_6)
#define SERIAL_7E4  (HARDSER_STOP_BIT_4 | HARDSER_PARITY_EVEN | HARDSER_DATA_7)
#define SERIAL_9E4  (HARDSER_STOP_BIT_4 | HARDSER_PARITY_EVEN | HARDSER_DATA_9)
#define SERIAL_8E4  (HARDSER_STOP_BIT_4 | HARDSER_PARITY_EVEN | HARDSER_DATA_8)
#define SERIAL_5O1  (HARDSER_STOP_BIT_1 | HARDSER_PARITY_ODD  | HARDSER_DATA_5)
#define SERIAL_6O1  (HARDSER_STOP_BIT_1 | HARDSER_PARITY_ODD  | HARDSER_DATA_6)
#define SERIAL_7O1  (HARDSER_STOP_BIT_1 | HARDSER_PARITY_ODD  | HARDSER_DATA_7)
#define SERIAL_8O1  (HARDSER_STOP_BIT_1 | HARDSER_PARITY_ODD  | HARDSER_DATA_8)
#define SERIAL_9O1  (HARDSER_STOP_BIT_1 | HARDSER_PARITY_ODD  | HARDSER_DATA_9)
#define SERIAL_5O2  (HARDSER_STOP_BIT_2 | HARDSER_PARITY_ODD  | HARDSER_DATA_5)
#define SERIAL_6O2  (HARDSER_STOP_BIT_2 | HARDSER_PARITY_ODD  | HARDSER_DATA_6)
#define SERIAL_7O2  (HARDSER_STOP_BIT_2 | HARDSER_PARITY_ODD  | HARDSER_DATA_7)
#define SERIAL_8O2  (HARDSER_STOP_BIT_2 | HARDSER_PARITY_ODD  | HARDSER_DATA_8)
#define SERIAL_9O2  (HARDSER_STOP_BIT_2 | HARDSER_PARITY_ODD  | HARDSER_DATA_9)
#define SERIAL_5O3  (HARDSER_STOP_BIT_3 | HARDSER_PARITY_ODD  | HARDSER_DATA_5)
#define SERIAL_6O3  (HARDSER_STOP_BIT_3 | HARDSER_PARITY_ODD  | HARDSER_DATA_6)
#define SERIAL_7O3  (HARDSER_STOP_BIT_3 | HARDSER_PARITY_ODD  | HARDSER_DATA_7)
#define SERIAL_8O3  (HARDSER_STOP_BIT_3 | HARDSER_PARITY_ODD  | HARDSER_DATA_8)
#define SERIAL_9O3  (HARDSER_STOP_BIT_3 | HARDSER_PARITY_ODD  | HARDSER_DATA_9)
#define SERIAL_5O4  (HARDSER_STOP_BIT_4 | HARDSER_PARITY_ODD  | HARDSER_DATA_5)
#define SERIAL_6O4  (HARDSER_STOP_BIT_4 | HARDSER_PARITY_ODD  | HARDSER_DATA_6)
#define SERIAL_7O4  (HARDSER_STOP_BIT_4 | HARDSER_PARITY_ODD  | HARDSER_DATA_7)
#define SERIAL_8O4  (HARDSER_STOP_BIT_4 | HARDSER_PARITY_ODD  | HARDSER_DATA_8)
#define SERIAL_9O4  (HARDSER_STOP_BIT_4 | HARDSER_PARITY_ODD  | HARDSER_DATA_9)

//#define USB_NAME "vcom"
#define USB_NAME "vcom"
#define UART1_NAME "uart1"
//#define UART2_NAME "uart2"

#define SERIAL_RX_BUFFER_SIZE 2048

class HardwareSerial : public Stream
{
  
public:
  HardwareSerial(String name, uint8_t format,  uint8_t mode,  uint8_t oder);
  void begin(unsigned long baud);
  void end();
  int available(void);
  int peek(void);
  int read(void);
  virtual size_t readBytes( char *buffer, size_t length);
  size_t readBytes( uint8_t *buffer, size_t length) { return readBytes((char *)buffer, length); }
  void flush(void);//clear tx fifo
  virtual size_t write(uint8_t);
  inline size_t write(unsigned long n) { return write((uint8_t)n); }
  inline size_t write(long n) { return write((uint8_t)n); }
  inline size_t write(unsigned int n) { return write((uint8_t)n); }
  inline size_t write(int n) { return write((uint8_t)n); }
  virtual size_t write(const uint8_t *buffer, size_t size);
  using Print::write;
  operator bool();
private:
  String _name;
  rt_device_t _ser;
  struct serial_configure _config;
};
extern HardwareSerial Serial;
extern HardwareSerial Serial1;

#endif


