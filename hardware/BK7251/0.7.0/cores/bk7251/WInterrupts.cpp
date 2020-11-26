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

#ifdef __cplusplus
extern "C" {
#endif
#ifdef __cplusplus
}
#endif

void attachInterrupt_ISR(void* args){
  ((voidFuncPtr)args)();
}

void attachInterrupt(uint32_t interruptNum, void (*hdr)(void *args), uint32_t mode, void *args){
  if(interruptNum > DIGITAL_PIN_NUMS - 1) return;
  switch(mode){
      case LOW: mode = PIN_IRQ_MODE_LOW_LEVEL;
                break;
      case HIGH: mode = PIN_IRQ_MODE_HIGH_LEVEL;
                break;
      case CHANGE: mode = PIN_IRQ_MODE_RISING_FALLING;
                break;
      case FALLING: mode = PIN_IRQ_MODE_FALLING;
                break;
      case RISING: mode = PIN_IRQ_MODE_RISING;
                break;
  }
  interruptNum = g_ADigitalPinMap[interruptNum];
  //rt_pin_mode((rt_int32_t)interruptNum , PIN_MODE_INPUT_PULLUP);
  rt_pin_attach_irq((rt_int32_t)interruptNum, (rt_uint32_t)mode,hdr, args);
  rt_pin_irq_enable((rt_base_t)interruptNum, PIN_IRQ_ENABLE);
}

void attachInterrupt(uint32_t interruptNum, voidFuncPtr callback, uint32_t mode){
  attachInterrupt(interruptNum, attachInterrupt_ISR , mode, (void *)callback);
}

void detachInterrupt(uint32_t interruptNum) {
  if(interruptNum > DIGITAL_PIN_NUMS - 1) return;
  interruptNum = g_ADigitalPinMap[interruptNum];
  rt_pin_detach_irq((rt_int32_t)interruptNum);
}

/*
ISR(TWI_vect) {
  if(twiIntFunc)
    twiIntFunc();
}
*/

