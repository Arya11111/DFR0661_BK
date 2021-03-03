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

#include <Arduino.h>
#include "CDC.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

//#ifdef CDC_ENABLED


/* For information purpose only since RTS is not always handled by the terminal application */
#define CDC_LINESTATE_DTR       0x01 // Data Terminal Ready
#define CDC_LINESTATE_RTS       0x02 // Ready to Send

#define CDC_LINESTATE_READY     (CDC_LINESTATE_RTS | CDC_LINESTATE_DTR)

typedef struct __attribute__((packed)) {
    uint32_t dwDTERate;
    uint8_t bCharFormat;
    uint8_t bParityType;
    uint8_t bDataBits;
    uint8_t lineState;
} LineInfo;

static volatile LineInfo _usbLineInfo = {
    115200, // dWDTERate
    0x00,   // bCharFormat
    0x00,   // bParityType
    0x08,   // bDataBits
    0x00    // lineState
};

static volatile int32_t breakValue = -1;

// CDC
#define CDC_ACM_INTERFACE  pluggedInterface              // CDC ACM
#define CDC_DATA_INTERFACE uint8_t(pluggedInterface + 1) // CDC Data
#define CDC_ENDPOINT_ACM   pluggedEndpoint
#define CDC_ENDPOINT_OUT   pluggedEndpoint + 1
#define CDC_ENDPOINT_IN    pluggedEndpoint + 2

#define CDC_TX_BUFFER_ACHE  1024

#define CDC_RX CDC_ENDPOINT_OUT
#define CDC_TX CDC_ENDPOINT_IN



int Serial_::getInterface(uint8_t* interfaceNum)
{
    interfaceNum[0] += 2;   // uses 2
    CDCDescriptor _cdcInterface = {
        D_IAD(pluggedInterface, 2, CDC_COMMUNICATION_INTERFACE_CLASS, CDC_ABSTRACT_CONTROL_MODEL, 0),

        // CDC communication interface
        D_INTERFACE(CDC_ACM_INTERFACE, 1, CDC_COMMUNICATION_INTERFACE_CLASS, CDC_ABSTRACT_CONTROL_MODEL, 0),
        D_CDCCS(CDC_HEADER, CDC_V1_10 & 0xFF, (CDC_V1_10>>8) & 0x0FF), // Header (1.10 bcd)

        D_CDCCS4(CDC_ABSTRACT_CONTROL_MANAGEMENT, 6), // SET_LINE_CODING, GET_LINE_CODING, SET_CONTROL_LINE_STATE supported
        D_CDCCS(CDC_UNION, CDC_ACM_INTERFACE, CDC_DATA_INTERFACE), // Communication interface is master, data interface is slave 0
        D_CDCCS(CDC_CALL_MANAGEMENT, 1, 1), // Device handles call management (not)
        D_ENDPOINT(USB_ENDPOINT_IN(CDC_ENDPOINT_ACM), USB_ENDPOINT_TYPE_INTERRUPT, 0x10, 0x10),

        // CDC data interface
        D_INTERFACE(CDC_DATA_INTERFACE, 2, CDC_DATA_INTERFACE_CLASS, 0, 0),
        D_ENDPOINT(USB_ENDPOINT_OUT(CDC_ENDPOINT_OUT), USB_ENDPOINT_TYPE_BULK, EPX_SIZE, 0),
        D_ENDPOINT(USB_ENDPOINT_IN (CDC_ENDPOINT_IN), USB_ENDPOINT_TYPE_BULK, EPX_SIZE, 0)
    };
    return USBDevice.sendControl(&_cdcInterface, sizeof(_cdcInterface));
}

int Serial_::getDescriptor(USBSetup& /* setup */)
{
    return 0;
}

static void utox8(uint32_t val, char* s) {
    for (int i = 0; i < 8; i++) {
        int d = val & 0XF;
        val = (val >> 4);

        s[7 - i] = d > 9 ? 'A' + d - 10 : '0' + d;
    }
}

