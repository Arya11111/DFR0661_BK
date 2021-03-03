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

#ifndef _SPI_H_INCLUDED
#define _SPI_H_INCLUDED

#include <Arduino.h>

// SPI_HAS_TRANSACTION means SPI has
//   - beginTransaction()
//   - endTransaction()
//   - usingInterrupt()
//   - SPISetting(clock, bitOrder, dataMode)
#define SPI_HAS_TRANSACTION 1


/*
SPI四种模式，分别对应时钟相位(CPHA)，极性(CPOL)
时钟极性:空闲时，SCLK的电平(1,空闲时高电平;0，空闲时低电平)
时钟相位:即SPI在SCLK的第几个边沿采样(0,第一个边沿，1,第二个边沿)

*/
/*#define SPI_MODE0 0x02
#define SPI_MODE1 0x00
#define SPI_MODE2 0x03
#define SPI_MODE3 0x01
*/
#define SPI_MODE0 0x00
#define SPI_MODE1 0x01
#define SPI_MODE2 0x02
#define SPI_MODE3 0x03

extern "C" {
#include "include.h"
#include "arm_arch.h"
#include "typedef.h"
#include "arm_arch.h"
#include "icu_pub.h"

#include "spi1.h"
#include "spi_pub.h"

#include "sys_ctrl_pub.h"

#include "drv_model_pub.h"
#include "mem_pub.h"
#include "sys_config.h"
#include "error.h"
#include "bk_rtos_pub.h"
}
#define SPI_BUFFER_SIZE 1025
#define SPI_PERI_CLK_26M        (26 * 1000 * 1000)
#define SPI_PERI_CLK_DCO        (180 * 1000 * 1000)

class SPISettings {
  public:
  SPISettings(uint32_t clock, uint8_t bitOrder, uint8_t dataMode) {
    if (__builtin_constant_p(clock)) {
      init_AlwaysInline(clock, bitOrder, dataMode);
    } else {
      init_MightInline(clock, bitOrder, dataMode);
    }
  }

  // Default speed set to 4MHz, SPI mode set to MODE 0 and Bit order set to MSB first.
  SPISettings() { init_AlwaysInline(10000000, MSBFIRST, SPI_MODE0); }

  private:
  void init_MightInline(uint32_t clock, uint8_t bitOrder, uint8_t dataMode) {
    init_AlwaysInline(clock, bitOrder, dataMode);
  }

  void init_AlwaysInline(uint32_t clock, uint8_t bitOrder, uint8_t dataMode) __attribute__((__always_inline__)) {
    this->clockFreq = clock;
    this->bitOrder = bitOrder;
    this->dataMode = dataMode;
  }

  uint32_t clockFreq;
  uint8_t dataMode;
  uint8_t bitOrder;
  uint8_t dataWidth;

  friend class SPIClass;
};

class SPIClass {
public:
  SPIClass();
  void begin(bool flag = false);
  byte transfer(uint8_t data);
  uint16_t transfer16(uint16_t data);
  void transfer(void *buf, size_t count);

  // Transaction Functions
  void beginTransaction(SPISettings settings);
  void endTransaction(void);

  void end();
  void setBitOrder(uint8_t bitOrder);
  void setDataMode(uint8_t uc_mode);
  void setClockDivider(uint8_t uc_div);
  void setFrequency(uint32_t freq);
  
protected:
  void setClock(uint32_t maxHz);
  
  
private:
  void init();
  void config(SPISettings settings);
  void initMasterEnable(bool val);
  void spiMasterDeinit();
  void spi_slave_set_cs_finish_interrupt(bool enable);
  void spi_icu_configuration(bool enable);
  void spi_gpio_configuration();
  SPISettings _settings;
  bool _initialized;
/*
  struct rt_spi_configuration _cfg;
  struct rt_spi_device * _spiDev;
  uint8_t _dataWidth;
*/
  
};

extern SPIClass SPI;

#define SPI_CLOCK_DIV2   2
#define SPI_CLOCK_DIV3   3
#define SPI_CLOCK_DIV4   4
#define SPI_CLOCK_DIV5   5
#define SPI_CLOCK_DIV6   6
#define SPI_CLOCK_DIV7   7
#define SPI_CLOCK_DIV8   8
#define SPI_CLOCK_DIV9   9
#define SPI_CLOCK_DIV10  10

#endif
