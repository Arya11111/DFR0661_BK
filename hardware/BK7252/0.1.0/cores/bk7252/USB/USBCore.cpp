/*
  Copyright (c) 2016 Arduino LLC.  All right reserved.

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

#include "PluggableUSB.h"
#include "USBAPI.h"
#include "USBCore.h"
#include "CDC.h"
#include "UD.h"


#define USB_VID 0x3343
#define USB_PID 0x8244
#define IMANUFACTURER 0x01
#define IPRODUCT      0x02
#define ISERIAL       0x03


sUsbEPAcheInfo_t _usbEPCacheBuffer[USB_ENDPOINT_NUMBERS] = {0};
uint8_t USB_UD_ENABLE_FLAG = 0;
uint8_t USB_UD_TEMPOARY_FLAG = 0;

void moveEPCache(uint8_t ep){
  ep &= 0x7f;
  if(ep >= USB_ENDPOINT_NUMBERS) return;
  uint8_t buf[64];
  uint8_t len = _usbEPCacheBuffer[ep].len - _usbEPCacheBuffer[ep].index;
  if(len){
      memset(buf, 0, 64);
      memcpy(buf, _usbEPCacheBuffer[ep].buf+_usbEPCacheBuffer[ep].index, len);
      memset(&_usbEPCacheBuffer[ep], 0, sizeof(sUsbEPAcheInfo_t));
      memcpy(_usbEPCacheBuffer[ep].buf, buf, len);
      _usbEPCacheBuffer[ep].len = len;
  }else{
      memset(&_usbEPCacheBuffer[ep], 0, sizeof(sUsbEPAcheInfo_t));
  }
}

uint8_t moveEPCacheLength(uint8_t ep, uint8_t length){
  
  ep &= 0x7f;
  if(ep >= USB_ENDPOINT_NUMBERS) return 0;
  if(length > (_usbEPCacheBuffer[ep].len - _usbEPCacheBuffer[ep].index)) return _usbEPCacheBuffer[ep].len - _usbEPCacheBuffer[ep].index;
  _usbEPCacheBuffer[ep].index += length;
  length = _usbEPCacheBuffer[ep].len - _usbEPCacheBuffer[ep].index;
  uint8_t buf[length];
  memset(buf, 0, length);
  memcpy(buf, _usbEPCacheBuffer[ep].buf+_usbEPCacheBuffer[ep].index, length);
  memset(_usbEPCacheBuffer[ep].buf, 0, USB_ENDPOINT_CACHE_SIZE);
  memcpy(_usbEPCacheBuffer[ep].buf, buf, length);
  _usbEPCacheBuffer[ep].index = 0;
  _usbEPCacheBuffer[ep].len = length;
  return _usbEPCacheBuffer[ep].len;
}


uint8_t epCaheRemainSpace(uint8_t ep){
  ep &= 0x7f;
  if(ep >= USB_ENDPOINT_NUMBERS) return 0;
  moveEPCache(ep);
  return USB_ENDPOINT_CACHE_SIZE - _usbEPCacheBuffer[ep].len;
}
uint8_t getEPCacheDataSize(uint8_t ep){
  ep &= 0x7f;
  if(ep >= USB_ENDPOINT_NUMBERS) return 0;
  return (_usbEPCacheBuffer[ep].len - _usbEPCacheBuffer[ep].index);
}

void clearEPCache(uint8_t ep){
  ep &= 0x7f;
  if(ep >= USB_ENDPOINT_NUMBERS) return;
  memset(&_usbEPCacheBuffer[ep], 0, sizeof(sUsbEPAcheInfo_t));
}

uint8_t writeDataToUsbEPCache(uint8_t ep, void *data, uint8_t len){
  ep &= 0x7f;
  //rt_kprintf("fun=%s, %d %d\n", __func__, ep,len);
  if((ep >= USB_ENDPOINT_NUMBERS) || (data == NULL) || (len == 0)) return 0;
  
  moveEPCache(ep);
  uint8_t remainLen = USB_ENDPOINT_CACHE_SIZE - _usbEPCacheBuffer[ep].len + _usbEPCacheBuffer[ep].index;
  //rt_kprintf("%s, %d\n", __func__, remainLen);
  if(remainLen == 0) return 0;
  uint8_t actLen = (len > remainLen) ? remainLen : len;
  uint8_t *pBuf = (uint8_t *)data;
  memset(_usbEPCacheBuffer[ep].buf + _usbEPCacheBuffer[ep].len, 0, remainLen);
  memcpy(_usbEPCacheBuffer[ep].buf + _usbEPCacheBuffer[ep].len, pBuf, actLen);
  _usbEPCacheBuffer[ep].len += actLen;
  return actLen;
}

uint8_t writeRingDataToUsbEPCache(uint8_t ep, void *data, uint16_t &head, uint16_t &tail, uint16_t total){
  ep &= 0x7f;
  if((ep >= USB_ENDPOINT_NUMBERS) ||  (data == NULL) || (total <= head) \
      || (total <= tail) || (((total + tail - head)%total) == 0) \
      || ((_usbEPCacheBuffer[ep].len - _usbEPCacheBuffer[ep].index) == USB_ENDPOINT_CACHE_SIZE)) return 0;
  uint16_t ringBufDataLen = (total + tail - head)%total;
  uint8_t epBufFreeSpace = USB_ENDPOINT_CACHE_SIZE - (_usbEPCacheBuffer[ep].len - _usbEPCacheBuffer[ep].index);
  uint8_t *pBuf = (uint8_t *)data;
  uint8_t actCopyLen = (ringBufDataLen > epBufFreeSpace) ? epBufFreeSpace : ringBufDataLen;
  moveEPCache(ep);
  
  if(head < tail || tail == 0){
      memcpy(_usbEPCacheBuffer[ep].buf + _usbEPCacheBuffer[ep].len, pBuf+head, actCopyLen);
      head = (head + actCopyLen)%total;
      //_usbEPCacheBuffer[ep].len += actLen;
  }else{
      uint16_t headLen = total - head;
      uint16_t tailLen = tail;
      if(actCopyLen > headLen){
          memcpy(_usbEPCacheBuffer[ep].buf + _usbEPCacheBuffer[ep].len, pBuf+head, headLen);
          //_usbEPCacheBuffer[ep].len += headLen;
          memcpy(_usbEPCacheBuffer[ep].buf + _usbEPCacheBuffer[ep].len + headLen, pBuf, actCopyLen - headLen);
          head = actCopyLen - headLen;
          //_usbEPCacheBuffer[ep].len += (actLen - headLen);
      }else{
          memcpy(_usbEPCacheBuffer[ep].buf + _usbEPCacheBuffer[ep].len, pBuf+head, actCopyLen);
          //_usbEPCacheBuffer[ep].len += actLen;
          head = (head + actCopyLen)%total;
      }
  }
  _usbEPCacheBuffer[ep].len += actCopyLen;
  return actCopyLen;
}
//从USB缓冲区中读取数据到环形缓冲区
uint8_t readRingDataFromUsbEPCache(uint8_t ep, void *data, uint16_t &head, uint16_t &tail, uint16_t total){
  ep &= 0x7f;
  if((ep >= USB_ENDPOINT_NUMBERS) ||  (data == NULL) || (total <= head) \
      || (total <= tail) || ((total - (total + tail - head)%total - 1) == 0) \
      || ((_usbEPCacheBuffer[ep].len - _usbEPCacheBuffer[ep].index) == 0)) return 0;
  
  uint16_t ringBufferFreeSpace = total - (total + tail - head)%total - 1;
  uint8_t *pBuf = (uint8_t *)data;
  uint8_t epBufferDataLen = _usbEPCacheBuffer[ep].len - _usbEPCacheBuffer[ep].index;
  uint8_t actCopyLen = (ringBufferFreeSpace > epBufferDataLen) ? epBufferDataLen : ringBufferFreeSpace;

  /*uint16_t ringRemainLen = total - (total + tail - head)%total - 1;
  uint8_t *pBuf = (uint8_t *)data;
  if((ep >= USB_ENDPOINT_NUMBERS) || (data == NULL) || (ringRemainLen == 0) || (_usbEPCacheBuffer[ep].len == 0)) return 0;
  uint8_t epLen = _usbEPCacheBuffer[ep].len - _usbEPCacheBuffer[ep].index;
  if(epLen == 0) return 0;
  uint8_t actLen = (ringRemainLen > epLen) ? epLen : ringRemainLen;*/
  if(tail < head || head == 0){
      memcpy(pBuf+tail, _usbEPCacheBuffer[ep].buf+_usbEPCacheBuffer[ep].index, actCopyLen);
      tail = (tail + actCopyLen)%total;
      //_usbEPCacheBuffer[ep].index += actCopyLen;
  }else{
      uint16_t headLen = head - 1;
      uint16_t tailLen = total - tail;
      if(actCopyLen > tailLen){
          memcpy(pBuf+tail, _usbEPCacheBuffer[ep].buf+_usbEPCacheBuffer[ep].index, tailLen);
          //_usbEPCacheBuffer[ep].index += tailLen;
          memcpy(pBuf, _usbEPCacheBuffer[ep].buf+_usbEPCacheBuffer[ep].index + tailLen, actCopyLen - tailLen);
          tail = (actCopyLen - tailLen);
          //_usbEPCacheBuffer[ep].index += (actCopyLen - tailLen);
      }else{
          memcpy(pBuf+tail, _usbEPCacheBuffer[ep].buf+_usbEPCacheBuffer[ep].index, actCopyLen);
          //_usbEPCacheBuffer[ep].index += actCopyLen;
          tail = (tail + actCopyLen)%total;
      }
  }
  _usbEPCacheBuffer[ep].index += actCopyLen;
  moveEPCache(ep);
  return actCopyLen;
}

