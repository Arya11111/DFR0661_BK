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
#include "CDC.h"
#include "USBAPI.h"
#include "USBCore.h"

#define USB_VID 0x2A03
#define USB_PID 0x0043
#define IMANUFACTURER 0x01
#define IPRODUCT      0x02
#define ISERIAL       0x03

bool _dry_run = false;
bool _pack_message = false;
uint16_t _pack_size = 0;
uint8_t _pack_buffer[256];


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

uint8_t usbd_ep_cache_buffer[16][512]={0x00};
uint16_t usbd_ep_size[16]={0x00};

static __attribute__((__aligned__(4))) //__attribute__((__section__(".bss_hram0")))
uint8_t udd_ep_out_cache_buffer[7][512];
 
static __attribute__((__aligned__(4))) //__attribute__((__section__(".bss_hram0")))
uint8_t udd_ep_in_cache_buffer[7][512];

// Some EP are handled using EPHanlders.
// Possibly all the sparse EP handling subroutines will be
// converted into reusable EPHandlers in the future.
//static EPHandler *epHandlers[7] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL};


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
#endif
};
#define EP_ARRAY_SIZE   (sizeof(EndPoints)/sizeof(EndPoints[0]))
USBDeviceClass::USBDeviceClass(){
  memset(&USBD, 0, sizeof(sMusb_hpcd_instance_t));
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
  USBD.hmusb->intr_usb_e.suspend = 0;
  USBD.hmusb->intr_usb_e.resume = 0;
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
  USBD_DEBUG("musb_hpcd1->hmusb->power\r\n");
  USBD.hmusb->faddr = 0;
  USBD_DEBUG("musb_hpcd1->hmusb->faddr = 0;\r\n");
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
#ifdef CDC_ENABLED
    SerialUSB.begin(0);
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
  return 1;
}

