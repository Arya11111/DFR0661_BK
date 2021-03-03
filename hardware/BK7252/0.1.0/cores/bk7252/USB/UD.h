#ifndef __UD_H__
#define __UD_H__

#include "USBDesc.h"
#include "USBAPI.h"
#include "PluggableUSB.h"
#include "USBCore.h"

class UDClass;
typedef enum {
  SCSI_STAT_CBW,
  SCSI_STAT_CMD,
  SCSI_STAT_CSW,
  SCSI_STAT_RECEIVE,
  SCSI_STAT_SEND
}eSCSIState_t;

typedef enum
{
    DIR_IN,
    DIR_OUT,
    DIR_NONE,
}eCB_DIR_t;


typedef enum
{
    FIXED,
    COUNT,
    BLOCK_COUNT,
}eCB_SIZE_TYPE_t;

typedef int (UDClass::*cbwHandler)();
typedef struct
{
    rt_uint16_t cmd;
    rt_size_t cmd_len;
    eCB_SIZE_TYPE_t type;
    rt_size_t data_size;
    eCB_DIR_t dir;
}sSCSICmd_t, *pSCSICmd_t;

typedef struct{
  uint8_t ErrorCode:7;
  uint8_t Valid:1;
  uint8_t Reserved1;
  uint8_t SenseKey:4;
  uint8_t Reserved2:4;
  uint8_t Information[4];
  uint8_t AdditionalSenseLength;
  uint8_t Reserved3[4];
  uint8_t AdditionalSenseCode;
  uint8_t AdditionalSenseCodeQualifier;
  uint8_t Reserved4[4];
}sRequestSenseData_t, *pRequestSenseData_t;

typedef struct
{
    //  IAD
    IADDescriptor               iad;    // Only needed on compound device  相关描述符

    //  Data
    InterfaceDescriptor         dif;  //接口描述符
    EndpointDescriptor          in;  //端点描述符 bulk in
    EndpointDescriptor          out; //端点描述符 bulk out
} UDDescriptor;

typedef struct{
  struct ustorage_csw csw_response;
  int status;
  
  
  uint32_t cbDataSize;
  rt_device_t disk;
  uint32_t block;
  int32_t count;
  int32_t size;
  struct rt_device_blk_geometry geometry;
  uint16_t epInIndex;
  uint16_t epOutIndex;
  uint8_t *epBuf;
  uint8_t *epOutBuf;
}sMstorage_t, *pMstorage_t;

class UDClass :public PluggableUSBModule{// U盘有1个接口 2个端点(输入输出)
public:
  UDClass(void);
  void begin(void);
protected:
  // Implementation of the PUSBListNode
  int getInterface(uint8_t* interfaceNum);
  int getDescriptor(USBSetup& setup);
  bool setup(USBSetup& setup);
  uint8_t getShortName(char* name);
  void handleEndpoint(int ep);
  int epOutHandler(uint32_t ep);
  int epInHandler(uint32_t ep);
  
  
  int testUnitReady();
  int requestSense();
  int inquiryCmd();
  int allowRemoval();
  int startStop();
  int modeSense6();
  int readCapacities();
  int readCapacity();
  int read10();
  int write10();
  int verify10();
  pSCSICmd_t findCbwCommand(rt_uint16_t cmd);
  int callbackCBWHandler(rt_uint16_t cmd);
  void cbLenCalc(pSCSICmd_t cmd);
  bool cbwVerify(pSCSICmd_t cmd);
  void sendStatus();
  
private:
  uint32_t epType[2];//用于存放2个端点的类型
  eSCSIState_t _scsiState;
  uint32_t _errorBlock;
  struct ustorage_cbw _cbw;
  sMstorage_t _udData;
  static sSCSICmd_t _cmdData[11];

};

extern UDClass UD;

#endif