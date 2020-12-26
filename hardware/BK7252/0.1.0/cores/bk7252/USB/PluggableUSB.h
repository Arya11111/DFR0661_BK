/*
  PluggableUSB.h
  Copyright (c) 2015 Arduino LLC

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
*/

#ifndef PUSB_h
#define PUSB_h

#include "USBAPI.h"
#include <cstddef>

typedef union{
  struct{
      uint8_t type;
      uint8_t index;
      uint8_t packetSize;
      uint8_t iso;
  }params;
  uint32_t epType;
}endPointPrams_t;

#if defined(USBCON)



/*可插拔USB模块*/
class PluggableUSBModule {
public:
  PluggableUSBModule(uint8_t numEps, uint8_t numIfs, uint32_t *epType) :
    numEndpoints(numEps), numInterfaces(numIfs), endpointType(epType)
  { }//初始化端点数量，接口数量，端点类型

protected:
  virtual bool setup(USBSetup& setup) = 0;
  virtual int getInterface(uint8_t* interfaceCount) = 0;
  virtual int getDescriptor(USBSetup& setup) = 0;
  virtual void handleEndpoint(int /* ep */) { /* Do nothing */ }
  virtual uint8_t getShortName(char *name) { name[0] = 'A'+pluggedInterface; return 1; }

  uint8_t pluggedInterface;//保存的是起始的接口
  uint8_t pluggedEndpoint;//保存起始端点

  const uint8_t numEndpoints;//端点数
  const uint8_t numInterfaces;//接口数
  const uint32_t *endpointType;//端点类型

  PluggableUSBModule *next = NULL;//通过PluggableUSBModule类指向下一个结点

  friend class PluggableUSB_;//将类PluggableUSB_声明为友元函数，类PluggableUSB_的所有成员函数可以访问类PluggableUSBModule的所有成员(包括私有)
};

class PluggableUSB_ {
public:
  PluggableUSB_();
  bool plug(PluggableUSBModule *node);
  int getInterface(uint8_t* interfaceCount);
  int getDescriptor(USBSetup& setup);
  bool setup(USBSetup& setup);
  void handleEndpoint(int ep);
  uint8_t getShortName(char *iSerialNum);

private:
  uint8_t lastIf;  //接口数量
  uint8_t lastEp;  //记录已经使用的端点号，从 0~15 记录最后一个被使用的端点号
  PluggableUSBModule* rootNode;
};

// Replacement for global singleton.
// This function prevents static-initialization-order-fiasco
// https://isocpp.org/wiki/faq/ctors#static-init-order-on-first-use
PluggableUSB_& PluggableUSB();

#endif

#endif
