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

void SPIClass::begin(bool flag)
{
  //if(flag) _cfg.mode = 1 << 3;
  //else _cfg.mode = 0 << 3;
  
  init();
  config(DEFAULT_SPI_SETTINGS);
}

void SPIClass::init()
{
  if (initialized)
    return;
  interruptMode = SPI_IMODE_NONE;
  interruptSave = 0;
  interruptMask = 0;
  _spiDev = (struct rt_spi_device *)rt_device_find("gspi");
  if(_spiDev == NULL){
	  //rt_hw_spi_device_init();
      rt_kprintf("spi dev not found failed!\r\n");
  }else{
      rt_kprintf("spi dev sucess!\r\n");
  }
  initialized = true;
}

void SPIClass::config(SPISettings settings)
{
  _cfg.mode = (settings.bitOrder << 2)|(settings.dataMode & 0x03);
  _cfg.data_width = _dataWidth;
  _cfg.max_hz = settings.clockFreq;
  rt_spi_configure(_spiDev, &_cfg);
}

void SPIClass::end()
{
  _spiDev = NULL;
  initialized = false;
}

void SPIClass::beginTransaction(SPISettings settings)
{
  config(settings);
}

void SPIClass::endTransaction(void)
{
}

void SPIClass::setBitOrder(uint8_t bitOrder){
  _cfg.mode &= (~(1<<2));//将第2位清0
  _cfg.mode |= (bitOrder << 2);
  rt_spi_configure(_spiDev, &_cfg);
}

void SPIClass::setBitWidth(uint8_t width)
{
  _dataWidth = width;
  _cfg.data_width = width;
  rt_spi_configure(_spiDev, &_cfg);
}

void SPIClass::setDataMode(uint8_t mode)
{
  _cfg.mode &= 0xFC;
  _cfg.mode |= (mode & 0x03);
  rt_spi_configure(_spiDev, &_cfg);
}

void SPIClass::setClockDivider(uint8_t div)
{
  uint32_t freq = 26000000;
  if(div < 5) freq = 180000000;
  _cfg.max_hz = (freq >> 1)/(div + 1);
  rt_spi_configure(_spiDev, &_cfg);
}

byte SPIClass::transfer(uint8_t data)
{
  struct spi_message msg;
  byte temp;
  rt_spi_transfer(_spiDev, &data, &temp, 1);
  //rt_kprintf("temp = 0x%2x \r\n", temp);
  return temp;
}

uint16_t SPIClass::transfer16(uint16_t data) {
  uint16_t temp = 0;
  uint8_t in[2] = {data&0xFF, data >> 8};
  uint8_t out[2] = {0};
  rt_spi_transfer(_spiDev, &in, &out, 2);
  temp = out[1] << 8 | out[0];
  return temp;
}

SPIClass SPI;