uint8_t Serial_::getShortName(char* name) {
  // from section 9.3.3 of the datasheet
  #define SERIAL_NUMBER_WORD_0    *(volatile uint32_t*)(0x0080A00C)
  #define SERIAL_NUMBER_WORD_1    *(volatile uint32_t*)(0x0080A040)
  #define SERIAL_NUMBER_WORD_2    *(volatile uint32_t*)(0x0080A044)
  #define SERIAL_NUMBER_WORD_3    *(volatile uint32_t*)(0x0080A048)

  utox8(SERIAL_NUMBER_WORD_0, &name[0]);
  utox8(SERIAL_NUMBER_WORD_1, &name[8]);
  utox8(SERIAL_NUMBER_WORD_2, &name[16]);
  utox8(SERIAL_NUMBER_WORD_3, &name[24]);
  return 32;
}

void Serial_::handleEndpoint(int ep) {
  if((ep & 0x7f) == CDC_ENDPOINT_OUT){
      usb.recv(CDC_ENDPOINT_OUT, cdcSerialRecvBuf, _rxHead, _rxTail, CDC_SERIAL_BUFFER_SIZE);
  }
      
  if((ep & 0x7f) == CDC_ENDPOINT_IN){

  }

}

bool Serial_::setup(USBSetup& setup)
{
  uint8_t r = setup.bRequest;
  uint8_t i = setup.wIndex;
  if(CDC_ACM_INTERFACE != i) return false;
  switch(setup.bRequest){
      case CDC_SEND_ENCAPSULATED_COMMAND:
          USBD_DEBUG("CDC_SEND_ENCAPSULATED_COMMAND\n");
          //USBDevice.clearEp0RXPacketRdy();
          break;
      case CDC_GET_ENCAPSULATED_RESPONSE:
          USBD_DEBUG("CDC_GET_ENCAPSULATED_RESPONSE\n");
          //USBDevice.clearEp0RXPacketRdy();
          break;
      case CDC_SET_COMM_FEATURE:
          USBD_DEBUG("CDC_SET_COMM_FEATURE\n");
          //USBDevice.clearEp0RXPacketRdy();
          break;
      case CDC_GET_COMM_FEATURE:
          USBD_DEBUG("CDC_GET_COMM_FEATURE\n");
          //USBDevice.clearEp0RXPacketRdy();
          break;
      case CDC_CLEAR_COMM_FEATURE:
          USBD_DEBUG("CDC_CLEAR_COMM_FEATURE\n");
          //USBDevice.clearEp0RXPacketRdy();
          break;
      case CDC_SET_LINE_CODING:
          USBD_DEBUG("CDC_SET_LINE_CODING\n");
          
          USBDevice.recvControl((void*)&_usbLineInfo, 7);
          //_usbLineInfo.lineState = 0x80;
          break;
      case CDC_GET_LINE_CODING:
          USBD_DEBUG("CDC_GET_LINE_CODING\n");
          //USBDevice.clearEp0RXPacketRdy();
          //USBDevice.packMessages(false);
          USBDevice.sendControl((void*)&_usbLineInfo, ((setup.wLength > 7) ? 7 : setup.wLength));
          return true;
          break;
      case CDC_SET_CONTROL_LINE_STATE:
          //USBDevice.clearEp0RXPacketRdy();
          USBD_DEBUG("CDC_SET_CONTROL_LINE_STATE\n");
          _usbLineInfo.lineState =(setup.wValue&0xFF);
     
          USBD_DEBUG("vcom state:%d \n",((setup.wValue & 0x01) > 0 ? 1 : 0));
          break;
      case CDC_SEND_BREAK:
          //USBDevice.clearEp0RXPacketRdy();
          breakValue = setup.wValue;
          USBD_DEBUG("CDC_SEND_BREAK\n");
          break;
      default:
          //USBDevice.clearEp0RXPacketRdy();
          USBD_DEBUG("unknown cdc request\n",setup.request_type);
          break;
  }
  if(r == CDC_SET_LINE_CODING || r == CDC_SET_CONTROL_LINE_STATE){
      if (_usbLineInfo.dwDTERate == 1200 && (_usbLineInfo.lineState & CDC_LINESTATE_DTR) == 0){
          rt_hw_cpu_reset();
      }
  }

  return true;
}

