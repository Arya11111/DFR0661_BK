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

#ifndef __WIRING_ANALOG_H
#define __WIRING_ANALOG_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
#include "saradc_intf.h"
struct analogPWMArgs{
  uint8_t channel;
  uint32_t frequency;
  uint32_t resolution;
};
extern uint8_t getPinPWMChannel(uint8_t pin);
extern void analogReference(uint8_t mode);
extern void analogWrite( uint32_t ulPin, uint32_t ulValue );
extern uint32_t analogRead( uint32_t ulPin );
extern void analogReadResolution(int res);
extern void analogWriteResolution(uint8_t pin,uint32_t res);
extern void analogOutputInit( void );
extern void setPWMFrequency(uint8_t pin, uint32_t freq_Hz);
extern void PWMInit();
extern void setPWMArguments(uint8_t pin, uint32_t freq_Hz, uint32_t res);


#ifdef __cplusplus
}
#endif

#endif

