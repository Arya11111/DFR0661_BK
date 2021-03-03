/* -*- mode: jde; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  Part of the Wiring project - http://wiring.uniandes.edu.co

  Copyright (c) 2004-05 Hernando Barragan

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General
  Public License along with this library; if not, write to the
  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
  Boston, MA  02111-1307  USA
  
  Modified 24 November 2006 by David A. Mellis
  Modified 1 August 2010 by Mark Sproul
*/

#include "WInterrupts.h"
#include "Arduino.h"
typedef struct gpioIrqDesc{
  voidFuncPtr handler;
  uint8_t mode;
  bool changeFlag;
}sGpioIrqDesc_t;
#ifdef __cplusplus
extern "C" {
#endif

//static uint8_t PIN_INT_CHANGE_FLAG[GPIONUM] = {0};
//typedef void (*gpioIsrHandler_t)();

static sGpioIrqDesc_t gpioIrqDescription[GPIONUM] = {0};
static void gpioIRQDispatch(unsigned char index){
  if(index >= GPIONUM) return;
  if(gpioIrqDescription[index].changeFlag){
      if(bk_gpio_input(index)){
          gpioIrqDescription[index].mode = GPIO_INT_LEVEL_FALLING;
      }else{
          gpioIrqDescription[index].mode = GPIO_INT_LEVEL_RISING ;
      }
      gpio_int_enable(index, gpioIrqDescription[index].mode, gpioIRQDispatch);
  }
  if(gpioIrqDescription[index].handler != NULL){
      (gpioIrqDescription[index].handler)();
  }
}

void attachInterrupt(uint32_t interruptNum, voidFuncPtr callback, uint32_t mode){
  uint32_t ulPin = interruptNum;
  if(ulPin >= PINS_COUNT) return;
  if( g_APinDescription[ulPin].ulPinType == PIO_NOT_A_PIN ) return;
  if(!(g_APinDescription[ulPin].ulAttribute & PIN_ATTR_DIGITAL)) return;
  GPIO_INDEX pin_ = (GPIO_INDEX)g_APinDescription[ulPin].ulPin;
  if(pin_ >= GPIONUM) return;
  gpioIrqDescription[pin_].handler = callback;
  gpioIrqDescription[pin_].changeFlag = false;
  switch(mode){
      case LOW: gpioIrqDescription[pin_].mode = GPIO_INT_LEVEL_LOW;
                break;
      case HIGH: gpioIrqDescription[pin_].mode = GPIO_INT_LEVEL_HIGH;
                break;
      case CHANGE: 
                gpio_config(pin_, GMODE_INPUT);
                if(bk_gpio_input(pin_)){
                    gpioIrqDescription[pin_].mode = GPIO_INT_LEVEL_FALLING ;
                }else{
                    gpioIrqDescription[pin_].mode = GPIO_INT_LEVEL_RISING;
                }
                gpioIrqDescription[pin_].changeFlag = true;
                break;
      case FALLING: gpioIrqDescription[pin_].mode = GPIO_INT_LEVEL_FALLING;
                break;
      case RISING: gpioIrqDescription[pin_].mode = GPIO_INT_LEVEL_RISING;
                break;
      default:
            memset(&gpioIrqDescription[pin_], 0, sizeof(sGpioIrqDesc_t));
            return;
  }
  gpio_int_enable(pin_, gpioIrqDescription[pin_].mode, gpioIRQDispatch);
}

void detachInterrupt(uint32_t interruptNum) {
  uint32_t ulPin = interruptNum;
  if(ulPin >= PINS_COUNT) return;
  if( g_APinDescription[ulPin].ulPinType == PIO_NOT_A_PIN ) return;
  if(!(g_APinDescription[ulPin].ulAttribute & PIN_ATTR_DIGITAL)) return;
  GPIO_INDEX pin_ = (GPIO_INDEX)g_APinDescription[ulPin].ulPin;
  if(pin_ >= GPIONUM) return;
  gpio_int_disable(pin_);
  memset(&gpioIrqDescription[pin_], 0, sizeof(sGpioIrqDesc_t));
}

#ifdef __cplusplus
}
#endif

/*
ISR(TWI_vect) {
  if(twiIntFunc)
    twiIntFunc();
}
*/

