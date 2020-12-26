/*
  wiring_analog.c - analog input and output
  Part of Arduino - http://www.arduino.cc/

  Copyright (c) 2005-2006 David A. Mellis

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

  Modified 28 September 2010 by Mark Sproul
*/

//#include "wiring_private.h"


#include "Arduino.h"
#define PWM_DEF_FREQ_HZ  1000//Hz
#define PWM_DEF_RESOLUTION  255
#define PWM_INVAILD_CHANNEL 255

uint8_t analog_reference = 0;//DEFAULT;


#ifdef __cplusplus
extern "C" {
#endif
#include "BkDriverPwm.h"


struct analogPWMArgs PWMArgs[sizeof(pwmChannelMapPin)] = {0};
static ADC_OBJ _adcAnalogList[7];
void PWMInit(){
  for(uint8_t i = 0; i < sizeof(pwmChannelMapPin); i++){
      PWMArgs[i].channel = i;
      PWMArgs[i].frequency = PWM_DEF_FREQ_HZ;//Hz 50000;//2000ns/2ms
      PWMArgs[i].resolution = PWM_DEF_RESOLUTION;
  }
}
uint8_t getPinPWMChannel(uint8_t pin){
  uint8_t index = PWM_INVAILD_CHANNEL;
  uint32_t period, pulse;
  for(uint8_t i = 0; i < sizeof(pwmChannelMapPin); i++){
      if(pwmChannelMapPin[i] == pin){
          index = i;
          break;
      }
  }
  return index;
}

void analogReference(uint8_t mode)
{
    // can't actually set the register here because the default setting
    // will connect AVCC and the AREF pin, which would cause a short if
    // there's something connected to AREF.
    analog_reference = mode;
}

uint32_t analogRead( uint32_t ulPin)
{
   uint32_t temp = 0;
   //rt_kprintf("123456\r\n");
   //saradc_work_create(20);
   uint8_t index = 0;
   if(ulPin > (DIGITAL_PIN_NUMS - 1)) return;
   ulPin = g_ADigitalPinMap[ulPin];
   for(uint8_t i = 0; i < sizeof(analog_pin_to_channel_PGM); i++){
       if(analog_pin_to_channel_PGM[i] == ulPin){
           index = i+1;
           break;
       }
   }
   if(index == 0) return;
   adc_obj_init(&_adcAnalogList[index-1], RT_NULL, index, &_adcAnalogList[index-1]);
   adc_obj_start(&_adcAnalogList[index-1]);
   for(int i = 0; i < 10; i++){
       tadc_obj_handler(&_adcAnalogList[index-1]);
   }
   temp = *((int *)(_adcAnalogList[index-1].user_data));
   adc_obj_stop(&_adcAnalogList[index-1]);
   return temp;
}

// Right now, PWM output only works on the pins with
// hardware support.  These are defined in the appropriate
// pins_*.c file.  For the rest of the pins, we default
// to digital output.
void analogWrite( uint32_t ulPin, uint32_t ulValue )
{
  pinMode(ulPin, OUTPUT);
  
  uint8_t index;
  uint32_t period, pulse;
  if(ulPin > (DIGITAL_PIN_NUMS - 1)) return;
  ulPin = g_ADigitalPinMap[ulPin];
  if((index = getPinPWMChannel(ulPin)) == PWM_INVAILD_CHANNEL) return;
  if(PWMArgs[index].frequency == 0 || PWMArgs[index].resolution == 0){
      PWMInit();
  }
  if(ulValue > PWMArgs[index].resolution) return;
  period = (1000000/PWMArgs[index].frequency)*26;
  pulse = (ulValue*period)/PWMArgs[index].resolution;
  bk_pwm_initialize((bk_pwm_t)index, period, (float)pulse);
  bk_pwm_start((bk_pwm_t)index);
}

void analogWriteResolution(uint8_t pin,uint32_t res){
  uint8_t index;
  if(pin > (DIGITAL_PIN_NUMS - 1)) return;
  pin = (uint8_t)g_ADigitalPinMap[pin];
  if((index = getPinPWMChannel(pin)) == PWM_INVAILD_CHANNEL) return;
  if(PWMArgs[index].frequency == 0){
      PWMArgs[index].frequency = PWM_DEF_FREQ_HZ;
  }
  PWMArgs[index].channel = index;
  PWMArgs[index].resolution = res;
}

void setPWMFrequency(uint8_t pin, uint32_t freq_Hz){
  uint8_t index;
  if(pin > (DIGITAL_PIN_NUMS - 1)) return;
  pin = (uint8_t)g_ADigitalPinMap[pin];
  if((index = getPinPWMChannel(pin)) == PWM_INVAILD_CHANNEL) return;
  if(PWMArgs[index].resolution == 0){
      PWMArgs[index].resolution = PWM_DEF_RESOLUTION;
  }
  PWMArgs[index].channel = index;
  PWMArgs[index].frequency = freq_Hz;
}

void setPWMArguments(uint8_t pin, uint32_t freq_Hz, uint32_t res){
  uint8_t index;
  if(pin > (DIGITAL_PIN_NUMS - 1)) return;
  pin = (uint8_t)g_ADigitalPinMap[pin];
  if((index = getPinPWMChannel(pin)) == PWM_INVAILD_CHANNEL) return;
  PWMArgs[index].channel = index;
  PWMArgs[index].frequency = freq_Hz;
  PWMArgs[index].resolution = res;
}

#ifdef __cplusplus
}
#endif


