/*
 * SPI Master library for nRF5x.
 * Copyright (c) 2015 Arduino LLC
 * Copyright (c) 2016 Sandeep Mistry All right reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "SPI.h"
#include <Arduino.h>
#include <wiring_private.h>
#include <assert.h>

#define SPI_IMODE_NONE   0
#define SPI_IMODE_EXTINT 1
#define SPI_IMODE_GLOBAL 2


const SPISettings DEFAULT_SPI_SETTINGS = SPISettings();


SPIClass::SPIClass()
  :_settings(SPISettings(10000000, MSBFIRST, SPI_MODE0)),_initialized(false){}

void SPIClass::begin(bool flag)
{
  init();
}

void SPIClass::init()
{
  UINT32 param;
  OSStatus result = 0;
  if(_initialized){
      spiMasterDeinit();
      _initialized = false;
  }
  _initialized = true;
  param = 0;
  spi_ctrl(CMD_SPI_SET_BITWIDTH, (void *)&param);
  config(_settings);
  param = 1;
  spi_ctrl(CMD_SPI_SET_MSTEN, (void *)&param);
  param = 3;
  spi_ctrl(CMD_SPI_SET_NSSID, (void *)&param);
  param = 1;
  initMasterEnable(true);
  //spi_ctrl(CMD_SPI_INIT_MSTEN, (void *)&param);
  spi_ctrl(CMD_SPI_RXOVR_EN, (void *)&param);//接收完成
  
  param = 1;
  spi_ctrl(CMD_SPI_UNIT_ENABLE, (void *)&param);
  return;
}

void SPIClass::config(SPISettings settings)
{
  setClock(_settings.clockFreq);
  setDataMode(_settings.dataMode);
}

void SPIClass::end()
{
  if(_initialized){
      spiMasterDeinit();
      _initialized = false;
  }
}

void SPIClass::beginTransaction(SPISettings settings)
{
  _settings = settings;
  setClock(_settings.clockFreq);
  setDataMode(_settings.dataMode);
}

void SPIClass::endTransaction(void)
{
}

void SPIClass::setBitOrder(uint8_t bitOrder){
  _settings.bitOrder = bitOrder;
}

void SPIClass::setDataMode(uint8_t mode)
{
  UINT32 value;
  if(!_initialized) return;

  value = REG_READ(SPI_CTRL);
  if(mode & BK_SPI_CPOL){
      value |= CKPOL;
  }else{
       value &= ~CKPOL;
  }
  REG_WRITE(SPI_CTRL, value);
  value = REG_READ(SPI_CTRL);
  if (mode & BK_SPI_CPHA){
      value |= CKPHA;
  }else{
      value &= ~CKPHA;
  }
  REG_WRITE(SPI_CTRL, value);
}

void SPIClass::setFrequency(uint32_t freq){
  _settings.clockFreq = freq;
  setClock(freq);
}

/*50KHz~4MHz*/
void SPIClass::setClockDivider(uint8_t div)
{
  if(!_initialized) return;
  uint32_t param = 0;
  if(div < 2){
      div = 2;
  }else if(div > 255){
      div = 255;
  }
  param = PCLK_POSI_SPI;
  sddev_control(ICU_DEV_NAME, CMD_CONF_PCLK_26M, &param);
  
  param = REG_READ(SPI_CTRL);
  param &= ~(SPI_CKR_MASK << SPI_CKR_POSI);
  param |= (div << SPI_CKR_POSI);
  REG_WRITE(SPI_CTRL, param);
}

byte SPIClass::transfer(uint8_t data)
{
  
  byte temp;
  //rt_kprintf("1=0x%x, ", data);
    //REG_WRITE(SPI_DAT, data);
    //rt_kprintf("2=0x%x,", data);
  while(spi_write_txfifo(data) == 0);
  while(!spi_read_rxfifo((uint8_t *)&temp));
  //rt_kprintf("3=0x%x\n", temp);
  return temp;
  // byte temp;
  // sendAndRecvBuffer(&data, 1, &temp, 1);
  // return temp;
}

uint16_t SPIClass::transfer16(uint16_t data) {
  union { uint16_t val; struct { uint8_t lsb; uint8_t msb; }; } t;
  t.val = data;
  if(_settings.bitOrder == MSBFIRST){
     t.msb = transfer(t.msb);
     t.lsb = transfer(t.lsb);
     
  }else{
      t.lsb = transfer(t.lsb);
      t.msb = transfer(t.msb);
  }
  return t.val;
}
void SPIClass::transfer(void *buf, size_t count){
  uint8_t *buffer = (uint8_t *)buf;
  for (size_t i=0; i<count; i++) {
    *buffer = transfer(*buffer);
    buffer++;
  }
}

