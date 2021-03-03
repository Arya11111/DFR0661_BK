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
#include "gpio_pub.h"

#ifdef __cplusplus
extern "C" {
#endif

void pinMode( uint32_t ulPin, uint32_t ulMode )
{
  if(ulPin >= PINS_COUNT) return;
  if( g_APinDescription[ulPin].ulPinType == PIO_NOT_A_PIN ) return;
  if(!(g_APinDescription[ulPin].ulAttribute & PIN_ATTR_DIGITAL)) return;
  GPIO_INDEX pin_ = (GPIO_INDEX)g_APinDescription[ulPin].ulPin;
  if(pin_ >= GPIONUM) return;
  switch (ulMode)
  {
      case INPUT:
          bk_gpio_config_input(pin_);
          break;
      
      case OUTPUT:
          bk_gpio_config_output(pin_);
          break;
      
      case INPUT_PULLUP:
          bk_gpio_config_input_pup(pin_);
          break;
      
      case INPUT_PULLDOWN:
          bk_gpio_config_input_pdwn(pin_);
          break;
  }
}

void digitalWrite( uint32_t ulPin, uint32_t ulVal )
{
  if(ulPin >= PINS_COUNT) return;
  if( g_APinDescription[ulPin].ulPinType == PIO_NOT_A_PIN ) return;
  if(!(g_APinDescription[ulPin].ulAttribute & PIN_ATTR_DIGITAL)) return;
  GPIO_INDEX pin_ = (GPIO_INDEX)g_APinDescription[ulPin].ulPin;
  if(pin_ >= GPIONUM) return;
  bk_gpio_output(pin_, ulVal);
}

int digitalRead( uint32_t ulPin )
{
  if(ulPin >= PINS_COUNT) return -1;
  if( g_APinDescription[ulPin].ulPinType == PIO_NOT_A_PIN ) return -1;
  if(!(g_APinDescription[ulPin].ulAttribute & PIN_ATTR_DIGITAL)) return -1;
  GPIO_INDEX pin_ = (GPIO_INDEX)g_APinDescription[ulPin].ulPin;
  if(pin_ >= GPIONUM) return -1;
  return bk_gpio_input(pin_);
}

#ifdef __cplusplus
}
#endif
