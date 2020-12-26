#ifndef __CDC_H__
#define __CDC_H__

#ifdef CDC_ENABLED

#include "USBDesc.h"
#include "USBAPI.h"
#include "PluggableUSB.h"
//#include "DFRobot_queue.h"



#define CDC_V1_10                               0x0110
#define CDC_COMMUNICATION_INTERFACE_CLASS       0x02

#define CDC_CALL_MANAGEMENT                     0x01
#define CDC_ABSTRACT_CONTROL_MODEL              0x02
#define CDC_HEADER                              0x00
#define CDC_ABSTRACT_CONTROL_MANAGEMENT         0x02
#define CDC_UNION                               0x06
#define CDC_CS_INTERFACE                        0x24
#define CDC_CS_ENDPOINT                         0x25
#define CDC_DATA_INTERFACE_CLASS                0x0A


#define USB_CDC_BUFSIZE                 0x40

#define USB_CDC_CLASS_COMM              0x02
#define USB_CDC_CLASS_DATA              0x0A

#define USB_CDC_SUBCLASS_NONE           0x00
#define USB_CDC_SUBCLASS_DLCM           0x01
#define USB_CDC_SUBCLASS_ACM            0x02
#define USB_CDC_SUBCLASS_TCM            0x03
#define USB_CDC_SUBCLASS_MCCM           0x04
#define USB_CDC_SUBCLASS_CCM            0x05
#define USB_CDC_SUBCLASS_ETH            0x06
#define USB_CDC_SUBCLASS_ATM            0x07
#define USB_CDC_SUBCLASS_EEM            0x0C

#define USB_CDC_PROTOCOL_NONE           0x00
#define USB_CDC_PROTOCOL_V25TER         0x01
#define USB_CDC_PROTOCOL_I430           0x30
#define USB_CDC_PROTOCOL_HDLC           0x31
#define USB_CDC_PROTOCOL_TRANS          0x32
#define USB_CDC_PROTOCOL_Q921M          0x50
#define USB_CDC_PROTOCOL_Q921           0x51
#define USB_CDC_PROTOCOL_Q921TM         0x52
#define USB_CDC_PROTOCOL_V42BIS         0x90
#define USB_CDC_PROTOCOL_Q931           0x91
#define USB_CDC_PROTOCOL_V120           0x92
#define USB_CDC_PROTOCOL_CAPI20         0x93
#define USB_CDC_PROTOCOL_HOST           0xFD
#define USB_CDC_PROTOCOL_PUFD           0xFE
#define USB_CDC_PROTOCOL_VENDOR         0xFF
#define USB_CDC_PROTOCOL_EEM            0x07

#define USB_CDC_CS_INTERFACE            0x24
#define USB_CDC_CS_ENDPOINT             0x25

#define USB_CDC_SCS_HEADER              0x00
#define USB_CDC_SCS_CALL_MGMT           0x01
#define USB_CDC_SCS_ACM                 0x02
#define USB_CDC_SCS_UNION               0x06
#define USB_CDC_SCS_ETH                 0x0F

#define CDC_SEND_ENCAPSULATED_COMMAND   0x00
#define CDC_GET_ENCAPSULATED_RESPONSE   0x01
#define CDC_SET_COMM_FEATURE            0x02
#define CDC_GET_COMM_FEATURE            0x03
#define CDC_CLEAR_COMM_FEATURE          0x04
#define CDC_SET_AUX_LINE_STATE          0x10
#define CDC_SET_HOOK_STATE              0x11
#define CDC_PULSE_SETUP                 0x12
#define CDC_SEND_PULSE                  0x13
#define CDC_SET_PULSE_TIME              0x14
#define CDC_RING_AUX_JACK               0x15
#define CDC_SET_LINE_CODING             0x20
#define CDC_GET_LINE_CODING             0x21
#define CDC_SET_CONTROL_LINE_STATE      0x22
#define CDC_SEND_BREAK                  0x23
#define CDC_SET_RINGER_PARMS            0x30
#define CDC_GET_RINGER_PARMS            0x31
#define CDC_SET_OPERATION_PARMS         0x32
#define CDC_GET_OPERATION_PARMS         0x33
#define CDC_SET_LINE_PARMS              0x34
#define CDC_GET_LINE_PARMS              0x35
#define CDC_DIAL_DIGITS                 0x36
#define CDC_SET_UNIT_PARAMETER          0x37
#define CDC_GET_UNIT_PARAMETER          0x38
#define CDC_CLEAR_UNIT_PARAMETER        0x39
#define CDC_GET_PROFILE                 0x3A
#define CDC_SET_ETH_MULTICAST_FILTERS   0x40
#define CDC_SET_ETH_POWER_MGMT_FILT     0x41
#define CDC_GET_ETH_POWER_MGMT_FILT     0x42
#define CDC_SET_ETH_PACKET_FILTER       0x43
#define CDC_GET_ETH_STATISTIC           0x44
#define CDC_SET_ATM_DATA_FORMAT         0x50
#define CDC_GET_ATM_DEVICE_STATISTICS   0x51
#define CDC_SET_ATM_DEFAULT_VC          0x52
#define CDC_GET_ATM_VC_STATISTICS       0x53

