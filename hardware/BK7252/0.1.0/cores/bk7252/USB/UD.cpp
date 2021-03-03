#include "UD.h"
extern "C" {
#include "player.h"
}

#define UD_DATA_INTERFACE  pluggedInterface
#define UD_ENDPOINT_IN  pluggedEndpoint
#define UD_ENDPOINT_OUT  pluggedEndpoint + 1
static uint8_t UDSTOPFLAG = 0;


sSCSICmd_t  UDClass::_cmdData[11] = {
      {SCSI_TEST_UNIT_READY,  6,  FIXED,       0, DIR_NONE},
      {SCSI_REQUEST_SENSE,    6,  COUNT,       0, DIR_IN},
      {SCSI_INQUIRY_CMD,      6,  COUNT,       0, DIR_IN},
      {SCSI_ALLOW_REMOVAL,    6,  FIXED,       0, DIR_NONE},
      {SCSI_MODE_SENSE_6,     6,  COUNT,       0, DIR_IN},
      {SCSI_START_STOP,       6,  FIXED,       0, DIR_NONE},
      {SCSI_READ_CAPACITIES,  10, COUNT,       0, DIR_NONE},
      {SCSI_READ_CAPACITY,    10, FIXED,       8, DIR_IN},
      {SCSI_READ_10,          10, BLOCK_COUNT, 0, DIR_IN},
      {SCSI_WRITE_10,         10, BLOCK_COUNT, 0, DIR_OUT},
      {SCSI_VERIFY_10,        10, FIXED,       0, DIR_NONE}
};

UDClass::UDClass(void) : PluggableUSBModule(2, 1, epType),_scsiState(SCSI_STAT_CBW)
{
  endPointPrams_t epArgs;
  epArgs.params.type = USB_ENDPOINT_TYPE_BULK;
  epArgs.params.index = USB_ENDPOINT_IN(0);
  epArgs.params.packetSize = 64;
  epArgs.params.iso = 0;
  epType[0] = epArgs.epType;
  
  epArgs.params.index = USB_ENDPOINT_OUT(0);
  epType[1] = epArgs.epType;
  PluggableUSB().plug(this);
  memset(&_udData, 0, sizeof(_udData));
  _udData.status = SCSI_STAT_CBW;

  
}



void UDClass::begin(void){
  _udData.disk = rt_device_find("sd0");
  if(_udData.disk == NULL){
    //USBD_DEBUG("no disk\n");
  }else{
    //USBD_DEBUG("disk sd0\n");
  }
  if(rt_device_init(_udData.disk) != RT_EOK)
   {
        //USBD_DEBUG("disk init error\n");
      return ;
   }
  

  if(rt_device_open(_udData.disk, RT_DEVICE_OFLAG_RDWR) != RT_EOK)
  {
       //USBD_DEBUG("disk open error\n");
      //return;
  }
   if(rt_device_control(_udData.disk, RT_DEVICE_CTRL_BLK_GETGEOME, (void*)&_udData.geometry) != RT_EOK)
   {
      //USBD_DEBUG("get disk info error\n");
       return;
   }
   _udData.epBuf = (rt_uint8_t*)rt_malloc(_udData.geometry.bytes_per_sector);
  
  if(_udData.epBuf == RT_NULL)
  {
      //USBD_DEBUG("no memory\n");
      return;
  }
   _udData.epOutBuf = (rt_uint8_t*)rt_malloc(_udData.geometry.bytes_per_sector);
  if(_udData.epOutBuf == RT_NULL)
  {
      //USBD_DEBUG("no memory\n");
      return;
  }
  //USBD_DEBUG("sector_count=%d, bytes_per_sector=%d, block_size=%d\n",_udData.geometry.sector_count,_udData.geometry.bytes_per_sector,_udData.geometry.block_size);
  _udData.status = SCSI_STAT_CBW;
  UDSTOPFLAG = 0;
}