uint8_t readDataFromUsbEPCache(uint8_t ep, void *data, uint8_t len){
  ep &= 0x7f;
  if((ep >= USB_ENDPOINT_NUMBERS) || (data == NULL) || (len == 0)) return 0;
  uint8_t epLen = _usbEPCacheBuffer[ep].len - _usbEPCacheBuffer[ep].index;
  if(epLen == 0) return 0;
  uint8_t actLen = (epLen > len) ? len : epLen;
  uint8_t *pBuf = (uint8_t *)data;
  memcpy(pBuf, _usbEPCacheBuffer[ep].buf+_usbEPCacheBuffer[ep].index, actLen);
  _usbEPCacheBuffer[ep].index += actLen;
  if(_usbEPCacheBuffer[ep].index == _usbEPCacheBuffer[ep].len){
      memset(&_usbEPCacheBuffer[ep], 0, sizeof(sUsbEPAcheInfo_t));
  }else{
      moveEPCache(ep);
  }
  return actLen;
}

bool _pack_message = false;

void USB_SetHandler(void (*pf_isr)(void)){
  intc_service_register(IRQ_USB, PRI_IRQ_USB, pf_isr);
  USBD_DEBUG("intc_service_register(IRQ_USB, PRI_IRQ_USB, usbd_irq)\r\n");
}
sMusb_hpcd_instance_t USBD;
/** Pulse generation counters to keep track of the number of milliseconds remaining for each pulse type */
#define TX_RX_LED_PULSE_MS 100
#ifdef PIN_LED_TXL
static volatile uint8_t txLEDPulse; /**< Milliseconds remaining for data Tx LED pulse */
#endif
#ifdef PIN_LED_RXL
static volatile uint8_t rxLEDPulse; /**< Milliseconds remaining for data Rx LED pulse */
#endif

// USB_Handler ISR
extern "C" void UDD_Handler(void) {
  USBDevice.ISRHandler();
}

const uint16_t STRING_LANGUAGE[2] = {
  (3<<8) | (2+2),
  0x0409  // English
};

#ifndef USB_PRODUCT
// If no product is provided, use USB IO Board
#define USB_PRODUCT     "USB IO Board"
#endif

const uint8_t STRING_PRODUCT[] = USB_PRODUCT;

#if USB_VID == 0x2341
#  if defined(USB_MANUFACTURER)
#    undef USB_MANUFACTURER
#  endif
#  define USB_MANUFACTURER "Arduino LLC"
#elif !defined(USB_MANUFACTURER)
// Fall through to unknown if no manufacturer name was provided in a macro
#  define USB_MANUFACTURER "Unknown"
#endif

const uint8_t STRING_MANUFACTURER[] = USB_MANUFACTURER;


//  DEVICE DESCRIPTOR
const DeviceDescriptor USB_DeviceDescriptorB = D_DEVICE(0xEF, 0x02, 0x01, 64, USB_VID, USB_PID, 0x100, IMANUFACTURER, IPRODUCT, ISERIAL, 1);
const DeviceDescriptor USB_DeviceDescriptor = D_DEVICE(0x00, 0x00, 0x00, 64, USB_VID, USB_PID, 0x100, IMANUFACTURER, IPRODUCT, ISERIAL, 1);

//==================================================================
 
volatile uint32_t _usbConfiguration = 0;
volatile uint32_t _usbSetInterface = 0;

uint8_t usbd_ep_cache_buffer[1][512]={0x00};
uint16_t usbd_ep_size[1]={0x00};

uint8_t usbd_ep_out_cache_buffer[1][512]={0x00};
uint16_t usbd_ep_out_size[1]={0x00};
uint8_t usbd_ep_recv_flag[1]={0};

// Some EP are handled using EPHanlders.
// Possibly all the sparse EP handling subroutines will be
// converted into reusable EPHandlers in the future.
//static EPHandler *epHandlers[7] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL};

uint32_t usbd_ep_queueData[16] = {0};
pDataQueue_t usbdEpQueueHead[16] = {NULL};
pDataQueue_t usbdEpQueueTail[16] = {NULL};

#define USB_TEMP_BUFFER_SIZE  256
static uint8_t usbdTempBuffer[USB_TEMP_BUFFER_SIZE] = {0};
static uint8_t usbdTempBufSize = 0;

void usbDataEnqueue(uint8_t ep_id, uint8_t *pBuf, uint8_t len){
  //rt_enter_critical();
  pDataQueue_t p;
  ep_id &= 0x7f;
  if((ep_id & 0x7f) > 16) return;
  p = (pDataQueue_t)malloc(sizeof(sDataQueue_t)+len+1);
  if(p == NULL) return;
  p->next = NULL;
  p->before = NULL;
  if(usbdEpQueueHead[ep_id] == NULL){
      usbdEpQueueHead[ep_id] = p;
      usbdEpQueueTail[ep_id] = p;
      //rt_kprintf("fun1=%s  id=%d len=%d\n", __func__, ep_id, len);
  }else{
      p->before = usbdEpQueueTail[ep_id];
      usbdEpQueueTail[ep_id]->next = p;
      usbdEpQueueTail[ep_id] = p;
      //rt_kprintf("fun=%s  %d\n", __func__, ep_id);
  }
  p->ep_id = ep_id;
  p->len = len;
  p->index = 0;
  memset(p->data,'\0',len+1);
  memcpy(p->data, pBuf, len);
  usbd_ep_queueData[ep_id]++;
  //rt_exit_critical();
}

pDataQueue_t usbDataDequeue(uint8_t ep_id){
  //rt_enter_critical();
  pDataQueue_t p = NULL;
  ep_id &= 0x7f;
  if((ep_id & 0x7f) > 16) return p;
  p = usbdEpQueueHead[ep_id];
  if(usbdEpQueueHead[ep_id] != NULL){
      //rt_kprintf("fun=%s, id=%d len=%d, index=%d\n",__func__, ep_id,p->len,p->index);
      if(p->next != NULL){
          p->next->before = NULL;
      }
      usbdEpQueueHead[ep_id] = p->next;
      usbd_ep_queueData[ep_id]--;
  }
  //rt_kprintf("fun1=%s, id=%d len=%d, index=%d\n",__func__, ep_id,p->len,p->index);
  //rt_exit_critical();
  return p;
}

uint32_t getDataQueueTotalSize(uint8_t ep_id){
  //rt_enter_critical();
  uint32_t ret = 0;
  pDataQueue_t p;
  ep_id &= 0x7f;
  if(((ep_id & 0x7f) > 16) || !usbd_ep_queueData[ep_id]) return ret;
  for(p = usbdEpQueueHead[ep_id]; p != NULL; p = p->next){
      ret += (p->len - p->index);
  }
  //rt_exit_critical();
  return ret;
}

pDataQueue_t getDataQueueHead(uint8_t ep_id){
  //rt_enter_critical();
  pDataQueue_t p = NULL;
  ep_id &= 0x7f;
  p = usbdEpQueueHead[ep_id];
  //rt_exit_critical();
  return p;
}


uint32_t EndPoints[] =
{
    0,

#ifdef PLUGGABLE_USB_ENABLED
    //allocate 9 endpoints and remove const so they can be changed by the user
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
#endif
};
#define EP_ARRAY_SIZE   (sizeof(EndPoints)/sizeof(EndPoints[0]))
USBDeviceClass::USBDeviceClass(){
  memset(&USBD, 0, sizeof(sMusb_hpcd_instance_t));
  _config = 0;
  _status = 0;
}