#define CDC_SERIAL_BUFFER_SIZE  256



struct queueData{
  struct queueData *next;
  struct queueData *before;
  uint8_t len;
  uint8_t data[];
};

typedef struct queueData sQueueData_t;
extern uint32_t cdcTXQueueData;
extern uint32_t cdcRXQueueData;
void dataEnqueue(uint8_t *pbuf, uint16_t len);
sQueueData_t *dataFromHeadDequeue();
sQueueData_t *dataFromTailDequeue();
uint8_t getTailDataSize();
//void dataRecvEnqueue(uint8_t *pbuf, uint16_t len);



//  CDC CS interface descriptor
typedef struct
{
    uint8_t len;        // 5
    uint8_t dtype;      // 0x24
    uint8_t subtype;
    uint8_t d0;
    uint8_t d1;
} CDCCSInterfaceDescriptor;

typedef struct
{
    uint8_t len;        // 4
    uint8_t dtype;      // 0x24
    uint8_t subtype;
    uint8_t d0;
} CDCCSInterfaceDescriptor4;

typedef struct
{
    uint8_t len;
    uint8_t     dtype;      // 0x24
    uint8_t     subtype;    // 1
    uint8_t     bmCapabilities;
    uint8_t     bDataInterface;
} CMFunctionalDescriptor;

typedef struct
{
    uint8_t len;
    uint8_t     dtype;      // 0x24
    uint8_t     subtype;    // 1
    uint8_t     bmCapabilities;
} ACMFunctionalDescriptor;

typedef struct
{
    //  IAD
    IADDescriptor               iad;    // Only needed on compound device
    //  Control
    InterfaceDescriptor         cif;
    CDCCSInterfaceDescriptor    header;
    ACMFunctionalDescriptor     controlManagement;      // ACM
    CDCCSInterfaceDescriptor    functionalDescriptor;   // CDC_UNION
    CMFunctionalDescriptor      callManagement;         // Call Management
    EndpointDescriptor          cifin;

    //  Data
    InterfaceDescriptor         dif;
    EndpointDescriptor          in;
    EndpointDescriptor          out;
} CDCDescriptor;
//ring buffer

typedef struct{
  uint16_t head;
  uint16_t tail;
  uint8_t flag;//0表示空，1表示满
  uint8_t *buf;
}ringBuffer_t;



//================================================================================
//  Serial over CDC (Serial1 is the physical port)

class Serial_ : public Stream, public PluggableUSBModule {
public:
    Serial_(USBDeviceClass &_usb);

    void begin(uint32_t baud_count);
    void end(void);

    virtual int available(void);
    virtual int peek(void);
    virtual int read(void);
    size_t read(void *pBuf, size_t size);
    virtual void flush(void);
    virtual size_t write(uint8_t);
    virtual size_t write(const uint8_t *buffer, size_t size);
    using Print::write; // pull in write(str) from Print
    operator bool();

    size_t readBytes(char *buffer, size_t length);

    // These return the settings specified by the USB host for the
    // serial port. These aren't really used, but are offered here
    // in case a sketch wants to act on these settings.
    uint32_t baud();
    uint8_t stopbits();
    uint8_t paritytype();
    uint8_t numbits();
    bool dtr();
    bool rts();
    enum {
        ONE_STOP_BIT = 0,
        ONE_AND_HALF_STOP_BIT = 1,
        TWO_STOP_BITS = 2,
    };
    enum {
        NO_PARITY = 0,
        ODD_PARITY = 1,
        EVEN_PARITY = 2,
        MARK_PARITY = 3,
        SPACE_PARITY = 4,
    };
    void handleCDCISR();
protected:
    // Implementation of the PUSBListNode
    int getInterface(uint8_t* interfaceNum);
    int getDescriptor(USBSetup& setup);
    bool setup(USBSetup& setup);
    uint8_t getShortName(char* name);
    void handleEndpoint(int ep);
  

friend USBDeviceClass;

private:
    int availableForStore(void);

    USBDeviceClass &usb;
    bool stalled;
    
  
  uint16_t headIndex;
  uint16_t tailIndex;
  uint8_t cdcSerialRecvBuf[CDC_SERIAL_BUFFER_SIZE];
  uint32_t epType[3];//用于存放3个端点的类型
};

extern Serial_ SerialUSB;
#define Serial SerialUSB

#endif
#endif