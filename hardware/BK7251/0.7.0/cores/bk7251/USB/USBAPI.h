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

#define HSTPIPCFG_PTYPE_BLK 1
#define HSTPIPCFG_PTOKEN_IN 2
#define HSTPIPCFG_PTOKEN_OUT 3
#define HSTPIPCFG_PBK_1_BANK 4
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

#define USBD_DEBUG rt_kprintf
#define USBD_DEBUG1 rt_kprintf
#define IN_EP_MAX_NUM    16
#define OUT_EP_MAX_NUM   16

void USB_SetHandler(void (*pf_isr)(void));



typedef struct __attribute__((packed)) {
  union {
      uint8_t request_type;
      struct {
          uint8_t direction : 5;
          uint8_t type : 2;
          uint8_t transferDirection : 1;
      };
  };
  uint8_t  bRequest;
  uint16_t wValue;
  uint16_t wIndex;
  uint16_t wLength;
} USBSetup;



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
    bool connected();

    void standby();

    // Setup API
    bool handleStandardSetup(USBSetup &setup);
	bool handleClassInterfaceSetup(USBSetup& setup);
    bool sendDescriptor(USBSetup &setup);

    // Control EndPoint API
    uint32_t sendControl(const void *data, uint32_t len);
    uint32_t sendControl(int /* ep */, const void *data, uint32_t len) { return sendControl(data, len); }
    uint32_t recvControl(void *data, uint32_t len);
    uint32_t sendConfiguration(uint32_t maxlen);
    bool sendStringDescriptor(const uint8_t *string, uint32_t maxlen);
    uint8_t SendInterfaces(uint32_t* total);
    void packMessages(bool val);
  void ISRHandler();

    // Generic EndPoint API
    void initEndpoints(void);
    void initEP(uint32_t ep, uint32_t type);

    uint32_t send(uint32_t ep, const void *data, uint32_t len);
    void sendZlp(uint32_t ep);
    uint32_t recv(uint32_t ep, void *data, uint32_t len);
    int recv(uint32_t ep);
    uint32_t available(uint32_t ep);
    void flush(uint32_t ep);
    void clear(uint32_t ep);
    void stall(uint32_t ep);

    // private?
    uint32_t armSend(uint32_t ep, const void *data, uint32_t len);
    uint8_t armRecvCtrlOUT(uint32_t ep);
    void parsingSteupPkt(USBSetup &setup);

protected:
  void handleStandardRequest(USBSetup &setup);
  void handleFunctionRequest(USBSetup &setup);
  void handleVendorRequest(USBSetup &setup);
  
  void getDescriptor(USBSetup &setup);
  void getDeviceDescriptor(USBSetup &setup);
  void getConfigDescriptor(USBSetup &setup);
  void getStringDescriptor(USBSetup &setup);
  
  uint8_t getInterfacesInfo(uint32_t* total);
  
  void readFifo(int ep_index, uint8_t * data, uint32_t size);
  void writeFifo(int ep_index, uint8_t * data, uint32_t size);
  uint32_t epWrite(uint8_t ep_addr, void *buffer, uint32_t size);
  uint32_t epReadPrepare(uint32_t ep_addr, void *buffer, uint32_t size);
  int epOpen(uint8_t ep_addr, uint8_t max_packet_size, uint8_t is_iso);
  

protected:
  uint32_t _tx_irq;
  uint32_t _rx_irq;
  uint32_t _ep_index;
  mihd06t _intr_irq;
  uint8_t ep0_stage;
private:
    bool initialized;
};

extern USBDeviceClass USBDevice;

#endif  // __cplusplus