void USBDeviceClass::init(){
  uint32_t param;
  unsigned char ucUSBIntStatus;
  
#ifdef PIN_LED_TXL
  txLEDPulse = 0;
  pinMode(PIN_LED_TXL, OUTPUT);
  digitalWrite(PIN_LED_TXL, HIGH);
#endif

#ifdef PIN_LED_RXL
  rxLEDPulse = 0;
  pinMode(PIN_LED_RXL, OUTPUT);
  digitalWrite(PIN_LED_RXL, HIGH);
#endif
  memset(&USBD, 0, sizeof(sMusb_hpcd_instance_t));
  USBD.hmusb = get_musb_instance(REG_USB_BASE_ADDR);//get usb register addr
  gpio_usb_second_function();
  USBD_DEBUG("usb gpio init\r\n");
  param = 0;
  sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_USB_POWERUP, &param);
  USBD_DEBUG("power up usb subsystem\r\n");
  /*step 1.0: reset usb module*/
  param = 0;
  sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_USB_SUBSYS_RESET, &param);
  USBD_DEBUG("reset usb module\r\n");
  /*step1.1: open clock*/
  param = BLK_BIT_DPLL_480M | BLK_BIT_USB;
  sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_BLK_ENABLE, &param);
  USBD_DEBUG("BLK_ENABLE\r\n");
  param = MCLK_SELECT_DPLL;
  sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_MCLK_SELECT, &param);
  USBD_DEBUG("MCLK_SELECT\r\n");
  param = USB_DPLL_DIVISION;
  sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_MCLK_DIVISION, &param);
  USBD_DEBUG("MCLK_DIVISION\r\n");
  /*step2: config clock power down for peripheral unit*/
  param = PWD_USB_CLK_BIT;
  sddev_control(ICU_DEV_NAME, CMD_CLK_PWR_UP, &param);
  USBD_DEBUG("PWR_UP\r\n");

  (*((volatile unsigned long *) (0x00804000 + 0x30C))) = 0x01;
  USBD.hmusb->intr_tx_1_e = 0;
  USBD.hmusb->intr_rx_1_e = 0;
  USBD.hmusb->intr_usb_e.suspend = 1;
  USBD.hmusb->intr_usb_e.resume = 1;
  USBD.hmusb->intr_usb_e.babble_reset = 0;
  USBD.hmusb->intr_usb_e.SOF = 0;
  USBD.hmusb->intr_usb_e.connect = 0;
  USBD.hmusb->intr_usb_e.disconnect = 0;
  USBD.hmusb->intr_usb_e.vbus_error = 0;
  REG_AHB2_USB_VTH &= ~(1 << 7);
  
  USBD.hmusb->power.suspend_enable = 0;
  USBD.hmusb->power.iso_update = 0;
  USBD.hmusb->power.resume = 0;
  USBD.hmusb->power.vbus_sess = 1;
  USBD_DEBUG("USBD.hmusb->power\r\n");
  USBD.hmusb->faddr = 0;
  USBD_DEBUG("USBD.hmusb->faddr = 0;\r\n");
  USBD.hmusb->dev_ctl.session = 1;

  (*((volatile unsigned long *) ((0x00800000UL) + 0x18 * 4))) |= (1 << 25);
  USBD.hmusb->intr_tx_1_e = 0x01;
  USBD.hmusb->intr_tx_2_e = 0x01;
  USBD.hmusb->intr_rx_1_e = 0x01;
  USBD.hmusb->intr_rx_2_e = 0x01;
  USBD.hmusb->intr_usb_e.suspend = 1;
  USBD.hmusb->intr_usb_e.resume = 1;
  USBD.hmusb->intr_usb_e.babble_reset = 1;
  USBD.hmusb->intr_usb_e.SOF = 0;
  USBD.hmusb->intr_usb_e.connect = 1;
  USBD.hmusb->intr_usb_e.disconnect = 1;
  USBD.hmusb->intr_usb_e.vbus_error = 0;
  
  REG_AHB2_USB_OTG_CFG = 0x08;
  REG_AHB2_USB_DEV_CFG = 0xF4;
  REG_AHB2_USB_OTG_CFG |= 0x01;
  
  ucUSBIntStatus = REG_AHB2_USB_INT;
  
  rt_thread_delay(1);
  REG_AHB2_USB_INT = ucUSBIntStatus;
  rt_thread_delay(1);
  REG_AHB2_USB_GEN = (0x7<<4) | (0x7<<0);
  
  USB_SetHandler(&UDD_Handler);
  intc_enable(IRQ_USB);
  param = GINTR_IRQ_BIT;
  sddev_control(ICU_DEV_NAME, CMD_ICU_GLOBAL_INT_ENABLE, &param);
  epOpen(0x80,64,0);
  USBD_DEBUG("attach end");
  
  initialized = 1;
 SerialUSB.begin(0);
 UD.begin();
#ifdef CDC_ENABLED
   // SerialUSB.begin(0);
#endif
}

bool USBDeviceClass::end() {
    if (!initialized)
        return false;
    //usbd.disable();
    return true;
}

bool USBDeviceClass::attach()
{
    if (!initialized)
        return false;
    _usbConfiguration = 0;
    return true;
}

bool USBDeviceClass::detach()
{
  if (!initialized)
      return false;
  return true;
}

void USBDeviceClass::setAddress(uint32_t addr)
{
  USBD.hmusb->faddr = addr;
}

bool USBDeviceClass::configured()
{
  return _usbConfiguration != 0;
}

bool USBDeviceClass::connected()
{
  return true;
}

void USBDeviceClass::standby() {}

bool USBDeviceClass::handleClassInterfaceSetup(USBSetup& setup)
{
  //USBD.hmusb->mode.peripheral.csr0_tx_csr1.csr0.serviced_rx_pkt_rdy = 1;
  #if defined(PLUGGABLE_USB_ENABLED)
    bool ret = PluggableUSB().setup(setup);
    if ( ret == false) {
        //sendZlp(0);
    }
    return ret;
#endif

    return false;

}

bool USBDeviceClass::handleStandardSetup(USBSetup &setup)
{
  return true;
}

bool USBDeviceClass::sendDescriptor(USBSetup &setup)
{
  return true;
}



uint32_t USBDeviceClass::sendControl(const void* _data, uint32_t len)
{
  const uint8_t *data = reinterpret_cast<const uint8_t *>(_data);
  uint32_t length = len;
  
  
  if(_pack_message == true){
      memcpy(usbd_ep_cache_buffer[0]+usbd_ep_size[0], data, length);
      USBD_DEBUG("fun=%s, %d\r\n", __func__, usbd_ep_size[0]);
      usbd_ep_size[0] += length;
  }else{
      //usbd_ep_size[0] = 0;
      //memset(usbd_ep_cache_buffer[0],0,512);
      //memcpy(usbd_ep_cache_buffer[0], data, length);
      USBD.in_ep[0].buffer = (uint8_t *)_data;
      USBD.in_ep[0].remain_size = length;
      USBD.in_ep[0].transfer_size = (length > USBD.in_ep[0].max_packet_size) ? USBD.in_ep[0].max_packet_size : length;
      //USBD.hmusb->mode.peripheral.csr0_tx_csr1.csr0.serviced_rx_pkt_rdy = 1;
      len = epWrite(EP0_IN_ADDR,USBD.in_ep[0].buffer, USBD.in_ep[0].transfer_size);
      USBD.in_ep[0].transfer_size = len;
  }
  return length;
}

uint32_t USBDeviceClass::sendControl(int ep , const void *data, uint32_t len){
  //rt_enter_critical();
  uint8_t ep_index = ep & 0x7f;
  USBD.in_ep[ep_index].buffer = (uint8_t *)data;
  USBD.in_ep[ep_index].remain_size = len;
  USBD.in_ep[ep_index].transfer_size = (len > USBD.in_ep[ep_index].max_packet_size) ? USBD.in_ep[ep_index].max_packet_size : len;
  len = epWrite(ep_index,USBD.in_ep[ep_index].buffer, USBD.in_ep[ep_index].transfer_size);
  USBD.in_ep[ep_index].transfer_size = len;
  //rt_exit_critical();
  return len;
}

uint32_t USBDeviceClass::recvControl(void *_data, uint32_t len)
{
  uint8_t *data = reinterpret_cast<uint8_t *>(_data);
  uint32_t index = 0;
  USBD.out_ep[0].buffer = data;
  USBD.out_ep[0].remain_size = len;
  USBD.out_ep[0].transfer_size = 0;
  USBD_DEBUG("fun=%s, 0x%p, 0x%p, 0x%p\n", __func__, _data, data,USBD.out_ep[0].buffer);
  //USBD.hmusb->mode.peripheral.csr0_tx_csr1.csr0.serviced_rx_pkt_rdy = 1;
  if(USBD.hmusb->mode.peripheral.csr0_tx_csr1.csr0.rx_pkt_rdy){
      uint32_t read_len = USBD.hmusb->mode.peripheral.rx_count2;
      read_len <<= 8;
      read_len |= USBD.hmusb->mode.peripheral.count0_rx_count1;
      
      if(USBD.out_ep[0].buffer){
          readFIFO(EP0_OUT_ADDR, USBD.out_ep[0].buffer+USBD.out_ep[0].transfer_size,  read_len);
          USBD_DEBUG("123456\n");
          USBD.out_ep[0].transfer_size += read_len;
          
          USBD.hmusb->mode.peripheral.csr0_tx_csr1.csr0.serviced_rx_pkt_rdy = 1;
          if(USBD.out_ep[0].transfer_size == USBD.out_ep[0].remain_size){
              USBD.out_ep[0].transfer_size = 0;
              USBD.out_ep[0].remain_size = 0;
              USBD.out_ep[0].buffer = 0;
              USBD_DEBUG("78910\n");
              ep0_stage = MUSB_STAGE_SETUP;
              USBD.hmusb->mode.peripheral.csr0_tx_csr1.csr0.data_end = 1;
              USBD.hmusb->mode.peripheral.csr0_tx_csr1.csr0.tx_pkt_rdy = 1;
              //epWrite(EP0_IN_ADDR, NULL, 0);
          }
          
      }
  }
  return 0;
}










uint32_t USBDeviceClass::sendConfiguration(uint32_t maxlen)
{
  return true;
}






// Send a USB descriptor string. The string is stored as a
// plain ASCII string but is sent out as UTF-16 with the
// correct 2-byte prefix
bool USBDeviceClass::sendStringDescriptor(const uint8_t *string, uint32_t maxlen)
{
  return true;
}

uint8_t USBDeviceClass::SendInterfaces(uint32_t* total)
{
  return 0;
}

void USBDeviceClass::packMessages(bool val, uint16_t size)
{
  if (val) {
      _pack_message = true;
      usbd_ep_size[0] = size;
  } else {
      _pack_message = false;
  }
}



void USBDeviceClass::initEndpoints() {
  endPointPrams_t epArgs;
  for (uint8_t i = 1; (i < EP_ARRAY_SIZE) && (EndPoints[i] != 0); i++) {
      //initEP(i, EndPoints[i]);
      epArgs.epType = EndPoints[i];
      epArgs.params.index |= i;
      epOpen(epArgs.params.index, epArgs.params.packetSize, epArgs.params.iso);
      //USBD_DEBUG1("fun=%s, index=%x, size=%d, iso=%d",__func__, epArgs.params.index, epArgs.params.packetSize, epArgs.params.iso);
  }
}