int UDClass::getInterface(uint8_t* interfaceNum)//U盘有1个接口
{
  interfaceNum[0] += 1;   // uses 2
  UDDescriptor _udInterface = {
      D_IAD(pluggedInterface, 1, 0x08, 0x06, 0x50),
      // CDC data interface
      D_INTERFACE(UD_DATA_INTERFACE, 2, 0x08, 0x06, 0x50),
      D_ENDPOINT(USB_ENDPOINT_IN(UD_ENDPOINT_IN), USB_ENDPOINT_TYPE_BULK, EPX_SIZE, 0),
      D_ENDPOINT(USB_ENDPOINT_OUT(UD_ENDPOINT_OUT), USB_ENDPOINT_TYPE_BULK, EPX_SIZE, 0)
  };
  return USBDevice.sendControl(&_udInterface, sizeof(_udInterface));
}

int UDClass::getDescriptor(USBSetup& setup){
  
}

bool UDClass::setup(USBSetup& setup){
  uint8_t lun = 0;
  
  switch(setup.bRequest)
  {
  case USBREQ_GET_MAX_LUN:        
      //USBDevice.packMessages(false);
      USBDevice.packMessages(true);
      USBDevice.sendControl(&lun, ((setup.wLength > 1) ? 1 : setup.wLength));
      USBDevice.packMessages(false);
      break;
  case USBREQ_MASS_STORAGE_RESET:
      //USBD_DEBUG("USBREQ_MASS_STORAGE_RESET\n");
      break;
  default:
      //USBD_DEBUG("unknown interface request\n");
      break;
  }

  return true;
}

uint8_t UDClass::getShortName(char* name){
  return 1;
}

void UDClass::handleEndpoint(int ep){
  //1判断端点的方向
  if((ep & 0x7F) == UD_ENDPOINT_OUT){
      epOutHandler(ep);
  }else{
      epInHandler(ep);
  }
}

int UDClass::epOutHandler(uint32_t ep){
  uint8_t epIndex = ep & 0x7f;
  if(_udData.status == SCSI_STAT_CBW){
      pSCSICmd_t cmd;
      uint8_t *pBuf = (uint8_t *)&_cbw;
      int len;
      uint8_t size = USBDevice.recv(epIndex, pBuf, sizeof(struct ustorage_cbw));
      if(_cbw.signature != CBW_SIGNATURE || size != SIZEOF_CBW)
      {
          //goto exit;
      }
      _udData.csw_response.signature = CSW_SIGNATURE;
      _udData.csw_response.tag = _cbw.tag;
      _udData.csw_response.data_reside = _cbw.xfer_len;
      _udData.csw_response.status = 0;
      cmd = findCbwCommand(_cbw.cb[0]);
      if(cmd == NULL)
      {
            //USBD_DEBUG("can't find cbw command  cmd=0x%x\n", _cbw.cb[0]);
            goto exit;
      }
      cbLenCalc(cmd);
      if(!cbwVerify(cmd)) goto exit;
      len = callbackCBWHandler(_cbw.cb[0]);
      if(len == 0){
          sendStatus();
          if(UDSTOPFLAG == 1) rt_hw_cpu_reset();
      }
      return 0;
  }else if(_udData.status == SCSI_STAT_RECEIVE){
      if(USB_UD_TEMPOARY_FLAG + 1 > 255){
          USB_UD_ENABLE_FLAG = 0;
      }else{
          USB_UD_ENABLE_FLAG = USB_UD_TEMPOARY_FLAG + 1;
      }
      if(_udData.csw_response.data_reside){
          _udData.epOutIndex += USBDevice.recv(ep, _udData.epOutBuf+_udData.epOutIndex, 512 - _udData.epOutIndex);
          if(_udData.epOutIndex == 512){
              _udData.epOutIndex = 0;
              if(rt_device_write(_udData.disk, _udData.block, _udData.epOutBuf, 1) != 1){
                  _udData.csw_response.status = 1;
                  _errorBlock = _udData.block;
              }
                  
              memset(_udData.epOutBuf, 0, 512);
              _udData.csw_response.data_reside -= _udData.geometry.bytes_per_sector;
              _udData.block++;
          }
          if(_udData.csw_response.data_reside == 0){
              sendStatus();
          }
      }else{
          sendStatus();
      }
      return 0;
  }
  exit:
  _udData.csw_response.status = 1;
  sendStatus();
  
  return -1;
  
}
int UDClass::epInHandler(uint32_t ep){
  //rt_enter_critical();
  switch(_udData.status){
      case SCSI_STAT_CSW:
          _udData.status = SCSI_STAT_CBW;
          break;
      case SCSI_STAT_CMD:
          _udData.csw_response.data_reside = 0;
          sendStatus();
          break;
      case SCSI_STAT_SEND:
          
          if(USB_UD_TEMPOARY_FLAG + 1 > 255){
              USB_UD_ENABLE_FLAG = 0;
          }else{
              USB_UD_ENABLE_FLAG = USB_UD_TEMPOARY_FLAG + 1;
          }
          _udData.csw_response.data_reside -= _udData.epInIndex;
          _udData.epInIndex = 0;
          memset(_udData.epBuf, 0, _udData.geometry.bytes_per_sector);
          if(_udData.csw_response.data_reside){
              _udData.count--;
              if(_udData.count){
                  _udData.block++;
                  //rt_device_read(_udData.disk, _udData.block, _udData.epBuf, 1);
                  uint32_t size = rt_device_read(_udData.disk, _udData.block, _udData.epBuf, 1);
                  if(rt_device_read(_udData.disk, _udData.block, _udData.epBuf, 1) != 1){
                      //USBD_DEBUG("++read data error\n");
                      memset(_udData.epBuf, 0, 512);
                      _udData.csw_response.status = 1;
                      _errorBlock = _udData.block - 1;
                  }
                  _udData.epInIndex = _udData.geometry.bytes_per_sector;
                  USBDevice.sendControl(UD_ENDPOINT_IN, _udData.epBuf, _udData.epInIndex);
              }
          }else{
              sendStatus();
          }

  }
  //rt_exit_critical();
  return 0;
}


