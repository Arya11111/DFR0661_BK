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

#pragma once

#define HSTPIPCFG_PTYPE_BLK    1
#define HSTPIPCFG_PTOKEN_IN    2
#define HSTPIPCFG_PTOKEN_OUT   3
#define HSTPIPCFG_PBK_1_BANK   4
#define HSTPIPCFG_PTYPE_INTRPT 5

#define EP0      0
#define EPX_SIZE 64 // 64 for Full Speed, EPT size max is 1024


#if defined __cplusplus

#include "Stream.h"
#include "RingBuffer.h"

extern "C"{
#include <musb_pcd.h>
#include <rthw.h>
#include <drv_gpio.h>
#include <rtdevice.h>
#include <drv_sys_ctrl.h>
#include <icu_pub.h>
#include <intc_pub.h>
#include <bk_rtos_pub.h>
#include <DFRobot_queue.h>
#include <fal.h>
#include <rt_ota.h>
#include "BkDriverFlash.h"
#include "drv_flash.h"
}


/*状态*/
#define MUSB_STAGE_IDLE         0
#define MUSB_STAGE_SETUP        1//建立
#define MUSB_STAGE_STATUS_IN    2
#define MUSB_STAGE_STATUS_OUT   3
#define MUSB_STAGE_DIN          4//控制数据写过程 host->device
#define MUSB_STAGE_DOUT         5//控制数据读过程 device->host
#define MYUSB_POINT_SIZE        64


//////////////////////////////////////////////////////

#define USB_STANDARD_REQ_TYPE           0
#define USB_CLASS_REQ_TYPE              1
#define USB_VENDOR_REQ_TYPE             2

#define USBD_DEBUG //rt_kprintf
#define USBD_DEBUG1 //rt_kprintf
#define IN_EP_MAX_NUM    16
#define OUT_EP_MAX_NUM   16




#define USB_ENDPOINT_CACHE_SIZE 64
#define USB_ENDPOINT_NUMBERS    16

typedef struct{
  uint8_t len;
  uint8_t index;
  uint8_t buf[USB_ENDPOINT_CACHE_SIZE];
}sUsbEPAcheInfo_t, *pUsbEPAcheInfo_t;

extern sUsbEPAcheInfo_t _usbEPCacheBuffer[USB_ENDPOINT_NUMBERS];
void moveEPCache(uint8_t ep);
uint8_t moveEPCacheLength(uint8_t ep, uint8_t length);
uint8_t epCaheRemainSpace(uint8_t ep);//返回usb端点缓存的剩余空间
uint8_t getEPCacheDataSize(uint8_t ep);
void clearEPCache(uint8_t ep);

uint8_t writeDataToUsbEPCache(uint8_t ep, void *data, uint8_t len);
uint8_t writeRingDataToUsbEPCache(uint8_t ep, void *data, uint16_t &head, uint16_t &tail, uint16_t total);
uint8_t readRingDataFromUsbEPCache(uint8_t ep, void *data, uint16_t &head, uint16_t &tail, uint16_t total);
uint8_t readDataFromUsbEPCache(uint8_t ep, void *data, uint8_t len);

struct dataQueue{
  struct dataQueue *next;
  struct dataQueue *before;
  uint8_t ep_id;
  uint8_t len;
  uint8_t index;
  uint8_t data[];
};

typedef struct dataQueue sDataQueue_t;
typedef struct dataQueue* pDataQueue_t;

extern uint32_t usbd_ep_queueData[16];

void usbDataEnqueue(uint8_t ep_id, uint8_t *pBuf, uint8_t len);
pDataQueue_t usbDataDequeue(uint8_t ep_id);
uint32_t getDataQueueTotalSize(uint8_t ep_id);
pDataQueue_t getDataQueueHead(uint8_t ep_id);


void USB_SetHandler(void (*pf_isr)(void));
typedef struct __attribute__((packed)) {
  union {
      uint8_t request_type; //0x81
      struct {
          uint8_t direction : 5; //1
          uint8_t type : 2;  //0
          uint8_t transferDirection : 1;//1
      };
  };
  uint8_t  bRequest;
  uint16_t wValue;
  uint16_t wIndex;
  uint16_t wLength;
} USBSetup;

//81 06 00 22 00 00 6F 00
//81 06 00 22 00 00 A5 00



typedef struct musb_hpcd_instance{
  musb_instance_t hmusb;
  struct musb_pcd_ep in_ep[IN_EP_MAX_NUM];
  struct musb_pcd_ep out_ep[OUT_EP_MAX_NUM];
  uint8_t address;
  USBSetup setup_pkt;
}sMusb_hpcd_instance_t, *pMusb_hpcd_instance_t;

extern sMusb_hpcd_instance_t USBD;
class USBDeviceClass {
public:
  USBDeviceClass();

  // USB Device API
  void init();
  bool end();
  bool attach();
  bool detach();
  void setAddress(uint32_t addr);

  bool configured();
  bool epFIFOIsNotEmpty(uint8_t ep_addr);
  bool handleClassInterfaceSetup(USBSetup& setup);

    // Control EndPoint API
    uint32_t sendControl(const void *data, uint32_t len);
    uint32_t sendControl(int ep , const void *data, uint32_t len);
    uint32_t recvControl(void *data, uint32_t len);
    bool sendStringDescriptor(const uint8_t *string, uint32_t maxlen);
    void packMessages(bool val, uint16_t index = 0);
  void ISRHandler();

    // Generic EndPoint API
    void initEndpoints(void);
  
  uint8_t send(uint8_t ep);//////
  uint32_t send(uint32_t ep, const void *data, uint32_t len);
  uint32_t recv(uint32_t ep, void *data, uint32_t len);
  uint16_t recv(uint8_t ep, uint8_t *data, uint16_t &head, uint16_t &tail, uint16_t total);
  uint32_t available(uint32_t ep);
  void flush(uint32_t ep);
  

    // private?
  uint32_t armSend(uint32_t ep, const void *data, uint32_t len);
  void parsingSteupPkt(USBSetup &setup);
  void clearEp0RXPacketRdy();
  uint32_t epWrite(uint8_t ep_addr, void *buffer, uint32_t size);

protected:
  void handleStandardRequest(USBSetup &setup);
  void handleFunctionRequest(USBSetup &setup);
  void handleVendorRequest(USBSetup &setup);
  
  void getDescriptor(USBSetup &setup);
  void getDeviceDescriptor(USBSetup &setup);
  void getConfigDescriptor(USBSetup &setup);
  void getStringDescriptor(USBSetup &setup);
  
  uint8_t getInterfacesInfo(uint32_t* total);
  
  void readFIFO(int ep_index, uint8_t * data, uint32_t size);
  void writeFIFO(int ep_index, uint8_t * data, uint32_t size);
  
  uint32_t epReadPrepare(uint32_t ep_addr, void *buffer, uint32_t size);
  int epOpen(uint8_t ep_addr, uint8_t max_packet_size, uint8_t is_iso);
  int epSetStall(uint8_t ep_addr);
  int epClearStall(uint8_t ep_addr);
  

protected:
  uint32_t _tx_irq;
  uint32_t _rx_irq;
  uint32_t _ep_index;
  mihd06t _intr_irq;
  uint8_t ep0_stage;
  uint8_t _config;
  uint16_t _status;
private:
    bool initialized;
};

extern USBDeviceClass USBDevice;

#endif  // __cplusplus