void USBDeviceClass::flush(uint32_t ep)
{
  USBD.hmusb->index = ep & 0x7f;
  USBD.hmusb->mode.peripheral.csr02_tx_csr2.csr02.flush_fifo = 1;//刷新FIFO
}

void USBDeviceClass::clear(uint32_t ep) {
  
}

void USBDeviceClass::stall(uint32_t ep)
{
  
}

// Number of bytes, assumes a rx endpoint
uint32_t USBDeviceClass::available(uint32_t ep)
{
  return getEPCacheDataSize(ep);
}

// Non Blocking receive
// Return number of bytes read
uint32_t USBDeviceClass::recv(uint32_t ep, void *_data, uint32_t len)
{
  /*pDataQueue_t p;
  uint32_t queueSize = getDataQueueTotalSize((uint8_t)ep);
  if(!queueSize) return 0;
  uint32_t actLen = (len > queueSize) ? queueSize : len;
  uint8_t *pBuf = (uint8_t *)_data;
  uint32_t total = 0;
  len = actLen;
  while(actLen){
      
      p = usbdEpQueueHead[ep & 0x7f];
      queueSize = p->len - p->index;
      //if(ep < 4) rt_kprintf("==\n");
      if(queueSize <= actLen) p = usbDataDequeue((uint8_t)ep);
     // if(ep < 4) rt_kprintf("+=\n");
      //rt_kprintf("+++size=%d, act=%d", queueSize, )
      len = (queueSize > actLen) ? actLen : queueSize;
      memcpy(pBuf, p->data+p->index, len);
      pBuf += len;
      p->index += len;
      if(queueSize <= actLen) free(p);
      //if(ep < 4) rt_kprintf("++=\n");
      actLen -= len;
      total += len;
  }
  return total;*/
  uint16_t ret = 0;
  ep &= 0x7f;
  if(ep > USB_ENDPOINT_NUMBERS || _data == NULL) return 0;
  uint8_t *pBuf = (uint8_t *)_data;
  uint8_t actRecvDataLen = USBD.out_ep[ep].remain_size;
  actRecvDataLen = actRecvDataLen > len ? len : actRecvDataLen;
  if(actRecvDataLen)
      readFIFO(ep, pBuf, actRecvDataLen);
  USBD.out_ep[ep].remain_size = 0;
  USBD.hmusb->mode.peripheral.rx_csr_1.rx_pkt_rdy = 0;
  return actRecvDataLen;
  
  
}
//先计算剩余空间，然后比较中断的数据和剩余空间大小，如果剩余不够，就挤掉相关数据
uint16_t USBDeviceClass::recv(uint8_t ep, uint8_t *data, uint16_t &head, uint16_t &tail, uint16_t total){
  uint16_t ret = 0;
  ep &= 0x7f;
  //rt_kprintf("fun=%s, ep=%d, %d\n", __func__,ep, USBD.out_ep[ep].remain_size);
  
  if(ep > USB_ENDPOINT_NUMBERS) return 0;
  uint8_t epBufferFreeSpace = epCaheRemainSpace(ep);//usb cache's free space
  uint8_t actRecvDataLen = USBD.out_ep[ep].remain_size;
  //rt_kprintf("head=%d, tail=%d, total=%d, %d\n", head, tail, total, epBufferFreeSpace);
  if((total + USB_ENDPOINT_CACHE_SIZE) < actRecvDataLen){
      USBD.hmusb->mode.peripheral.rx_csr_1.rx_pkt_rdy = 0;
      return 0;
  }
  if((USBD.out_ep[ep].remain_size == 0) && USBD.hmusb->mode.peripheral.csr0_tx_csr1.csr0.rx_pkt_rdy){
      USBD.hmusb->mode.peripheral.rx_csr_1.rx_pkt_rdy = 0;
      return 0;
  }
  if((data == NULL) || (total <= head) || (total <= tail) || (total < (total+tail-head)%total)){
      if(epBufferFreeSpace < actRecvDataLen){
          _usbEPCacheBuffer[ep].index += (actRecvDataLen - epBufferFreeSpace);
          moveEPCache(ep);
      }
      readFIFO(ep, _usbEPCacheBuffer[ep].buf+_usbEPCacheBuffer[ep].len, actRecvDataLen);
      ret = actRecvDataLen;
      USBD.out_ep[ep].remain_size = 0;
      USBD.hmusb->mode.peripheral.rx_csr_1.rx_pkt_rdy = 0;
      return ret;
  }
  uint16_t ringBufferFreeSpace = total - (total + tail - head)% total - 1;
  //这里只能处理环形缓冲区大于64字节的数据
  if((ringBufferFreeSpace + epBufferFreeSpace) < actRecvDataLen){
      head = (head + (actRecvDataLen - ringBufferFreeSpace - epBufferFreeSpace))%total;
  }
  ringBufferFreeSpace = total - (total + tail - head)% total - 1;
  if(USB_ENDPOINT_CACHE_SIZE - epBufferFreeSpace){
      ret += readRingDataFromUsbEPCache((uint8_t)ep, data, head, tail, total);
  }
  while(USBD.out_ep[ep].remain_size){
      epBufferFreeSpace = epCaheRemainSpace(ep);
      if(epBufferFreeSpace){
          actRecvDataLen = (epBufferFreeSpace > USBD.out_ep[ep].remain_size) ? USBD.out_ep[ep].remain_size : epBufferFreeSpace;
          readFIFO(ep, _usbEPCacheBuffer[ep].buf+_usbEPCacheBuffer[ep].len, actRecvDataLen);
          _usbEPCacheBuffer[ep].len += actRecvDataLen;
          USBD.out_ep[ep].remain_size -= actRecvDataLen;
      }
      ret += readRingDataFromUsbEPCache((uint8_t)ep, data, head, tail, total);
  }
  USBD.hmusb->mode.peripheral.rx_csr_1.rx_pkt_rdy = 0;
  return ret;
}

// Recv 1 byte if ready
int USBDeviceClass::recv(uint32_t ep)
{
  return 0;
}

uint8_t USBDeviceClass::armRecvCtrlOUT(uint32_t ep)
{
  return 0;
}

// Timeout for sends
#define TX_TIMEOUT_MS 70

static char LastTransmitTimedOut[7] = {
  0,
  0,
  0,
  0,
  0,
  0,
  0
};

uint8_t USBDeviceClass::send(uint8_t ep){
  ep &= 0x7f;
  if(ep >= USB_ENDPOINT_NUMBERS) return 0;
  USBD.in_ep[ep].remain_size = 0;
  USBD.in_ep[ep].transfer_size = 0;
  USBD.in_ep[ep].buffer = 0;
  uint8_t len = (uint8_t)epWrite(ep, _usbEPCacheBuffer[ep].buf+_usbEPCacheBuffer[ep].index, _usbEPCacheBuffer[ep].len - _usbEPCacheBuffer[ep].index);
  clearEPCache(ep);
  return len;
}

// Blocking Send of data to an endpoint
uint32_t USBDeviceClass::send(uint32_t ep, const void *data, uint32_t len)
{
  ep &= 0x7f;
  rt_kprintf("fun=%s %d\n", __func__, ep);
  uint8_t *pBuf = (uint8_t *)data;
  if(ep >= USB_ENDPOINT_NUMBERS || data == NULL) return 0;
  USBD.in_ep[ep].buffer = pBuf;
  USBD.in_ep[ep].remain_size = len;
  USBD.in_ep[ep].transfer_size = (len > 64) ? 64 : len;
  len = epWrite(ep,USBD.in_ep[ep].buffer, USBD.in_ep[ep].transfer_size);
  USBD.in_ep[ep].transfer_size = len;
  return len;
  //return sendControl((int)ep , data, len);
  /*uint32_t written = 0;
  uint32_t length = 0;
  uint8_t *pBuf = (uint8_t *)data;
  ep &= 0x7f;
  if(len > 16384) return -1;
  uint8_t jq=0;
  while(len != 0){
      
      if(USBD.hmusb->index != ep) USBD.hmusb->index = ep;
      //rt_kprintf("%d  %d  %d\n", USBD.hmusb->index, jq++,USBD.hmusb->mode.peripheral.csr0_tx_csr1.tx_csr1.fifo_not_empty);
      if(USBD.hmusb->mode.peripheral.csr0_tx_csr1.tx_csr1.fifo_not_empty){
          // previous transfer is still not complete
          uint32_t timeout = microsecondsToClockCycles(70*1000) / 23;
          if(USBD.hmusb->index != ep) USBD.hmusb->index = ep;
          while(USBD.hmusb->mode.peripheral.csr0_tx_csr1.tx_csr1.fifo_not_empty){
              if(timeout-- == 0){
                  USBD.hmusb->mode.peripheral.csr02_tx_csr2.csr02.flush_fifo = 1;
                  return -1;
              }
              if(USBD.hmusb->index != ep) USBD.hmusb->index = ep;
          }
          //clearEPCache(ep);
      }
      length = (len > USB_ENDPOINT_CACHE_SIZE) ? USB_ENDPOINT_CACHE_SIZE : len;
      length = writeDataToUsbEPCache(ep, pBuf, length);
      //rt_kprintf("0x%p, length=%d %d\n", pBuf, length, (uint8_t)len);
      pBuf += length;
      len -= length;
      written += length;
      USBD.in_ep[ep].remain_size = 0;
      USBD.in_ep[ep].transfer_size = 0;
      USBD.in_ep[ep].buffer = 0;
      epWrite(ep, _usbEPCacheBuffer[ep].buf+_usbEPCacheBuffer[ep].index, _usbEPCacheBuffer[ep].len - _usbEPCacheBuffer[ep].index);
      clearEPCache(ep);
  }
  return written;*/
}