int UDClass::testUnitReady(){
  _udData.csw_response.status = 0;
  return 0;
}
int UDClass::requestSense(){
  pRequestSenseData_t buf = (pRequestSenseData_t)_udData.epBuf;
  buf->ErrorCode = 0x70;
  buf->Valid = 0;
  buf->SenseKey = 2;
  buf->Information[0] = 0;
  buf->Information[1] = 0;
  buf->Information[2] = 0;
  buf->Information[3] = 0;
  buf->AdditionalSenseLength = 0x0a;
  buf->AdditionalSenseCode   = 0x3a;
  buf->AdditionalSenseCodeQualifier = 0;

  _udData.cbDataSize = MIN(_udData.cbDataSize , SIZEOF_REQUEST_SENSE);
  USBDevice.sendControl(UD_ENDPOINT_IN, _udData.epBuf, _udData.cbDataSize);
  _udData.status = SCSI_STAT_CMD;
  return _udData.cbDataSize;
}
int UDClass::inquiryCmd(){
  if(_udData.epBuf == NULL) return 0;
  *(rt_uint32_t*)&_udData.epBuf[0] = 0x0 | (0x80 << 8);
  *(rt_uint32_t*)&_udData.epBuf[4] = 31;
  memset(&_udData.epBuf[8], 0x20, 28);
  memcpy(&_udData.epBuf[8], "DFRobot", 7);
  memcpy(&_udData.epBuf[16], "BK7252 UD", 9);
  memcpy(&_udData.epBuf[32], "V1.0", 4);
  _udData.cbDataSize = MIN(_udData.cbDataSize , SIZEOF_INQUIRY_CMD);
  USBDevice.sendControl(UD_ENDPOINT_IN, _udData.epBuf, _udData.cbDataSize);
  _udData.status = SCSI_STAT_CMD;
  return _udData.cbDataSize;
}
int UDClass::allowRemoval(){
  _udData.csw_response.status = 0;
  return 0;
}
int UDClass::startStop(){
  UDSTOPFLAG = 1;
  _udData.csw_response.status = 0;
  return 0;
}
int UDClass::modeSense6(){
  _udData.epBuf[0]=3;
  _udData.epBuf[1]=0;
  _udData.epBuf[2]=0;
  _udData.epBuf[3]=0;
  
  _udData.cbDataSize = MIN(_udData.cbDataSize , SIZEOF_MODE_SENSE_6);
  USBDevice.sendControl(UD_ENDPOINT_IN, _udData.epBuf, _udData.cbDataSize);
  _udData.status = SCSI_STAT_CMD;
  return _udData.cbDataSize;
}
int UDClass::readCapacities(){
  *(rt_uint32_t*)&_udData.epBuf[0] = 0x08000000;
  _udData.epBuf[4]=0xff & (_udData.geometry.sector_count>>24);
  _udData.epBuf[5]=0xff & (_udData.geometry.sector_count>>16);
  _udData.epBuf[6]=0xff & (_udData.geometry.sector_count>>8);
  _udData.epBuf[7]=0xff & (_udData.geometry.sector_count>>0);
  _udData.epBuf[8]=3;
  _udData.epBuf[9]=0xff & (_udData.geometry.bytes_per_sector>>16);
  _udData.epBuf[10]=0xff & (_udData.geometry.bytes_per_sector>>8);
  _udData.epBuf[11]=0xff & (_udData.geometry.bytes_per_sector>>0);
  _udData.cbDataSize = MIN(_udData.cbDataSize , SIZEOF_READ_CAPACITIES);
  USBDevice.sendControl(UD_ENDPOINT_IN, _udData.epBuf, _udData.cbDataSize);
  _udData.status = SCSI_STAT_CMD;
  return _udData.cbDataSize;
}
int UDClass::readCapacity(){
  _udData.epBuf[0]=0xff & (_udData.geometry.sector_count>>24);
  _udData.epBuf[1]=0xff & (_udData.geometry.sector_count>>16);
  _udData.epBuf[2]=0xff & (_udData.geometry.sector_count>>8);
  _udData.epBuf[3]=0xff & (_udData.geometry.sector_count>>0);
  _udData.epBuf[4]=0xff & (_udData.geometry.bytes_per_sector>>24);
  _udData.epBuf[5]=0xff & (_udData.geometry.bytes_per_sector>>16);
  _udData.epBuf[6]=0xff & (_udData.geometry.bytes_per_sector>>8);
  _udData.epBuf[7]=0xff & (_udData.geometry.bytes_per_sector>>0);
  _udData.cbDataSize = MIN(_udData.cbDataSize , SIZEOF_READ_CAPACITY);
  USBDevice.sendControl(UD_ENDPOINT_IN, _udData.epBuf, _udData.cbDataSize);
  _udData.status = SCSI_STAT_CMD;
  return _udData.cbDataSize;
}
int UDClass::read10(){
  _udData.block = _cbw.cb[2]<<24 | _cbw.cb[3]<<16 | _cbw.cb[4]<<8  | _cbw.cb[5]<<0;
  _udData.count = _cbw.cb[7]<<8 | _cbw.cb[8]<<0;
  if(_udData.count  > _udData.geometry.sector_count) return 0;
  _udData.csw_response.data_reside = _udData.cbDataSize;
  if(USB_UD_TEMPOARY_FLAG + 1 > 255){
      USB_UD_ENABLE_FLAG = 0;
  }else{
      USB_UD_ENABLE_FLAG = USB_UD_TEMPOARY_FLAG + 1;
  }
  
  
  uint32_t size = rt_device_read(_udData.disk, _udData.block, _udData.epBuf, 1);
  if(size == 0){
      //USBD_DEBUG("read data error\n");
      memset(_udData.epBuf, 0, 512);
      _udData.csw_response.status = 1;
      _errorBlock = _udData.block;
  }
  USBDevice.sendControl(UD_ENDPOINT_IN, _udData.epBuf, 512);
  _udData.epInIndex = _udData.geometry.bytes_per_sector;
  _udData.status = SCSI_STAT_SEND;
  return _udData.geometry.bytes_per_sector;
}
int UDClass::write10(){
  _udData.block = _cbw.cb[2]<<24 | _cbw.cb[3]<<16 | _cbw.cb[4]<<8  | _cbw.cb[5]<<0;
  _udData.count = _cbw.cb[7]<<8 | _cbw.cb[8]<<0;
  _udData.csw_response.data_reside = _cbw.xfer_len;
  
  if(USB_UD_TEMPOARY_FLAG + 1 > 255){
      USB_UD_ENABLE_FLAG = 0;
  }else{
      USB_UD_ENABLE_FLAG = USB_UD_TEMPOARY_FLAG + 1;
  }
  
  _udData.size = _udData.count * _udData.geometry.bytes_per_sector;
  _udData.epOutIndex = 0;
  _udData.status = SCSI_STAT_RECEIVE;
  _udData.epOutIndex += USBDevice.recv(UD_ENDPOINT_OUT, _udData.epOutBuf, 512);

  if(_udData.epOutIndex == 512){
      if(rt_device_write(_udData.disk, _udData.block, _udData.epOutBuf, 1) != 1){
          _udData.csw_response.status = 1;
          _errorBlock = _udData.block;
      }
      _udData.csw_response.data_reside -= 512;
      _udData.block++;
      _udData.epOutIndex = 0;
  }
  return _udData.csw_response.data_reside;
}
int UDClass::verify10(){
  _udData.csw_response.status = 0;
  return 0;
}

