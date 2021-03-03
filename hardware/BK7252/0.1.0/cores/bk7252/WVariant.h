/*
  Copyright (c) 2015 Arduino LLC.  All right reserved.

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

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
#define DIGITAL_PIN_NUMS 31

typedef enum _EPioType
{
PIO_NOT_A_PIN = -1,
PIO_INPUT_OUTPUT = 0,
PIO_OUTPUT,
PIO_INPUT,
PIO_ANALOG,
PIO_SERCOM,
PIO_PWM,
}ePioType_t;

typedef enum _EAnalogChannel
{
  NO_ADC_CHANNEL = 0,
  ADC_CHANNEL1=1,
  ADC_CHANNEL2=2,
  ADC_CHANNEL3=3,
  ADC_CHANNEL4=4,
  ADC_CHANNEL5=5,
  ADC_CHANNEL6=6,
  ADC_CHANNEL7=7,
  ADC_CHANNEL_MAX=8
}eAnalogChannel_t;

typedef enum _EPWMChannel
{
  NOT_ON_PWM = -1,
  PWM0_CHANNEL=0,
  PWM1_CHANNEL=1,
  PWM2_CHANNEL=2,
  PWM3_CHANNEL=3,
  PWM4_CHANNEL=4,
  PWM5_CHANNEL=5,
  PWM_CHANNEL_MAX=6
}ePWMChannel_t;

typedef uint32_t pinAttribute_t;
#define PIN_ATTR_NONE          (0UL<<0)
#define PIN_ATTR_DIGITAL       (1UL<<1)
#define PIN_ATTR_ANALOG        (1UL<<2)
#define PIN_ATTR_PWM           (1UL<<3)
#define PIN_ATTR_EXTINT        (1UL<<4)

#define PIN_ATTR_TIMER         (1UL<<4)

typedef struct _PinDescription
{
  uint32_t ulPin;
  ePioType_t ulPinType; 
  pinAttribute_t ulAttribute;
  eAnalogChannel_t ulADCChannelNumber;
  ePWMChannel_t ulPWMChannel;
}sPinDescription_t;

extern const sPinDescription_t g_APinDescription[];

#ifdef __cplusplus
} // extern "C"
#endif
