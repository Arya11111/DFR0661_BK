/*
  Copyright (c) 2014-2015 Arduino LLC.  All right reserved.
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

// API compatibility
//#include "variant.h"

//#ifndef __PIN_ARDUINO_H
//#define __PIN_ARDUINO_H
//6位ADC 最大值0x3F

//#define A0 4 //ADC1
//#define A1 5 //ADC2
//#define A2 23 //ADC3
//#define A3 2//ADC4
//#define A4 3 //ADC5
//#define A5 12 //ADC6
//#define A6 13//ADC7
static const uint8_t A0 = 14;//
static const uint8_t A1 = 15;//ADC7  GPIO13 * 
static const uint8_t A2 = 16;//ADC2  GPIO5 *
static const uint8_t A3 = 17;//ADC4  GPIO3 
static const uint8_t A4 = 18;//ADC1  GPIO4 *
static const uint8_t A5 = 13;//ADC3  GPIO23 
static const uint8_t A6 = 8; //ADC5  GPIO2 *


//ADC内部基准电压 2.4V
static const uint8_t analog_pin_to_channel_PGM[] = {
  4,//A2,
  5,//A3,
  23,//A6,
  2,//A0,
  3,//A1,
  12,//A4,
  13,//A5
};


/*****************************************
pwm
p06----PWM0----GPIO6 *
p07----PWM1----GPIO7 *
p08----PWM2----GPIO8(NO PIN) *
p09----PWM3----GPIO9(NO PIN) *
p24----PWM4----GPIO24 *
p26----PWM5----GPIO26 *
period 2000ns~(65535/26)x1000=2520 000ns 
500,000Hz//500KHz
396Hz

*****************************************/
static const uint8_t pwmChannelMapPin[]={
  6,7,8,9,24,26
};
//#endif