int UDClass::callbackCBWHandler(rt_uint16_t cmd){
  eCB_SIZE_TYPE_t type;
  uint8_t cmd_len = 6;
  uint8_t dataSize = 0;
  switch(cmd){
      case SCSI_TEST_UNIT_READY:
           return testUnitReady();
      case SCSI_REQUEST_SENSE:
           return requestSense();
      case SCSI_INQUIRY_CMD:
           return inquiryCmd();
      case SCSI_ALLOW_REMOVAL:
           return allowRemoval();
      case SCSI_MODE_SENSE_6:
           return modeSense6();
      case SCSI_START_STOP:
           return startStop();
      case SCSI_READ_CAPACITIES:
           return readCapacities();
      case SCSI_READ_CAPACITY:
           return readCapacity();
      case SCSI_READ_10:
           return read10();
      case SCSI_WRITE_10:
           return write10();
      case SCSI_VERIFY_10:
           return verify10();

  }
}

pSCSICmd_t UDClass::findCbwCommand(rt_uint16_t cmd){
  int i;

    for(i=0; i<sizeof(_cmdData)/sizeof(sSCSICmd_t); i++)
    {
        if(_cmdData[i].cmd == cmd)
            return &_cmdData[i];
    }

    return NULL;
}
void UDClass::cbLenCalc(pSCSICmd_t cmd){
  if(cmd->cmd_len == 6)
  {
      switch(cmd->type)
      {
      case COUNT:
          _udData.cbDataSize = _cbw.cb[4];
          break;
      case BLOCK_COUNT:
          _udData.cbDataSize = _cbw.cb[4] * _udData.geometry.bytes_per_sector;
          break;
      case FIXED:
          _udData.cbDataSize = cmd->data_size;
          break;
      default:
          break;
      }
  }else if(cmd->cmd_len == 10){
      switch(cmd->type)
        {
        case COUNT:
            _udData.cbDataSize = _cbw.cb[7]<<8 | _cbw.cb[8];
            break;
        case BLOCK_COUNT:
            _udData.cbDataSize = (_cbw.cb[7]<<8 | _cbw.cb[8]) * _udData.geometry.bytes_per_sector;
            break;
        case FIXED:
            _udData.cbDataSize = cmd->data_size;
            break;
        default:
            break;
        }
  }
}

bool UDClass::cbwVerify(pSCSICmd_t cmd){
  if(cmd->cmd_len != _cbw.cb_len){
      cmd->cmd_len = _cbw.cb_len;
  }
  
  if(_cbw.cb[0]==0x12 && _cbw.cb_len==12){
        _udData.cbDataSize = 56;
    }
  
  if(_cbw.xfer_len > 0 && _udData.cbDataSize == 0)
  {
      return false;
  }

  if(_cbw.xfer_len == 0 && _udData.cbDataSize > 0)
  {
      return false;
  }

  if((_cbw.dflags & USB_DIR_IN) && cmd->dir == DIR_OUT || !(_cbw.dflags & USB_DIR_IN) && cmd->dir == DIR_IN)
  {
      return false;
  }

  if(_cbw.xfer_len > _udData.cbDataSize)
  {
      return false;
  }
  
  if(_cbw.xfer_len < _udData.cbDataSize)
  {
      _udData.cbDataSize = _cbw.xfer_len;
      _udData.csw_response.status = 1;
  }

  return true; 
}

void UDClass::sendStatus(){
  USBDevice.sendControl(UD_ENDPOINT_IN, &_udData.csw_response, 13);
  _udData.status = SCSI_STAT_CSW;
}

UDClass UD;