Serial_::Serial_(USBDeviceClass &_usb) : PluggableUSBModule(3, 2, epType), usb(_usb), stalled(false),_txRdyFlag(1),_cdcTxBufIndex(0),_rxHead(0), _rxTail(0), _txHead(0), _txTail(0)
{
  endPointPrams_t epArgs;
  memset(cdcSerialRecvBuf,0, CDC_SERIAL_BUFFER_SIZE);
  memset(cdcSerialSendBuf,0, CDC_SERIAL_BUFFER_SIZE);
  epArgs.params.type = USB_ENDPOINT_TYPE_INTERRUPT;
  epArgs.params.index = USB_ENDPOINT_IN(0);
  epArgs.params.packetSize = 8;
  epArgs.params.iso = 0;
  epType[0] = epArgs.epType;
  
  epArgs.params.type = USB_ENDPOINT_TYPE_BULK;
  epArgs.params.index = USB_ENDPOINT_OUT(0);
  epArgs.params.packetSize = 64;
  epType[1] = epArgs.epType;
  
  epArgs.params.index = USB_ENDPOINT_IN(0);
  epType[2] = epArgs.epType;
  
  PluggableUSB().plug(this);
  
}

void Serial_::begin(uint32_t baud_count)
{
  // uart config is ignored in USB-CDC
  _usbLineInfo.dwDTERate = baud_count;
 
}


void Serial_::end(void)
{
    memset((void*)&_usbLineInfo, 0, sizeof(_usbLineInfo));
}

int Serial_::available(void)
{
  int ret = 0;
  uint32_t level;
  uint16_t ringBufferFreeSpace, epBufferDataLen, copyLen;
  level = rt_hw_interrupt_disable();
  ringBufferFreeSpace = CDC_SERIAL_BUFFER_SIZE - (CDC_SERIAL_BUFFER_SIZE - (_rxTail - _rxHead))%CDC_SERIAL_BUFFER_SIZE - 1;
  epBufferDataLen = usb.available(CDC_ENDPOINT_OUT);
  if(ringBufferFreeSpace && epBufferDataLen){
      copyLen= readRingDataFromUsbEPCache(CDC_ENDPOINT_OUT, cdcSerialRecvBuf, _rxHead, _rxTail, CDC_SERIAL_BUFFER_SIZE);
  }
  rt_hw_interrupt_enable(level);
  ringBufferFreeSpace -= copyLen;
  epBufferDataLen -= copyLen;
  ret = epBufferDataLen + (CDC_SERIAL_BUFFER_SIZE - ringBufferFreeSpace - 1);
  return ret;
}

int Serial_::peek(void)
{
  int ret = -1;
  if(available()){
      ret = cdcSerialRecvBuf[headIndex];
  }
  return ret;
}

int Serial_::read(void)
{
  int c = -1;
  readBytes((char *)&c, 1);
  return c;
}

size_t Serial_::read(void *pBuf, size_t size){
  //if(available())
  readBytes((char *)pBuf, size);
}

/*
  uint32_t level;
  level = rt_hw_interrupt_disable();
  
  rt_hw_interrupt_enable(level);
*/
//1.先看环形缓冲区有没有数据
size_t Serial_::readBytes(char *buffer, size_t length)
{
  if((buffer == NULL) || (length == 0)) return 0;
  uint8_t *pBuf = (uint8_t *)buffer;
  uint16_t ringBufDataLen, epBufDataLen, actCopyLen = 0;
  size_t ret = 0;
  uint32_t level;
  
  while(length){
      level = rt_hw_interrupt_disable();
      ringBufDataLen = (CDC_SERIAL_BUFFER_SIZE + _rxTail - _rxHead)%CDC_SERIAL_BUFFER_SIZE;
      if(ringBufDataLen){
          actCopyLen = readDataFromRingBuffer(cdcSerialRecvBuf, _rxHead, _rxTail, CDC_SERIAL_BUFFER_SIZE, pBuf, length);
          pBuf += actCopyLen;
          ret += actCopyLen;
          length -= actCopyLen;
      }
      rt_hw_interrupt_enable(level);
      available();
  }
  return ret;

}