uint32_t USBDeviceClass::recvControl(void *_data, uint32_t len)
{
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

void USBDeviceClass::packMessages(bool val)
{
  if (val) {
      _pack_message = true;
      _pack_size = 0;
  } else {
      _pack_message = false;
      sendControl(_pack_buffer, _pack_size);
  }
}



void USBDeviceClass::initEndpoints() {
  
}

void USBDeviceClass::initEP(uint32_t ep, uint32_t config)
{
  
}

void USBDeviceClass::flush(uint32_t ep)
{
  
}

void USBDeviceClass::clear(uint32_t ep) {
  
}

void USBDeviceClass::stall(uint32_t ep)
{
  
}

// Number of bytes, assumes a rx endpoint
uint32_t USBDeviceClass::available(uint32_t ep)
{
  return 0;
}

// Non Blocking receive
// Return number of bytes read
uint32_t USBDeviceClass::recv(uint32_t ep, void *_data, uint32_t len)
{
  if (!_usbConfiguration)
      return -1;

#ifdef PIN_LED_RXL
  if (rxLEDPulse == 0)
      digitalWrite(PIN_LED_RXL, LOW);

  rxLEDPulse = TX_RX_LED_PULSE_MS;
#endif

  return 0;
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

// Blocking Send of data to an endpoint
uint32_t USBDeviceClass::send(uint32_t ep, const void *data, uint32_t len)
{
#ifdef PIN_LED_TXL
    if (txLEDPulse == 0)
        digitalWrite(PIN_LED_TXL, LOW);

    txLEDPulse = TX_RX_LED_PULSE_MS;
#endif
  return 0;
}

uint32_t USBDeviceClass::armSend(uint32_t ep, const void* data, uint32_t len)
{
  return 0;
}

void USBDeviceClass::sendZlp(uint32_t ep)
{
  
}

void USBDeviceClass::ISRHandler()
{
  USBD_DEBUG("irq\r\n");
  uint32_t ep_index    = 0;
  uint32_t read_len = 0;
  _intr_irq = USBD.hmusb->intr_usb;

  _tx_irq = USBD.hmusb->intr_tx_2;
  _tx_irq <<= 8;
  _tx_irq |= USBD.hmusb->intr_tx_1;
  _rx_irq = USBD.hmusb->intr_rx_2;
  _rx_irq <<= 8;
  _rx_irq |= USBD.hmusb->intr_rx_1;
  
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
      USBD.hmusb->index = ep_index;
      if (USBD.hmusb->mode.peripheral.csr0_tx_csr1.csr0.setup_end)
      {
          USBD.hmusb->mode.peripheral.csr0_tx_csr1.csr0.serviced_setup_end = 1;
          ep0_stage = MUSB_STAGE_SETUP;
          USBD.in_ep[0].remain_size = 0;
          USBD.out_ep[0].remain_size = 0;
      }
      if (USBD.address != USBD.hmusb->faddr)
      {
          USBD.hmusb->faddr = USBD.address;
          USBD_DEBUG("change address %02X", USBD.hmusb->faddr);
      }
      switch(ep0_stage){
          case MUSB_STAGE_SETUP:
            USBD_DEBUG("case MUSB_STAGE_SETUP\n");
            read_len = USBD.in_ep[0].max_packet_size;
            rt_memset(&USBD.in_ep[0],0,sizeof(struct musb_pcd_ep));
            USBD.in_ep[0].max_packet_size = read_len;
            if(USBD.hmusb->mode.peripheral.csr0_tx_csr1.csr0.rx_pkt_rdy){//如果端点0有数据
                read_len =USBD.hmusb->mode.peripheral.count0_rx_count1;
                USBD_DEBUG("len=%d\r\n", read_len);
                /*建立过程由USB主机发起*/
                readFifo(0, (uint8_t *)&USBD.setup_pkt, read_len);
                #if 1
                // /*USB标准请求，8个字节*/
                USBD_DEBUG("=====================================\n");
                USBD_DEBUG("request_type = 0x%X\n",USBD.setup_pkt.request_type);
                USBD_DEBUG("bRequest = 0x%X\n",USBD.setup_pkt.bRequest);
                USBD_DEBUG("wValue = 0x%X\n",USBD.setup_pkt.wValue);
                USBD_DEBUG("wIndex = 0x%X\n",USBD.setup_pkt.wIndex);
                USBD_DEBUG("wLength = 0x%X\n",USBD.setup_pkt.wLength);
                USBD_DEBUG("=====================================\n");
                #endif
                /*清除接收标志位*/
                USBD.hmusb->mode.peripheral.csr0_tx_csr1.csr0.serviced_rx_pkt_rdy = 1;
            
                if(USBD.setup_pkt.wLength != 0){/*如果标准请求需要请求的数据不为0，则代表控制传输的过程由数据事务的产生*/
                    /*判断数据过程是发送DOUT，还是接收DIN*/
                    if(USBD.setup_pkt.request_type & 0x80){
                        USBD_DEBUG("ep0_stage = MUSB_STAGE_DIN;\n");
                        ep0_stage = MUSB_STAGE_DIN;
                        USBD.in_ep[0].remain_size = USBD.setup_pkt.wLength;//需要发送的数据字节
                    }else{
                        USBD_DEBUG("ep0_stage = MUSB_STAGE_DOUT;\n");
                        ep0_stage = MUSB_STAGE_DOUT;
                    }
                }else{//如果为0,发送一个0长度数据包
                    USBD.hmusb->mode.peripheral.csr0_tx_csr1.csr0.tx_pkt_rdy = 1;//准备发送
                    USBD.hmusb->mode.peripheral.csr0_tx_csr1.csr0.data_end = 1;//发送最后一个数据包
                }
                /*判断是否为设置地址命令*/
                if(USBD.setup_pkt.request_type == 0x00 && USBD.setup_pkt.bRequest == 0x05){
                    USBD.address = USBD.setup_pkt.wValue & 0x7f;//保存地址，待下一次事物传输改变地址
                    USBD_DEBUG("set address %02X\n", USBD.address);
                }else{//解析数据包
                    parsingSteupPkt(USBD.setup_pkt);
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
            epWrite(EP0_IN_ADDR, USBD.in_ep[0].buffer, USBD.in_ep[0].transfer_size);
            
            break;
          case MUSB_STAGE_DOUT:
            USBD_DEBUG("case MUSB_STAGE_DOUT\n");
            if(USBD.hmusb->mode.peripheral.csr0_tx_csr1.csr0.rx_pkt_rdy){
                read_len = USBD.hmusb->mode.peripheral.count0_rx_count1;
                if(USBD.out_ep[0].buffer){
                    ep0_stage = MUSB_STAGE_SETUP;
                    readFifo(EP0_OUT_ADDR, USBD.out_ep[0].buffer,  read_len);
                    USBD_DEBUG("********************************\n");
                    //USBD_DEBUG("dwDTERate=%d\nbCharFormat=%d\bDataBits=%d\bParityType=%d\n", line_coding.dwDTERate, line_coding.bCharFormat, line_coding.bParityType, line_coding.bDataBits);
                    USBD_DEBUG("********************************\n");
                    USBD.hmusb->mode.peripheral.csr0_tx_csr1.csr0.serviced_rx_pkt_rdy = 1;
                    USBD.out_ep[0].buffer = 0;
                }else{
                    USBD_DEBUG("musb_hpcd1->out_ep[0].buffer is NULL!\n");
                }
                if(ep0_stage == MUSB_STAGE_SETUP){
                    USBD.hmusb->mode.peripheral.csr0_tx_csr1.csr0.data_end = 1;
                }
            }
            break;

      }
  }
  
  for(ep_index = 0; ep_index < 16; ep_index++){
      if(_tx_irq & (0x01 << ep_index)){
          USBD.hmusb->index = ep_index;
          if(ep_index == 0){
              if (USBD.hmusb->mode.peripheral.csr0_tx_csr1.csr0.setup_end)
              {
                  USBD.hmusb->mode.peripheral.csr0_tx_csr1.csr0.serviced_setup_end = 1;
                  ep0_stage = MUSB_STAGE_SETUP;
                  USBD.in_ep[0].remain_size = 0;
                  USBD.out_ep[0].remain_size = 0;
              }
              if (USBD.address != USBD.hmusb->faddr)
              {
                  USBD.hmusb->faddr = USBD.address;
                  USBD_DEBUG("change address %02X", USBD.hmusb->faddr);
              }
              switch(ep0_stage){
                  case MUSB_STAGE_SETUP:
                    break;
                  case MUSB_STAGE_DIN:
                    break;
                  case MUSB_STAGE_DOUT:
                    break;

              }
          }else{
              
          }
      }
  }

}

void USBDeviceClass::handleStandardRequest(USBSetup &setup){
  switch(setup.direction){
      case USB_REQ_TYPE_DEVICE:
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
            case USB_REQ_SET_ADDRESS:
              USBD_DEBUG("USB_REQ_SET_ADDRESS\n");
              break;
            case USB_REQ_GET_DESCRIPTOR:
              USBD_DEBUG("USB_REQ_GET_DESCRIPTOR\n");
              break;
            case USB_REQ_SET_DESCRIPTOR:
              USBD_DEBUG("USB_REQ_SET_DESCRIPTOR\n");
              break;
            case USB_REQ_GET_CONFIGURATION:
              USBD_DEBUG("USB_REQ_GET_CONFIGURATION\n");
              break;
            case USB_REQ_SET_CONFIGURATION:
              USBD_DEBUG("USB_REQ_SET_CONFIGURATION\n");
              break;
            default:
              USBD_DEBUG("unknown device request\n");
              break;
        }
        break;
      case USB_REQ_TYPE_INTERFACE:
        switch(setup.bRequest){
            case USB_REQ_GET_INTERFACE:
              USBD_DEBUG("USB_REQ_GET_INTERFACE\n");
              break;
            case USB_REQ_SET_INTERFACE:
              USBD_DEBUG("USB_REQ_SET_INTERFACE\n");
              break;
            default:
              USBD_DEBUG("unknown interface request\n");
              break;
        }
        break;
      case USB_REQ_TYPE_ENDPOINT:
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
  
}
void USBDeviceClass::handleVendorRequest(USBSetup &setup){
  
}

void USBDeviceClass::getDescriptor(USBSetup &setup){
  if(setup.transferDirection == 1){
      switch(setup.wValue >> 8){
          case USB_DESC_TYPE_DEVICE:
              USBD_DEBUG("USB_DESC_TYPE_DEVICE\n");
              getDeviceDescriptor(setup);
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
  size = epWrite(EP0_IN_ADDR,USBD.in_ep[0].buffer, USBD.in_ep[0].transfer_size);
  USBD.in_ep[0].transfer_size = size;
}
void USBDeviceClass::getConfigDescriptor(USBSetup &setup){
  uint32_t total = 0;
  uint8_t interfaceNums = getInterfacesInfo(&total);
  memset(usbd_ep_cache_buffer[0],0, 512);
  USBD.in_ep[0].buffer = usbd_ep_cache_buffer[0];
  
  //usbd_ep_cache_buffer[0]
  /*size = (setup.wLength > sizeof(cfg_desc_cfg)) ? sizeof(cfg_desc_cfg) : setup->wLength;
  musb_hpcd1->in_ep[0].buffer = cfg_desc_cfg;
  musb_hpcd1->in_ep[0].remain_size = size;
  musb_hpcd1->in_ep[0].transfer_size = (size > musb_hpcd1->in_ep[0].max_packet_size) ? musb_hpcd1->in_ep[0].max_packet_size : size;
  size = usbd_pcd_ep_write(musb_hpcd1, EP0_IN_ADDR, musb_hpcd1->in_ep[0].buffer, musb_hpcd1->in_ep[0].transfer_size);
  musb_hpcd1->in_ep[0].transfer_size = size;*/
}
void USBDeviceClass::getStringDescriptor(USBSetup &setup){
  
}
uint8_t USBDeviceClass::getInterfacesInfo(uint32_t* total){
  uint8_t interfaceNums = 0;
#ifdef PLUGGABLE_USB_ENABLED
  *total += PluggableUSB().getInterface(&interfaceNums);
#endif
  USBD_DEBUG("fun=%s, %d, %d",__func__, interfaceNums, *total);
  return interfaceNums;
}

void USBDeviceClass::readFifo(int ep_index, uint8_t * data, uint32_t size){
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
void USBDeviceClass::writeFifo(int ep_index, uint8_t * data, uint32_t size){
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
uint32_t USBDeviceClass::epWrite(uint8_t ep_addr, void *buffer, uint32_t size){
  mi_d11t value;
  *((m_reg_t *)(&value)) = 0;
  USBD.hmusb->index = ep_addr & 0x7f;
  *(m_reg_t *)(&USBD.hmusb->mode.peripheral.csr0_tx_csr1) = 0;
  if(size > USBD.in_ep[ep_addr & 0x7f].max_packet_size) return 0;
  
  if ((ep_addr & 0x7f) == 0)
  {
      USBD.hmusb->mode.peripheral.csr02_tx_csr2.csr02.flush_fifo = 1;//刷新FIFO
      USBD.hmusb->mode.peripheral.csr0_tx_csr1.csr0.serviced_rx_pkt_rdy = 1;//清除接收中断标志
      value.csr0.tx_pkt_rdy = 1;
      if (USBD.in_ep[0].remain_size < MYUSB_POINT_SIZE || size < MYUSB_POINT_SIZE)
      {
          USBD_DEBUG("====end====\n");
          value.csr0.data_end = 1;
          ep0_stage = MUSB_STAGE_SETUP;
      }
  }else{
     value.tx_csr1.tx_pkt_rdy = 1;
     //while(hpcd->hmusb->mode.peripheral.csr0_tx_csr1.tx_csr1.fifo_not_empty);//{//如果FIFO里面有数据，则等待FIFO数据完成再写
         //rt_kUSBD_DEBUG("fun=%s,%d %d\r\n",__func__, hpcd->hmusb->mode.peripheral.csr0_tx_csr1.tx_csr1.fifo_not_empty,hpcd->hmusb->mode.peripheral.csr0_tx_csr1.tx_csr1.tx_pkt_rdy);
     //}
     //hpcd->hmusb->mode.peripheral.csr0_tx_csr1.tx_csr1.flush_fifo = 1;//清除tx_pkt_rdy位和fifo,只有tx_pkt_rdy位为1的时候有效
  }
  writeFifo(ep_addr & 0x7f, (uint8_t *)buffer, size);
  USBD.hmusb->mode.peripheral.csr0_tx_csr1 = value;
  
  return size;
}
uint32_t USBDeviceClass::epReadPrepare(uint32_t ep_addr, void *buffer, uint32_t size){
  ep_addr &= 0x7f;
  USBD.hmusb->index = ep_addr;
  USBD.out_ep[ep_addr].buffer = (uint8_t *)buffer;
  USBD.out_ep[ep_addr].remain_size = size;
  return size;
}
int USBDeviceClass::epOpen(uint8_t ep_addr, uint8_t max_packet_size, uint8_t is_iso){
  USBD_DEBUG("%s,ep_addr=0x%X,is_iso=0x%X\n",__func__,ep_addr,is_iso);
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

USBDeviceClass USBDevice;

