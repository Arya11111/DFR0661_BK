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
//#include "HardwareSerial.h"
#define PWM_DEF_FREQ_HZ  1000//Hz
#define PWM_DEF_RESOLUTION  255
#define PWM_INVAILD_CHANNEL 255

#define ANALOGRERENCE_MODE_DEFAULT  ((ADC_CONFIG_MODE_4CLK_DELAY << 2) | ADC_CONFIG_MODE_CONTINUE)
uint8_t analog_reference = ANALOGRERENCE_MODE_DEFAULT;//DEFAULT;

typedef struct _pwmConfiguration{
  uint32_t channel;
  uint32_t period; //unit: ns 1ns~4.29s:1GHz~0.23Hz
  uint32_t pulse; //unit:ns(pulse <= period)
}sPWMConfiguration_t;

static sPWMConfiguration_t  pwmConfiguration[7]={
  {0, 500000, 500},
  {1, 500000, 500},
  {2, 500000, 500},
  {3, 500000, 500},
  {4, 500000, 500},
  {5, 500000, 500},
  {6, 500000, 500},
};

#ifdef __cplusplus
extern "C" {
#endif
#include "BkDriverPwm.h"

//Internal Reference is at 2.4
void analogReference(uint8_t mode)
{
    // can't actually set the register here because the default setting
    // will connect AVCC and the AREF pin, which would cause a short if
    // there's something connected to AREF.
    analog_reference = mode;
}

uint32_t analogRead( uint32_t ulPin)
{
  if(ulPin >= PINS_COUNT) return 0;
  if( g_APinDescription[ulPin].ulPinType == PIO_NOT_A_PIN ) return 0;
  if(!(g_APinDescription[ulPin].ulAttribute & PIN_ATTR_ANALOG)) return 0;
  if(g_APinDescription[ulPin].ulADCChannelNumber == NO_ADC_CHANNEL) return 0;
  UINT32 param, value = 0;
  //rt_kprintf("++hello\n");
  param = PWD_SARADC_CLK_BIT;
  sddev_control(ICU_DEV_NAME, CMD_CLK_PWR_UP, &param);
  switch(g_APinDescription[ulPin].ulADCChannelNumber){
      case ADC_CHANNEL1:
           param = GFUNC_MODE_ADC1;
           break;
      case ADC_CHANNEL2:
           param = GFUNC_MODE_ADC2;
           break;
      case ADC_CHANNEL3:
           param = GFUNC_MODE_ADC3;
           break;
      case ADC_CHANNEL4:
           param = GFUNC_MODE_ADC4;
           break;
      case ADC_CHANNEL5:
           param = GFUNC_MODE_ADC5;
           break;
      case ADC_CHANNEL6:
           param = GFUNC_MODE_ADC6;
           break;
      case ADC_CHANNEL7:
           param = GFUNC_MODE_ADC7;
           break;
      default:
           return 0;
  }
  //rt_kprintf("++hello\n");
  //adc gpio config
  sddev_control(GPIO_DEV_NAME, CMD_GPIO_ENABLE_SECOND, &param);
  //adc enable icu
  
  //clear fifo
  while((value & SARADC_ADC_FIFO_EMPTY) == 0) {
        REG_READ(SARADC_ADC_DATA);
        REG_READ(SARADC_ADC_DAT_AFTER_STA);
        value = REG_READ(SARADC_ADC_CONFIG);
  }
  //clear int
  value = REG_READ(SARADC_ADC_CONFIG);
  value |= SARADC_ADC_INT_CLR;
  REG_WRITE(SARADC_ADC_CONFIG, value);
  
  //config mode 连续采样
  value = (analog_reference & SARADC_ADC_MODE_MASK)<< SARADC_ADC_MODE_POSI; 
  value |= SARADC_ADC_CHNL_EN;
  value |= ((g_APinDescription[ulPin].ulADCChannelNumber & SARADC_ADC_CHNL_MASK) << SARADC_ADC_CHNL_POSI);
  value |= (((analog_reference >> 2)&0x01) << SARADC_ADC_DELAY_CLK_POSI);
  value |= SARADC_ADC_INT_CLR;
  //value |= SARADC_ADC_32M_MODE;
  value |= ((0x10 & SARADC_ADC_PRE_DIV_MASK) << SARADC_ADC_PRE_DIV_POSI);
  value |= ((0x20 & SARADC_ADC_SAMPLE_RATE_MASK) << SARADC_ADC_SAMPLE_RATE_POSI);
  value |= (( 0 & SARADC_ADC_FILTER_MASK)<< SARADC_ADC_FILTER_POSI);
  REG_WRITE(SARADC_ADC_CONFIG, value);
  value = SARADC_ADC_SAT_ENABLE | ((0x03 & SARADC_ADC_SAT_CTRL_MASK) << SARADC_ADC_SAT_CTRL_POSI);
  REG_WRITE(SARADC_ADC_SATURATION_CFG, value);
  //int enable
   //param = IRQ_SARADC_BIT;
   //sddev_control(ICU_DEV_NAME, CMD_ICU_INT_ENABLE, &param);
  
  uint16_t adcData[5];
  int g = 0;
  for(int i = 0; i < 5;){
      //rt_kprintf("++%d i=%d\n", g++, i);
      value = REG_READ(SARADC_ADC_CONFIG);
      //rt_kprintf("--0x%x %x\n", value,SARADC_ADC_FIFO_EMPTY);
      if((value & SARADC_ADC_FIFO_EMPTY) == 0){
          uint16_t dacVal;
          dacVal = REG_READ(SARADC_ADC_DAT_AFTER_STA)&0xFFFF;
          if(i < 5){
              adcData[i++] = dacVal;
              //rt_kprintf("%d----%d  %d\n", i, dacVal, adcData[i-1]);
              if(i == 5){
                  param = (IRQ_SARADC_BIT);
                  sddev_control(ICU_DEV_NAME, CMD_ICU_INT_DISABLE, &param);
                  value = REG_READ(SARADC_ADC_CONFIG);
                  value &= ~(SARADC_ADC_MODE_MASK << SARADC_ADC_MODE_POSI);
                  value &= ~(SARADC_ADC_CHNL_EN); 
                  value |= SARADC_ADC_INT_CLR;
                  REG_WRITE(SARADC_ADC_CONFIG, value);
                  param = PWD_SARADC_CLK_BIT;
                  sddev_control(ICU_DEV_NAME, CMD_CLK_PWR_DOWN, &param);
                  value = REG_READ(SARADC_ADC_CONFIG);
                  while((value & SARADC_ADC_FIFO_EMPTY) == 0) {
                      REG_READ(SARADC_ADC_DATA);
                      value = REG_READ(SARADC_ADC_CONFIG);
                  }
                  value = REG_READ(SARADC_ADC_CONFIG);
                  value |= SARADC_ADC_INT_CLR;
                  REG_WRITE(SARADC_ADC_CONFIG, value);
              }
          }
      }
  }
  uint16_t adcSum = adcData[0]+adcData[1]+adcData[2]+adcData[3]+adcData[4];
  //adcData[0] = (adcSum/5 + ((adcSum%5) ? 1 : 0));
  //float voltage=0.0;
  //int mv;
  //rt_kprintf("%d\n", adcData[0]);
  //voltage = (adcData[0]/4096.0)*2.4;
  //mv = voltage*1000;
  //Serial.print("voltage=");
  //Serial.print(voltage);
  //rt_kprintf("%d, %d, %dv  %d\n", adcSum, adcSum/5, mv, adcData[0]);
  return (adcSum/5 + ((adcSum%5) ? 1 : 0));
}

// Right now, PWM output only works on the pins with
// hardware support.  These are defined in the appropriate
// pins_*.c file.  For the rest of the pins, we default
// to digital output.
void analogWrite( uint32_t ulPin, uint32_t ulValue)
{
  if(ulPin >= PINS_COUNT) return;
  if( g_APinDescription[ulPin].ulPinType == PIO_NOT_A_PIN ) return;
  if(!(g_APinDescription[ulPin].ulAttribute & PIN_ATTR_ANALOG)) return;
  if(g_APinDescription[ulPin].ulPWMChannel == NOT_ON_PWM) return;
  UINT32 param, value = 0;
  pinMode(ulPin, OUTPUT);
  if(ulValue == 0){
      digitalWrite(ulPin, LOW);
  }else if(ulValue == 255){
      digitalWrite(ulPin, HIGH);
  }else{
      pwm_param_t pwmParam;
      
      //pwmConfiguration[g_APinDescription[ulPin].ulPWMChannel].period
      pwmParam.channel         = (uint8_t)g_APinDescription[ulPin].ulPWMChannel;
      pwmParam.cfg.bits.en     = PWM_INT_EN;
      pwmParam.cfg.bits.int_en = PWM_INT_DIS;
      pwmParam.cfg.bits.mode   = PMODE_PWM;
      pwmParam.cfg.bits.clk    = PWM_CLK_26M;
      pwmParam.p_Int_Handler   = 0;
      pwmParam.duty_cycle      = 260*ulValue/255;
      pwmParam.end_value       = 260;  // div

    sddev_control(PWM_DEV_NAME, CMD_PWM_INIT_PARAM, &pwmParam);
    bk_pwm_start((bk_pwm_t)g_APinDescription[ulPin].ulPWMChannel);
  }
  
}
//duty不变，改变频率，freq:1~  396Hz~13MHz
void toneFrequency(uint32_t ulPin, uint32_t freq){
  if(ulPin >= PINS_COUNT) return;
  if( g_APinDescription[ulPin].ulPinType == PIO_NOT_A_PIN ) return;
  if(!(g_APinDescription[ulPin].ulAttribute & PIN_ATTR_ANALOG)) return;
  if(g_APinDescription[ulPin].ulPWMChannel == NOT_ON_PWM) return;
  UINT32 div;
  pwm_param_t pwmParam;
  if(freq < 396){
      pwmParam.cfg.bits.clk  = PWM_CLK_32K;
      div = 32000/freq;
  }else{
      pwmParam.cfg.bits.clk = PWM_CLK_26M;
  }   div = 26000000/freq;
  if(div < 2 || div > 65535) return;

  //pwmConfiguration[g_APinDescription[ulPin].ulPWMChannel].period
  pwmParam.channel         = (uint8_t)g_APinDescription[ulPin].ulPWMChannel;
  pwmParam.cfg.bits.en     = PWM_INT_EN;
  pwmParam.cfg.bits.int_en = PWM_INT_DIS;
  pwmParam.cfg.bits.mode   = PMODE_PWM;

  pwmParam.cfg.bits.clk    = PWM_CLK_26M;
  pwmParam.p_Int_Handler   = 0;
  pwmParam.duty_cycle      = div/2;
  pwmParam.end_value       = div;  // div

  sddev_control(PWM_DEV_NAME, CMD_PWM_INIT_PARAM, &pwmParam);
  bk_pwm_start((bk_pwm_t)g_APinDescription[ulPin].ulPWMChannel);
}

#ifdef __cplusplus
}
#endif


