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
  if((ep & 0x7f) == CDC_ENDPOINT_OUT)
      usb.recv(ep, cdcSerialRecvBuf, headIndex, tailIndex, CDC_SERIAL_BUFFER_SIZE);
  if((ep & 0x7f) == CDC_ENDPOINT_IN){
      rt_enter_critical();
      memset(cdcSerialSendBuf+_cdcTxBufIndex, 0, CDC_SERIAL_TX_BUFFER_SIZE-_cdcTxBufIndex);
      if(CDC_SERIAL_TX_BUFFER_SIZE-_cdcTxBufIndex){
          _cdcTxBufIndex += usb.recv(CDC_ENDPOINT_IN, cdcSerialSendBuf+_cdcTxBufIndex, CDC_SERIAL_TX_BUFFER_SIZE-_cdcTxBufIndex);
      }
      if(_cdcTxBufIndex && (!usb.epFIFOIsNotEmpty(CDC_ENDPOINT_IN))){
          _cdcTxBufIndex -= usb.sendControl(CDC_ENDPOINT_IN,cdcSerialSendBuf, _cdcTxBufIndex);
      }
      rt_exit_critical();
  } //_txRdyFlag = 1;

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
          USBDevice.packMessages(false);
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

Serial_::Serial_(USBDeviceClass &_usb) : PluggableUSBModule(3, 2, epType), usb(_usb), stalled(false),headIndex(0),tailIndex(0),_txRdyFlag(1),_cdcTxBufIndex(0)
{
  endPointPrams_t epArgs;
  memset(cdcSerialRecvBuf,0, CDC_SERIAL_BUFFER_SIZE);
  memset(cdcSerialSendBuf,0, CDC_SERIAL_TX_BUFFER_SIZE);
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
  usb.recv(CDC_ENDPOINT_OUT, cdcSerialRecvBuf, headIndex, tailIndex, CDC_SERIAL_BUFFER_SIZE);
  ret = usb.available(CDC_ENDPOINT_OUT) + (CDC_SERIAL_BUFFER_SIZE - headIndex + tailIndex)%CDC_SERIAL_BUFFER_SIZE;
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
  // if(available()){
      // c = cdcSerialRecvBuf[headIndex];
      // headIndex = (headIndex + 1)%CDC_SERIAL_BUFFER_SIZE;
  // }
  return c;
}

size_t Serial_::read(void *pBuf, size_t size){
  //if(available())
  readBytes((char *)pBuf, size);
}

size_t Serial_::readBytes(char *buffer, size_t length)
{
  size_t size = 0;
  int avai;
  int arraySize = 0;
  uint16_t transferSize = 0;
  uint8_t *pBuf = (uint8_t *)buffer;
  while((avai = available())){
    arraySize = (CDC_SERIAL_BUFFER_SIZE + tailIndex - headIndex)%CDC_SERIAL_BUFFER_SIZE;
    transferSize = (length > arraySize) ? arraySize : length;
    length -= transferSize;
    size += transferSize;
    if(tailIndex < headIndex){
        uint16_t headLen = CDC_SERIAL_BUFFER_SIZE - headIndex;;
        uint16_t tailLen = tailIndex;
        if(transferSize <= headLen) {
            memcpy(pBuf, cdcSerialRecvBuf+headIndex, transferSize);
            pBuf += transferSize;
            headIndex = (headIndex + transferSize)%CDC_SERIAL_BUFFER_SIZE;
        }else{
            memcpy(pBuf, cdcSerialRecvBuf+headIndex, headLen);
            pBuf += headLen;
            headIndex = 0;
            memcpy(pBuf, cdcSerialRecvBuf+headIndex,transferSize - headLen);
            headIndex += (transferSize - headLen);
        }
    }else{
        memcpy(pBuf, cdcSerialRecvBuf+headIndex, transferSize);
        pBuf += transferSize;
        headIndex = (headIndex + transferSize)%CDC_SERIAL_BUFFER_SIZE;
    }
    if(length == 0) break;
  }
  //rt_kprintf("size=%d\n", size);

  return size;
}

void Serial_::flush(void)
{
  usb.flush(CDC_ENDPOINT_IN);
}

size_t Serial_::write(const uint8_t *buffer, size_t size)
{
  if (!(_usbLineInfo.lineState & CDC_LINESTATE_DTR)) return size;
  rt_uint32_t level;
  size_t ret = size;
  uint8_t *tailBuf = NULL;
  uint8_t *pBuf = (uint8_t *)buffer;
  endPointPrams_t epArgs;
  epArgs.epType = epType[2];
  uint16_t len = size;
  rt_enter_critical();
  memset(cdcSerialSendBuf+_cdcTxBufIndex, 0, CDC_SERIAL_TX_BUFFER_SIZE-_cdcTxBufIndex);
  if(CDC_SERIAL_TX_BUFFER_SIZE-_cdcTxBufIndex){
      _cdcTxBufIndex += usb.recv(CDC_ENDPOINT_IN, cdcSerialSendBuf+_cdcTxBufIndex, CDC_SERIAL_TX_BUFFER_SIZE-_cdcTxBufIndex);
      uint8_t cpySize = CDC_SERIAL_TX_BUFFER_SIZE - _cdcTxBufIndex;
      if(cpySize){
          cpySize = ((CDC_SERIAL_TX_BUFFER_SIZE-_cdcTxBufIndex) > size) ? size : CDC_SERIAL_TX_BUFFER_SIZE-_cdcTxBufIndex;
          memcpy(cdcSerialSendBuf+_cdcTxBufIndex, pBuf, cpySize);
          pBuf += cpySize;
          _cdcTxBufIndex += cpySize;
          size -= cpySize;
      }
  }
  len = size;
  
  while(len){
    size = (len > epArgs.params.packetSize) ? epArgs.params.packetSize : len;
    usbDataEnqueue(CDC_ENDPOINT_IN, pBuf,(uint16_t)size);
    pBuf += size;
    len -= size;
  }
  if(_cdcTxBufIndex  && (!usb.epFIFOIsNotEmpty(CDC_ENDPOINT_IN))){
      _cdcTxBufIndex -= usb.sendControl(CDC_ENDPOINT_IN, cdcSerialSendBuf, _cdcTxBufIndex);
  }
  rt_exit_critical();
  return ret;
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


Serial_ SerialUSB(USBDevice);

//#endif