uint32_t USBDeviceClass::armSend(uint32_t ep, const void* data, uint32_t len)
{
  
  return sendControl((int)ep , data, len);
}

void USBDeviceClass::sendZlp(uint32_t ep)
{
  
}

void USBDeviceClass::ISRHandler()
{
  //USBD_DEBUG1("irq\r\n");
  uint32_t ep_index    = 0;
  uint32_t read_len = 0;
  _intr_irq = USBD.hmusb->intr_usb;

  _tx_irq = USBD.hmusb->intr_tx_2;
  _tx_irq <<= 8;
  _tx_irq |= USBD.hmusb->intr_tx_1;
  _rx_irq = USBD.hmusb->intr_rx_2;
  _rx_irq <<= 8;
  _rx_irq |= USBD.hmusb->intr_rx_1;
  
  //USBD_DEBUG1("irq tx=0x%x, rx=0x%d\r\n",_tx_irq, _rx_irq);
  if(_intr_irq.babble_reset){
      USBD_DEBUG("irq reset\r\n");
      USBD.hmusb->faddr = 0;
      USBD.address = 0;
      ep0_stage = MUSB_STAGE_SETUP;
  }
  if(_intr_irq.resume){USBD_DEBUG("irq resume\r\n");}
  if(_intr_irq.suspend){ USBD_DEBUG("irq suspend\r\n");}
  if(_intr_irq.disconnect){USBD_DEBUG("irq disconnect\r\n");}
  if(_intr_irq.connect){USBD_DEBUG("irq connect\r\n");}
  if(_intr_irq.SOF){USBD_DEBUG("irq SOF\r\n");}
  
  if(_tx_irq & (0x01 << 0)){//endpoint0 intrrupt
      USBD.hmusb->index = 0;
      if(USBD.hmusb->mode.peripheral.csr0_tx_csr1.csr0.send_stall) epClearStall(0);
      if (USBD.hmusb->mode.peripheral.csr0_tx_csr1.csr0.setup_end)//一个控制传输结束，该位被置1
      {
          USBD_DEBUG("setup_end!\n");
          USBD.hmusb->mode.peripheral.csr0_tx_csr1.csr0.serviced_setup_end = 1;
          ep0_stage = MUSB_STAGE_SETUP;
          USBD.in_ep[0].remain_size = 0;
          USBD.out_ep[0].remain_size = 0;
      }
      
      if (USBD.address != USBD.hmusb->faddr)
      {
          USBD.hmusb->faddr = USBD.address;
          USBD_DEBUG("change address %02X\n", USBD.hmusb->faddr);
      }
      switch(ep0_stage){
          case MUSB_STAGE_SETUP:
            USBD_DEBUG("case MUSB_STAGE_SETUP %d\n",USBD.hmusb->mode.peripheral.csr0_tx_csr1.csr0.rx_pkt_rdy);
            //read_len = USBD.in_ep[0].max_packet_size;
            //rt_memset(&USBD.in_ep[0],0,sizeof(struct musb_pcd_ep));
            //USBD.in_ep[0].max_packet_size = read_len;
            if(USBD.hmusb->mode.peripheral.csr0_tx_csr1.csr0.rx_pkt_rdy){//如果端点0有数据
                read_len =USBD.hmusb->mode.peripheral.count0_rx_count1;
                 if (USBD.hmusb->mode.peripheral.count0_rx_count1 != 8)
                 {
                     USBD_DEBUG("__LINE__=%d\n",__LINE__);
                     USBD.hmusb->mode.peripheral.csr0_tx_csr1.csr0.serviced_rx_pkt_rdy = 1;
                     epSetStall(0);
                     return;
                 }
                USBD_DEBUG("len=%d\r\n", read_len);
                /*建立过程由USB主机发起*/
                readFIFO(0, (uint8_t *)&USBD.setup_pkt, read_len);
                #if 1
                // /*USB标准请求，8个字节*/
                USBD_DEBUG1("=====================================\n");
                USBD_DEBUG1("request_type = 0x%X\n",USBD.setup_pkt.request_type);
                USBD_DEBUG1("bRequest = 0x%X\n",USBD.setup_pkt.bRequest);
                USBD_DEBUG1("wValue = 0x%X\n",USBD.setup_pkt.wValue);
                USBD_DEBUG1("wIndex = 0x%X\n",USBD.setup_pkt.wIndex);
                USBD_DEBUG1("wLength = 0x%X\n",USBD.setup_pkt.wLength);
                USBD_DEBUG1("=====================================\n");
                #endif
                
                if(USBD.setup_pkt.wLength != 0){/*如果标准请求需要请求的数据不为0，则代表控制传输的过程由数据事务的产生*/
                    /*判断数据过程是发送DOUT，还是接收DIN*/
                    /*判断是否为设置地址命令*/
                    if(USBD.setup_pkt.request_type & 0x80){
                        USBD_DEBUG("ep0_stage = MUSB_STAGE_DIN;\n");
                        ep0_stage = MUSB_STAGE_DIN;
                        USBD.in_ep[0].remain_size = USBD.setup_pkt.wLength;
                        USBD.in_ep[0].transfer_size = 0;
                        USBD.in_ep[0].buffer = 0;
                    }else{
                        USBD_DEBUG("ep0_stage = MUSB_STAGE_DOUT; %d\n", USBD.in_ep[0].remain_size);
                        ep0_stage = MUSB_STAGE_DOUT;
                        USBD.out_ep[0].remain_size = USBD.setup_pkt.wLength;
                        USBD.out_ep[0].transfer_size = 0;
                        USBD.out_ep[0].buffer = 0;
                    }
                }
                /*清除接收标志位*/
                
                USBD.hmusb->mode.peripheral.csr0_tx_csr1.csr0.serviced_rx_pkt_rdy = 1;//1
                if(USBD.setup_pkt.request_type == 0x00 && USBD.setup_pkt.bRequest == 0x05){
                    //USBD.hmusb->mode.peripheral.csr0_tx_csr1.csr0.serviced_rx_pkt_rdy = 1;//2
                    USBD.address = USBD.setup_pkt.wValue & 0x7f;//保存地址，待下一次事物传输改变地址
                    USBD_DEBUG("set address %02X\n", USBD.address);
                }else{//解析数据包
                    parsingSteupPkt(USBD.setup_pkt);
                }
                if(USBD.setup_pkt.wLength == 0){
                    USBD.hmusb->mode.peripheral.csr0_tx_csr1.csr0.tx_pkt_rdy = 1;//准备发送
                    USBD.hmusb->mode.peripheral.csr0_tx_csr1.csr0.data_end = 1;//发送最后一个数据包
                }
            }
            break;
          case MUSB_STAGE_DIN:
            USBD_DEBUG("case MUSB_STAGE_DIN\n");
            USBD.in_ep[0].remain_size -= USBD.in_ep[0].transfer_size;
            if(USBD.in_ep[0].remain_size != 0)
                USBD.in_ep[0].buffer += USBD.in_ep[0].transfer_size;
            else
                USBD.in_ep[0].buffer = NULL;
            if(USBD.in_ep[0].remain_size >= USBD.in_ep[0].max_packet_size){
                USBD.in_ep[0].transfer_size = USBD.in_ep[0].max_packet_size;
            }else{
                USBD.in_ep[0].transfer_size = USBD.in_ep[0].remain_size;
                ep0_stage = MUSB_STAGE_SETUP;
            }
            USBD_DEBUG("fun=%s, %p tranfersize=%d\r\n", __func__, USBD.in_ep[0].buffer,USBD.in_ep[0].transfer_size);
            epWrite(EP0_IN_ADDR, USBD.in_ep[0].buffer, USBD.in_ep[0].transfer_size);
            
            break;
          case MUSB_STAGE_DOUT:
            USBD_DEBUG("case MUSB_STAGE_DOUT\n");
            if(USBD.hmusb->mode.peripheral.csr0_tx_csr1.csr0.rx_pkt_rdy){
                read_len = USBD.hmusb->mode.peripheral.rx_count2;
                read_len <<= 8;
                read_len |= USBD.hmusb->mode.peripheral.count0_rx_count1;
                if(USBD.out_ep[0].buffer){
                    readFIFO(EP0_OUT_ADDR, USBD.out_ep[0].buffer+USBD.out_ep[0].transfer_size,  read_len);
                    
                    USBD.out_ep[0].transfer_size += read_len;
                    
                    USBD.hmusb->mode.peripheral.csr0_tx_csr1.csr0.serviced_rx_pkt_rdy = 1;
                    if(USBD.out_ep[0].transfer_size == USBD.out_ep[0].remain_size){
                        USBD.out_ep[0].transfer_size = 0;
                        USBD.out_ep[0].remain_size = 0;
                        USBD.out_ep[0].buffer = 0;
                        ep0_stage = MUSB_STAGE_SETUP;
                        USBD.hmusb->mode.peripheral.csr0_tx_csr1.csr0.data_end = 1;
                        USBD.hmusb->mode.peripheral.csr0_tx_csr1.csr0.tx_pkt_rdy = 1;
                        //epWrite(EP0_IN_ADDR, NULL, 0);
                    }
                    
                }else{
                    
                    USBD_DEBUG("fun=%s, %d, %d\n",__func__, usbd_ep_recv_flag[0], usbd_ep_out_size[0]);
                    readFIFO(0, usbd_ep_out_cache_buffer[0]+usbd_ep_out_size[0], read_len);
                    usbd_ep_recv_flag[0] = 1;
                    usbd_ep_out_size[0] = read_len;
                }
                
            }
            break;

      }
  }
  
  for (ep_index = 1; ep_index < 16; ep_index++)
  {
    if(_tx_irq & (0x01 << ep_index)){
         USBD.hmusb->index = ep_index;
         //rt_kprintf("_tx_irq, ep=%d  %d  re=%d  tran=%d\n",ep_index, USBD.in_ep[ep_index].remain_size, USBD.hmusb->mode.peripheral.csr0_tx_csr1.tx_csr1.tx_pkt_rdy,USBD.in_ep[ep_index].transfer_size);
        if(!USBD.hmusb->mode.peripheral.csr0_tx_csr1.tx_csr1.tx_pkt_rdy){
            USBD.in_ep[ep_index].remain_size -= USBD.in_ep[ep_index].transfer_size;
            if(USBD.in_ep[ep_index].remain_size > 0){
                USBD.in_ep[ep_index].buffer += USBD.in_ep[ep_index].transfer_size;
                USBD.in_ep[ep_index].transfer_size = (USBD.in_ep[ep_index].remain_size > USBD.in_ep[ep_index].max_packet_size) ? USBD.in_ep[ep_index].max_packet_size :USBD.in_ep[ep_index].remain_size;
                epWrite(ep_index, USBD.in_ep[ep_index].buffer, USBD.in_ep[ep_index].transfer_size);
            }else{
                USBD.in_ep[ep_index].transfer_size = 0;
                USBD.in_ep[ep_index].buffer = 0;
                PluggableUSB().handleEndpoint(ep_index);
            }
        }
    }
  }
  
  for (ep_index = 1; ep_index < 16; ep_index++)
  {
    if (_rx_irq & (0x01 << ep_index)){
         USBD.hmusb->index = ep_index;
         //USBD_DEBUG1("fun=%s ep=%d, %d\r\n",__func__, ep_index, USBD.hmusb->mode.peripheral.rx_csr_1.rx_pkt_rdy);
         if(USBD.hmusb->mode.peripheral.rx_csr_1.rx_pkt_rdy){
             read_len = USBD.hmusb->mode.peripheral.rx_count2;
             read_len <<= 8;
             read_len |= USBD.hmusb->mode.peripheral.count0_rx_count1;
             USBD.out_ep[ep_index].remain_size = read_len;
             // uint8_t epRemain = epCaheRemainSpace(ep_index);
             // if(epRemain){
                 // read_len = (USBD.out_ep[ep_index].remain_size > epRemain) ? epRemain : USBD.out_ep[ep_index].remain_size;
                 // readFIFO(ep_index, _usbEPCacheBuffer[ep_index].buf+_usbEPCacheBuffer[ep_index].len, read_len);
                 // _usbEPCacheBuffer[ep_index].len += read_len;
                 // USBD.out_ep[ep_index].remain_size -= read_len;
             // }
             USBD.in_ep[ep_index].transfer_size = 0;
             PluggableUSB().handleEndpoint(ep_index);
             //while(USBD.out_ep[ep_index].remain_size){
                 //read_len = (USBD.out_ep[ep_index].remain_size > USB_TEMP_BUFFER_SIZE) ? USB_TEMP_BUFFER_SIZE : USBD.out_ep[ep_index].remain_size;
                 //readFIFO(ep_index, usbdTempBuffer, read_len);
                 //writeDataToUsbEPCache(ep_index, usbdTempBuffer, read_len);
                 //usbDataEnqueue(ep_index, usbdTempBuffer, read_len);
                 //rt_kprintf("index=%d,read_len = %d  %d  %d %d\n", read_len, usbd_ep_queueData[ep_index],getDataQueueTotalSize(ep_index),ep_index);
                 //USBD.out_ep[ep_index].remain_size -= read_len;
             //}
             //USBD.hmusb->mode.peripheral.rx_csr_1.rx_pkt_rdy = 0;
             
             
             //USBD_DEBUG1("fun=%s ep=%d %d\r\n",__func__, ep_index, USBD.hmusb->index);
             
             
             //if(USBD.hmusb->index != ep_index) USBD.hmusb->index = ep_index;
             
         }
    }
  }
}