void Serial_::flush(void)
{
  usb.flush(CDC_ENDPOINT_IN);
  clearEPCache(CDC_ENDPOINT_IN);
  memset(cdcSerialSendBuf, 0, CDC_SERIAL_BUFFER_SIZE);
}


//1.先查看剩余空间，如果不够，就移除
size_t Serial_::write(const uint8_t *buffer, size_t size)
{
  if((!(_usbLineInfo.lineState & CDC_LINESTATE_DTR)) || buffer == NULL || size == 0){
       return 0;
  }//if cdc com is close, return
  size_t r = usb.send(CDC_ENDPOINT_IN, buffer, size);
  if(r) return r;
  else return 0;
}

size_t Serial_::write(uint8_t c) {
    return write(&c, 1);
}

// This operator is a convenient way for a sketch to check whether the
// port has actually been configured and opened by the host (as opposed
// to just being connected to the host).  It can be used, for example, in
// setup() before printing to ensure that an application on the host is
// actually ready to receive and display the data.
// We add a short delay before returning to fix a bug observed by Federico
// where the port is configured (lineState != 0) but not quite opened.
Serial_::operator bool()
{
    // this is here to avoid spurious opening after upload
    if (millis() < 500)
        return false;

    bool result = false;

    if (_usbLineInfo.lineState & CDC_LINESTATE_DTR)
    {
        result = true;
    }
    delay(10);
    return result;
}


unsigned long Serial_::baud() {
    return _usbLineInfo.dwDTERate;
}

uint8_t Serial_::stopbits() {
    return _usbLineInfo.bCharFormat;
}

uint8_t Serial_::paritytype() {
    return _usbLineInfo.bParityType;
}

uint8_t Serial_::numbits() {
    return _usbLineInfo.bDataBits;
}

bool Serial_::dtr() {
    return ((_usbLineInfo.lineState & CDC_LINESTATE_DTR) == CDC_LINESTATE_DTR);
}

bool Serial_::rts() {
    return ((_usbLineInfo.lineState & CDC_LINESTATE_RTS) == CDC_LINESTATE_RTS);
}

uint16_t Serial_::writeBufToRingBuffer(uint8_t *ringBuf, uint16_t &head, uint16_t &tail, uint16_t total, uint8_t *data, uint16_t size){
  uint16_t ringRemain = total - (total + tail - head)%total - 1;
  if(ringBuf == NULL || data == NULL || ringRemain == 0 || size ==0) return 0;
  uint16_t actLen = ringRemain > size ? size : ringRemain;
  uint8_t *pBuf = data;
  if(tail < head || head == 0){
      memcpy(ringBuf+tail, pBuf, actLen);
      tail = (tail + actLen)%total;
  }else{
      uint16_t headLen = head - 1;
      uint16_t tailLen = total - tail;
      if(actLen > tailLen){
           memcpy(ringBuf+tail, pBuf, tailLen);
           pBuf += tailLen;
           memcpy(ringBuf, pBuf, actLen - tailLen);
           tail = actLen - tailLen;
      }else{
          memcpy(ringBuf+tail, pBuf, actLen);
          tail = (tail + actLen)%total;
      }
  }
  return actLen;
}

uint16_t Serial_::readDataFromRingBuffer(uint8_t *ringBuf, uint16_t &head, uint16_t &tail, uint16_t total, uint8_t *data, uint16_t size){
  uint16_t ringLen = (total + tail - head)%total;
  if(ringBuf == NULL || data == NULL || ringLen == 0 || size ==0) return 0;
  uint16_t actLen = ringLen > size ? size : ringLen;
  uint8_t *pBuf = data;
  if((head < tail) || (tail == 0)){
      memcpy(pBuf, ringBuf+head, actLen);
      head = (head+actLen)%total;
  }else{
      uint16_t headLen = total - head;
      uint16_t tailLen = tail;
      if(actLen > headLen){
          memcpy(pBuf, ringBuf+head, headLen);
          pBuf += headLen;
          memcpy(pBuf, ringBuf, actLen - headLen);
          head = actLen - headLen;
      }else{
          memcpy(pBuf, ringBuf+head, actLen);
          head = (head + actLen)%total;
      }
  }
  return actLen;
}


Serial_ SerialUSB(USBDevice);

//#endif