void SPIClass::setClock(uint32_t maxHz){
  if(!_initialized) return;
  uint32_t param = 0;
  uint32_t _sourceClock, _actSpiClock;
  uint8_t _div;
  if(maxHz > 4333000){
      if(maxHz > 30000000) _actSpiClock = 30000000;
      else _actSpiClock = maxHz;
      _sourceClock = SPI_PERI_CLK_DCO;
      param = PCLK_POSI_SPI;
      sddev_control(ICU_DEV_NAME, CMD_CONF_PCLK_DCO, &param);
  }else{
      _actSpiClock = maxHz;
      _sourceClock = SPI_PERI_CLK_26M;
      param = PCLK_POSI_SPI;
      sddev_control(ICU_DEV_NAME, CMD_CONF_PCLK_26M, &param);
  }
  _div = ((_sourceClock>>1)/_actSpiClock) - 1;
  if(_div < 2){
      _div = 2;
  }else if(_div > 255){
      _div = 255;
  }
  param = REG_READ(SPI_CTRL);
  param &= ~(SPI_CKR_MASK << SPI_CKR_POSI);
  param |= (_div << SPI_CKR_POSI);
  REG_WRITE(SPI_CTRL, param);
  //rt_kprintf("div=%d, spiclk=%d, source=%d, tarGetFreq=%d, actFreq=%d\n", _div, _actSpiClock, _sourceClock, maxHz, ((_sourceClock>>1)/(_div+1)));
}


void SPIClass::initMasterEnable(bool val){
  UINT32 value = 0;

  value = REG_READ(SPI_CTRL);
  value &= ~((0x03UL << TXINT_MODE_POSI) | (0x03UL << RXINT_MODE_POSI));
  value |= RXOVR_EN| TXOVR_EN/*| RXINT_EN | TXINT_EN*/| (0x3UL << RXINT_MODE_POSI) | (0x3UL << TXINT_MODE_POSI);   // 12byte left

  REG_WRITE(SPI_CTRL, value);
  if(val)
  {
      spi_slave_set_cs_finish_interrupt(false);
  }
  else
  {
      spi_slave_set_cs_finish_interrupt(true);
  }
  
  spi_icu_configuration(true);
  spi_gpio_configuration();
}

void SPIClass::spiMasterDeinit(){
  UINT32 status, slv_status;

  spi_icu_configuration(false);
  
  REG_WRITE(SPI_CTRL, 0);

  status = REG_READ(SPI_STAT);
  slv_status = REG_READ(SPI_SLAVE_CTRL);

  REG_WRITE(SPI_STAT, status);
  REG_WRITE(SPI_SLAVE_CTRL, slv_status);
}

void SPIClass::spi_slave_set_cs_finish_interrupt(bool enable){
  UINT32 value;

  value = REG_READ(SPI_SLAVE_CTRL);
  if(enable)
  {
      value |= SPI_S_CS_UP_INT_EN;
  }else{
      value &= ~(SPI_S_CS_UP_INT_EN);
  }
  // don't clean cs finish status
  value &= ~(SPI_S_CS_UP_INT_STATUS);
  REG_WRITE(SPI_SLAVE_CTRL, value);
}

void SPIClass::spi_icu_configuration(bool enable){
  UINT32 param;

  if(enable) 
  {
      param = PWD_SPI_CLK_BIT;
      sddev_control(ICU_DEV_NAME, CMD_CLK_PWR_UP, &param);

      param = (IRQ_SPI_BIT);
      sddev_control(ICU_DEV_NAME, CMD_ICU_INT_ENABLE, &param);
  }else{
      param = (IRQ_SPI_BIT);
      sddev_control(ICU_DEV_NAME, CMD_ICU_INT_DISABLE, &param);
      
      param = PWD_SPI_CLK_BIT;
      sddev_control(ICU_DEV_NAME, CMD_CLK_PWR_DOWN, &param);
  }
}
void SPIClass::spi_gpio_configuration()
{
    uint32_t val;
    
#if (USE_SPI_GPIO_NUM == USE_SPI_GPIO_14_17)
    val = GFUNC_MODE_SPI;
#elif (USE_SPI_GPIO_NUM == USE_SPI_GPIO_30_33)
    val = GFUNC_MODE_SPI1;
#else
    #error "USE_SPI_GPIO_NUM must set to gpio14-17 or gpio30-33"
#endif

    sddev_control(GPIO_DEV_NAME, CMD_GPIO_ENABLE_SECOND, &val);
}

SPIClass SPI;