void USBDeviceClass::handleStandardRequest(USBSetup &setup){
  switch(setup.direction){
      case USB_REQ_TYPE_DEVICE:
        switch(setup.bRequest){
            case USB_REQ_GET_STATUS:
              USBD_DEBUG1("USB_REQ_GET_STATUS\n");
              sendControl(0 , (const void *)&_status, ((setup.wLength > 2) ? 2 : setup.wLength));
              //USBD.hmusb->mode.peripheral.csr0_tx_csr1.csr0.serviced_rx_pkt_rdy = 1;
              break;
            case USB_REQ_CLEAR_FEATURE:
              USBD_DEBUG1("USB_REQ_CLEAR_FEATURE\n");
              //USBD.hmusb->mode.peripheral.csr0_tx_csr1.csr0.serviced_rx_pkt_rdy = 1;
              break;
            case USB_REQ_SET_FEATURE:
              USBD_DEBUG1("USB_REQ_SET_FEATURE\n");
              //USBD.hmusb->mode.peripheral.csr0_tx_csr1.csr0.serviced_rx_pkt_rdy = 1;
              break;
            case USB_REQ_SET_ADDRESS:
              USBD_DEBUG("USB_REQ_SET_ADDRESS\n");
              //USBD.hmusb->mode.peripheral.csr0_tx_csr1.csr0.serviced_rx_pkt_rdy = 1;
              break;
            case USB_REQ_GET_DESCRIPTOR:
              USBD_DEBUG1("USB_REQ_GET_DESCRIPTOR\n");
              getDescriptor(setup);
              break;
            case USB_REQ_SET_DESCRIPTOR:
              USBD_DEBUG1("USB_REQ_SET_DESCRIPTOR\n");
              //USBD.hmusb->mode.peripheral.csr0_tx_csr1.csr0.serviced_rx_pkt_rdy = 1;
              break;
            case USB_REQ_GET_CONFIGURATION:
              USBD_DEBUG1("USB_REQ_GET_CONFIGURATION\n");
              //epSetStall(0);
              sendControl(0 , (const void *)&_config, ((setup.wLength > 1) ? 1 : setup.wLength));
              //USBD.hmusb->mode.peripheral.csr0_tx_csr1.csr0.serviced_rx_pkt_rdy = 1;
              break;
            case USB_REQ_SET_CONFIGURATION:
              USBD_DEBUG1("USB_REQ_SET_CONFIGURATION\n");
              //USBD.hmusb->mode.peripheral.csr0_tx_csr1.csr0.serviced_rx_pkt_rdy = 1;
              initEndpoints();
              break;
            default:
              USBD_DEBUG("unknown device request\n");
              break;
        }
        break;
      case USB_REQ_TYPE_INTERFACE:
        //USBD.hmusb->mode.peripheral.csr0_tx_csr1.csr0.serviced_rx_pkt_rdy = 1;
        switch(setup.bRequest){
            case USB_REQ_GET_INTERFACE:
              USBD_DEBUG1("USB_REQ_GET_INTERFACE\n");
              
              break;
            case USB_REQ_SET_INTERFACE:
              USBD_DEBUG1("USB_REQ_SET_INTERFACE\n");
              break;
            case USB_REQ_GET_DESCRIPTOR:
              //getDescriptor(setup);
			  //PluggableUSB().getDescriptor(setup);
			  USBD_DEBUG1("USB_REQ_GET_DESCRIPTOR\n");
              break;
            default:
              USBD_DEBUG1("unknown interface request\n");
              break;
        }
        break;
      case USB_REQ_TYPE_ENDPOINT:
        //USBD.hmusb->mode.peripheral.csr0_tx_csr1.csr0.serviced_rx_pkt_rdy = 1;
        switch(setup.bRequest){
            case USB_REQ_GET_STATUS:
                USBD_DEBUG("USB_REQ_GET_STATUS\n");
                break;
            case USB_REQ_CLEAR_FEATURE:
                USBD_DEBUG("USB_REQ_CLEAR_FEATURE\n");
                break;
            case USB_REQ_SET_FEATURE:
                USBD_DEBUG("USB_REQ_SET_FEATURE\n");
                break;
            case USB_REQ_SYNCH_FRAME:
                USBD_DEBUG("USB_REQ_SYNCH_FRAME\n");
                break;
            default:
                USBD_DEBUG("unknown endpoint request\n");
                break;
        }
        break;
      case USB_REQ_TYPE_OTHER:
        USBD_DEBUG("USB_REQ_TYPE_OTHER\n");
        break;
      default:
        USBD_DEBUG("unknown type request\n");
        break;
  }
}
void USBDeviceClass::handleFunctionRequest(USBSetup &setup){
  USBD_DEBUG("_function_request\n");
  switch(setup.direction){
      case USB_REQ_TYPE_INTERFACE:
          USBD_DEBUG("USB_REQ_TYPE_INTERFACE\n");
          handleClassInterfaceSetup(setup);
          break;
      case USB_REQ_TYPE_ENDPOINT:
          USBD_DEBUG("USB_REQ_TYPE_ENDPOINT");
          break;
      default:
          USBD_DEBUG("unknown function request type\n");
          break;
  }
}
void USBDeviceClass::handleVendorRequest(USBSetup &setup){
  USBD_DEBUG("_vendor_request\n");
  switch(setup.bRequest){
      case 'A':
          switch(setup.wIndex){
              case 0x04:
                  USBD_DEBUG("0x04\n");
                  break;
              case 0x05:
                  USBD_DEBUG("0x05\n");
                  break;
          }
          break;
  }
}

