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
#include "saradc.h"
#include "icu_pub.h"
#include "gpio_pub.h"
#include "arm_arch.h"
#include "saradc_pub.h"

#include "pwm_pub.h"

extern uint8_t getPinPWMChannel(uint8_t pin);
extern void analogReference(uint8_t mode);
extern void analogWrite( uint32_t ulPin, uint32_t ulValue );
extern uint32_t analogRead( uint32_t ulPin );
extern void toneFrequency(uint32_t ulPin, uint32_t freq);


#ifdef __cplusplus
}
#endif

#endif

