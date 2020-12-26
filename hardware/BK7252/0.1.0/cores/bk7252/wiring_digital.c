/*
  Copyright (c) 2015 Arduino LLC.  All right reserved.
  Copyright (c) 2016 Sandeep Mistry All right reserved.

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


#include "Arduino.h"
#include "spi_pub.h"

#ifdef __cplusplus
extern "C" {
#endif

void pinMode( uint32_t ulPin, uint32_t ulMode )
{
     if(ulPin > (DIGITAL_PIN_NUMS - 1)) return;
     rt_pin_mode((rt_base_t)g_ADigitalPinMap[ulPin], (rt_base_t)ulMode);
      //param = 0x2;
        //spi_ctrl(CMD_SPI_SET_NSSID, (void *)&param);
}

void digitalWrite( uint32_t ulPin, uint32_t ulVal )
{
      uint32_t param = 0;
    if(ulPin > (DIGITAL_PIN_NUMS - 1)) return;
    //if(g_ADigitalPinMap[ulPin] == 15){
    //    rt_pin_write((rt_base_t)g_ADigitalPinMap[ulPin], (rt_base_t)ulVal);
    //    if(ulVal)  param = 0x3;
    //         else param = 0x2;
    //    spi_ctrl(CMD_SPI_SET_NSSID, (void *)&param);
   // }else{
           rt_pin_write((rt_base_t)g_ADigitalPinMap[ulPin], (rt_base_t)ulVal);
    //}
    
   
}

int digitalRead( uint32_t ulPin )
{
   if(ulPin > (DIGITAL_PIN_NUMS - 1)) return -1;
   return rt_pin_read((rt_base_t) g_ADigitalPinMap[ulPin]);
}

#ifdef __cplusplus
}
#endif