void USBDeviceClass::getDescriptor(USBSetup &setup){
  if(setup.transferDirection == 1){
      //USBD.hmusb->mode.peripheral.csr0_tx_csr1.csr0.serviced_rx_pkt_rdy = 1;//清除setup接收包
      switch(setup.wValue >> 8){
          case USB_DESC_TYPE_DEVICE:{
              USBD_DEBUG("USB_DESC_TYPE_DEVICE\n");
              // int ret = PluggableUSB().getDescriptor(setup);
              // if (ret != 0) {
                 // return;
              // }
          getDeviceDescriptor(setup);}
              break;
          case USB_DESC_TYPE_CONFIGURATION:
              USBD_DEBUG("USB_DESC_TYPE_CONFIGURATION\n");
              getConfigDescriptor(setup);
              break;
          case USB_DESC_TYPE_STRING:
              USBD_DEBUG("USB_DESC_TYPE_STRING\n");
              getStringDescriptor(setup);
              break;
          case USB_DESC_TYPE_DEVICEQUALIFIER:
              USBD_DEBUG("USB_DESC_TYPE_DEVICEQUALIFIER\n");
              //USBD.hmusb->mode.peripheral.csr0_tx_csr1.csr0.serviced_rx_pkt_rdy = 1;
              epSetStall(0);
              break;
          case USB_DESC_TYPE_OTHERSPEED:
              USBD_DEBUG("USB_DESC_TYPE_OTHERSPEED\n");
              break;
          default:
              USBD_DEBUG("unsupported descriptor request\n");
              break;
      }
  }else{
      USBD_DEBUG("request direction error\n");
  }
}

void USBDeviceClass::getDeviceDescriptor(USBSetup &setup){
  
  uint32_t size;
  size = (setup.wLength > USB_DESC_LENGTH_DEVICE) ? USB_DESC_LENGTH_DEVICE : setup.wLength;
  memset(usbd_ep_cache_buffer[0],0, 512);
  USBD.in_ep[0].buffer = usbd_ep_cache_buffer[0];
  memcpy(usbd_ep_cache_buffer[0], &USB_DeviceDescriptorB, sizeof(DeviceDescriptor));
  USBD.in_ep[0].remain_size = size;
  USBD.in_ep[0].transfer_size = (size > USBD.in_ep[0].max_packet_size) ? USBD.in_ep[0].max_packet_size : size;
  USBD_DEBUG("func=%s, 0x%p, 0x%p %d, %d %d\r\n",__func__, USBD.in_ep[0].buffer, &USB_DeviceDescriptorB, USBD.in_ep[0].transfer_size, USBD.in_ep[0].max_packet_size,size);
  //USBD.hmusb->mode.peripheral.csr0_tx_csr1.csr0.serviced_rx_pkt_rdy = 1;
  size = epWrite(EP0_IN_ADDR,USBD.in_ep[0].buffer, USBD.in_ep[0].transfer_size);
  USBD.in_ep[0].transfer_size = size;
}
void USBDeviceClass::getConfigDescriptor(USBSetup &setup){
  uint32_t total = 0;//配置描述符的总字节数
  uint32_t size;
  memset(usbd_ep_cache_buffer[0],0, 512);
  
  USBD.in_ep[0].buffer = usbd_ep_cache_buffer[0];
  USBD_DEBUG("0[0]=%p, size=%d\r\n", usbd_ep_cache_buffer[0], usbd_ep_size[0]);
  packMessages(true, sizeof(ConfigDescriptor));
  uint8_t interfaceNums = getInterfacesInfo(&total);//配置描述符的接口数量
  ConfigDescriptor config = D_CONFIG((uint16_t)(total + sizeof(ConfigDescriptor)), interfaceNums);
  memcpy(usbd_ep_cache_buffer[0], &config, sizeof(ConfigDescriptor));
  _config = 1;
  size = (setup.wLength > usbd_ep_size[0]) ? usbd_ep_size[0] : setup.wLength;
  USBD.in_ep[0].remain_size =size;
  USBD.in_ep[0].transfer_size = (size > USBD.in_ep[0].max_packet_size) ? USBD.in_ep[0].max_packet_size : size;
  //USBD.hmusb->mode.peripheral.csr0_tx_csr1.csr0.serviced_rx_pkt_rdy = 1;
  size = epWrite(EP0_IN_ADDR,USBD.in_ep[0].buffer, USBD.in_ep[0].transfer_size);
  USBD.in_ep[0].transfer_size = size;
  USBD_DEBUG("1[0]=%p, size=%d\r\n", usbd_ep_cache_buffer[0], usbd_ep_size[0]);
  
  //usbd_ep_cache_buffer[0]
  /*size = (setup.wLength > sizeof(cfg_desc_cfg)) ? sizeof(cfg_desc_cfg) : setup->wLength;
  musb_hpcd1->in_ep[0].buffer = cfg_desc_cfg;
  musb_hpcd1->in_ep[0].remain_size = size;
  musb_hpcd1->in_ep[0].transfer_size = (size > musb_hpcd1->in_ep[0].max_packet_size) ? musb_hpcd1->in_ep[0].max_packet_size : size;
  size = usbd_pcd_ep_write(musb_hpcd1, EP0_IN_ADDR, musb_hpcd1->in_ep[0].buffer, musb_hpcd1->in_ep[0].transfer_size);
  musb_hpcd1->in_ep[0].transfer_size = size;*/
}

static uint8_t str_serial_cfg[]={
    0x2A, 0x03, 0x37, 0x00, 0x35, 0x00, 0x38, 0x00,
    0x33, 0x00, 0x33, 0x00, 0x33, 0x00, 0x35, 0x00,
    0x33, 0x00, 0x39, 0x00, 0x33, 0x00, 0x34, 0x00,
    0x33, 0x00, 0x35, 0x00, 0x31, 0x00, 0x37, 0x00,
    0x31, 0x00, 0x36, 0x00, 0x31, 0x00, 0x35, 0x00,
    0x30, 0x00
};

static uint8_t str_langid_cfg[] = {
    0x04, 0x03, 0x09, 0x04
};

void USBDeviceClass::getStringDescriptor(USBSetup &setup){
  uint16_t size = 0;//=0;
  uint8_t index;
  index = setup.wValue & 0xFF;
  if(index == 0xEE){
      index = USB_STRING_OS_INDEX;
  }
  //USBD.hmusb->mode.peripheral.csr0_tx_csr1.csr0.serviced_rx_pkt_rdy = 1;
  switch(index){
      case USB_STRING_LANGID_INDEX: //获取语言ID字符串描述符
          USBD_DEBUG("USB_STRING_LANGID_INDEX\n");
          USBD.in_ep[0].buffer = str_langid_cfg;
          size = (setup.wLength > sizeof(str_langid_cfg)) ? sizeof(str_langid_cfg) : setup.wLength;
          break;
      case USB_STRING_MANU_INDEX: //获取厂商描述字符串描述符
          USBD_DEBUG("USB_STRING_MANU_INDEX\n");
          USBD.in_ep[0].buffer = (uint8_t *)STRING_MANUFACTURER;
          size = (setup.wLength > sizeof(STRING_MANUFACTURER)) ? sizeof(STRING_MANUFACTURER) : setup.wLength;
          break;
      case USB_STRING_PRODUCT_INDEX: //获取产品字符串描述符
          USBD_DEBUG("USB_STRING_PRODUCT_INDEX\n");
          USBD.in_ep[0].buffer = (uint8_t *)STRING_PRODUCT;
          size = (setup.wLength > sizeof(STRING_PRODUCT)) ? sizeof(STRING_PRODUCT) : setup.wLength;
          break;
      case USB_STRING_SERIAL_INDEX: //获取产品序列号字符串描述符
          USBD_DEBUG("USB_STRING_SERIAL_INDEX\n");
          USBD.in_ep[0].buffer =str_serial_cfg;
          size = (setup.wLength > sizeof(str_serial_cfg)) ? sizeof(str_serial_cfg) : setup.wLength;
          break;
      case USB_STRING_CONFIG_INDEX: //获取配置描述符描述字符串描述符
          USBD_DEBUG("USB_STRING_CONFIG_INDEX\n");
          break;
      case USB_STRING_INTERFACE_INDEX: //获取接口描述符描述字符串描述符
          USBD_DEBUG("USB_STRING_INTERFACE_INDEX\n");
          break;
      case USB_STRING_OS_INDEX: //获取操作系统描述字符串描述符
          USBD_DEBUG("USB_STRING_OS_INDEX\n");
          break;
      default:
          USBD_DEBUG("unknown string index\n");
          //musb_pcd_ep_set_stall(0);
          return;
          break;
  }
  if(index > USB_STRING_MAX){
      USBD_DEBUG("unknown string index\n");
      //musb_pcd_ep_set_stall(0);
      return;
  }
  USBD_DEBUG("index = %d\n",index);
  USBD.in_ep[0].transfer_size = (size > USBD.in_ep[0].max_packet_size) ? USBD.in_ep[0].max_packet_size : size;
  size =  epWrite(EP0_IN_ADDR,USBD.in_ep[0].buffer, USBD.in_ep[0].transfer_size);
  USBD.in_ep[0].transfer_size = size;
}
uint8_t USBDeviceClass::getInterfacesInfo(uint32_t* total){
  uint8_t interfaceNums = 0;
#ifdef PLUGGABLE_USB_ENABLED
  *total += PluggableUSB().getInterface(&interfaceNums);
  USBD_DEBUG("fun=%s, %d, %d\n",__func__, interfaceNums, *total);
#endif
  
  return interfaceNums;
}

void USBDeviceClass::readFIFO(int ep_index, uint8_t * data, uint32_t size){
  uint32_t count32 = ((uint32_t)data & 3) ? 0 : (size >> 2);
  uint32_t offset = 0;
  size -= count32 << 2;
  while(count32){
      *((uint32_t *)&data[offset]) = USBD.hmusb->fifo[ep_index];
      offset += 4;
      count32--;
  }
  while(size){
      data[offset] = *((m_reg_t *)&USBD.hmusb->fifo[ep_index]);
      offset ++;
      size--;
  }
}
void USBDeviceClass::writeFIFO(int ep_index, uint8_t * data, uint32_t size){
  uint32_t count32 = ((uint32_t)data & 3) ? 0 : (size >> 2);
  uint32_t offset = 0;
  size -= count32 << 2;
  while(count32)
  {
      USBD.hmusb->fifo[ep_index] = *((uint32_t *)&data[offset]);
      offset += 4;
      count32--;
  }
  while(size)
  {
      *((m_reg_t *)&USBD.hmusb->fifo[ep_index]) = data[offset];
      offset ++;
      size--;
  }
}

bool USBDeviceClass::epFIFOIsNotEmpty(uint8_t ep_addr){
  uint8_t epIndex = USBD.hmusb->index;
  USBD.hmusb->index = ep_addr & 0x7f;
  int ret = 0;
  if(ep_addr & 0x7f){
     ret = USBD.hmusb->mode.peripheral.csr0_tx_csr1.tx_csr1.fifo_not_empty;
  }
  if(ret == 0) USBD.hmusb->mode.peripheral.csr02_tx_csr2.csr02.flush_fifo = 1;
  //rt_kprintf("empty=%d, 0x%x, %d\n", ret, ep_addr, epIndex);
  USBD.hmusb->index = epIndex;
   return ret;
}
uint32_t USBDeviceClass::epWrite(uint8_t ep_addr, void *buffer, uint32_t size){
  mi_d11t value;
  *((m_reg_t *)(&value)) = 0;
  USBD.hmusb->index = ep_addr & 0x7f;
  
  //if(size > USBD.in_ep[ep_addr & 0x7f].max_packet_size) return 0;
  //USBD_DEBUG("fun=%s, %d\n", __func__, size);
  if ((ep_addr & 0x7f) == 0)
  {
      //USBD.hmusb->mode.peripheral.csr02_tx_csr2.csr02.flush_fifo = 1;//刷新FIFO
      //USBD.hmusb->mode.peripheral.csr0_tx_csr1.csr0.serviced_rx_pkt_rdy = 1;//清除接收中断标志
      //writeFIFO(ep_addr & 0x7f, (uint8_t *)buffer, size);
      //清除接收中断标志
      *(m_reg_t *)(&USBD.hmusb->mode.peripheral.csr0_tx_csr1) = 0;
      writeFIFO(ep_addr & 0x7f, (uint8_t *)buffer, size);
      value.csr0.tx_pkt_rdy = 1;
      if (USBD.in_ep[0].remain_size < MYUSB_POINT_SIZE || size < MYUSB_POINT_SIZE)
      {
          USBD_DEBUG("====end====\n");
          value.csr0.data_end = 1;
          ep0_stage = MUSB_STAGE_SETUP;
      }
      USBD.hmusb->mode.peripheral.csr0_tx_csr1 = value;
  }else{
     value.tx_csr1.tx_pkt_rdy = 1;
     writeFIFO(ep_addr & 0x7f, (uint8_t *)buffer, size);
     USBD.hmusb->mode.peripheral.csr0_tx_csr1.tx_csr1.tx_pkt_rdy = 1;
     //while(hpcd->hmusb->mode.peripheral.csr0_tx_csr1.tx_csr1.fifo_not_empty);//{//如果FIFO里面有数据，则等待FIFO数据完成再写
         //rt_kUSBD_DEBUG("fun=%s,%d %d\r\n",__func__, hpcd->hmusb->mode.peripheral.csr0_tx_csr1.tx_csr1.fifo_not_empty,hpcd->hmusb->mode.peripheral.csr0_tx_csr1.tx_csr1.tx_pkt_rdy);
     //}
     //hpcd->hmusb->mode.peripheral.csr0_tx_csr1.tx_csr1.flush_fifo = 1;//清除tx_pkt_rdy位和fifo,只有tx_pkt_rdy位为1的时候有效
  }
  
  //USBD.hmusb->mode.peripheral.csr0_tx_csr1 = value;
  
  return size;
}
uint32_t USBDeviceClass::epReadPrepare(uint32_t ep_addr, void *buffer, uint32_t size){
  ep_addr &= 0x7f;
  USBD.hmusb->index = ep_addr;
  USBD.out_ep[ep_addr].buffer = (uint8_t *)buffer;
  USBD.out_ep[ep_addr].remain_size = size;
  return size;
}

int USBDeviceClass::epSetStall(uint8_t ep_addr){
  USBD_DEBUG("%s,ep_addr = 0x%X\n",__func__,ep_addr);
  USBD.hmusb->index = ep_addr & 0x7f;
  if ((ep_addr & 0x7f) == 0){
      USBD.hmusb->mode.peripheral.csr0_tx_csr1.csr0.send_stall = 1;
      return 0;
  }else{
      if (ep_addr & 0x80){
          USBD.hmusb->mode.peripheral.csr0_tx_csr1.tx_csr1.send_stall = 1;
          return 0;
      }else{
          USBD.hmusb->mode.peripheral.rx_csr_1.send_stall = 1;
          return 0;
      }
  }
  return -1;
}

int USBDeviceClass::epClearStall(uint8_t ep_addr){
  USBD_DEBUG("%s,ep_addr = 0x%X\n",__func__,ep_addr);
  USBD.hmusb->index = ep_addr & 0x7f;
  if ((ep_addr & 0x7f) == 0)
  {
      USBD.hmusb->mode.peripheral.csr0_tx_csr1.csr0.send_stall = 0;
      USBD.hmusb->mode.peripheral.csr0_tx_csr1.csr0.sent_stall = 0;
      return 0;
  }
  else
  {
      if (ep_addr & 0x80)
      {
          USBD.hmusb->mode.peripheral.csr0_tx_csr1.tx_csr1.send_stall = 0;
          USBD.hmusb->mode.peripheral.csr0_tx_csr1.tx_csr1.sent_stall = 0;
          return epOpen(ep_addr, USBD.in_ep[(ep_addr & 0x7f)].max_packet_size, USBD.in_ep[(ep_addr & 0x7f)].is_iso);
      }
      else
      {
          USBD.hmusb->mode.peripheral.rx_csr_1.send_stall = 0;
          USBD.hmusb->mode.peripheral.rx_csr_1.sent_stall = 0;
          return epOpen(ep_addr, USBD.out_ep[(ep_addr & 0x7f)].max_packet_size, USBD.out_ep[(ep_addr & 0x7f)].is_iso);
      }
  }
  return -1;
}

int USBDeviceClass::epOpen(uint8_t ep_addr, uint8_t max_packet_size, uint8_t is_iso){
  //USBD_DEBUG1("%s,ep_addr=0x%X,is_iso=0x%X\n",__func__,ep_addr,is_iso);
  USBD.hmusb->index = ep_addr & 0x7f;
  if(ep_addr & 0x80){
      USBD.in_ep[(ep_addr & 0x7f)].is_iso = is_iso;
      USBD.in_ep[(ep_addr & 0x7f)].max_packet_size = max_packet_size;
      USBD.hmusb->mode.peripheral.csr02_tx_csr2.tx_csr2.auto_set = 1;
      USBD.hmusb->mode.peripheral.csr02_tx_csr2.tx_csr2.ISO = 0x00;
      USBD.hmusb->mode.peripheral.csr02_tx_csr2.tx_csr2.mode = 1;
      USBD.hmusb->mode.peripheral.csr02_tx_csr2.tx_csr2.dma_enable = 1;
      USBD.hmusb->mode.peripheral.csr02_tx_csr2.tx_csr2.frc_data_tog = 1;
      USBD.hmusb->mode.peripheral.tx_max_p = 9;
  }else{
      USBD.out_ep[(ep_addr & 0x7f)].is_iso = is_iso;
      USBD.hmusb->mode.peripheral.rx_csr_2.auto_clear = 0;
      USBD.hmusb->mode.peripheral.rx_csr_2.ISO_auto_req = 0x00;
      USBD.hmusb->mode.peripheral.rx_csr_2.dma_enable = 0;
      USBD.hmusb->mode.peripheral.rx_max_p = 9;
  }
  return 0;
}

void USBDeviceClass::parsingSteupPkt(USBSetup &setup){
  switch(setup.type){
      case USB_STANDARD_REQ_TYPE:
        handleStandardRequest(setup);
        break;
      case USB_CLASS_REQ_TYPE:
        handleFunctionRequest(setup);
        break;
      case USB_VENDOR_REQ_TYPE:
        handleVendorRequest(setup);
        break;
      default:
        USBD_DEBUG("unknown setup request type\n");
        break;

  }
}
void USBDeviceClass::clearEp0RXPacketRdy(){
  USBD.hmusb->mode.peripheral.csr0_tx_csr1.csr0.serviced_rx_pkt_rdy = 1;
}

USBDeviceClass USBDevice;

