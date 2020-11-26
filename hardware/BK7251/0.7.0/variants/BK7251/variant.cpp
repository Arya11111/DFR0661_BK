/*
  Copyright (c) 2014-2015 Arduino LLC.  All right reserved.
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

#include "variant.h"

const uint32_t g_ADigitalPinMap[] = {
  10,//D0/P10/UART RX
  11,//D1/P11/UART TX
  6, //D2/P6/13MHz PWM0
  7, //D3/P7/WiFi active outputor PWM1
  15,//D4/P15/SDIO_D3/SPI_CS/QSPI_CS(有问题)
  8, //D5/P8/Bluetooth activeinput/PWM2
  9, //D6/P9/Bluetooth priorityinput/PWM3
  24,//D7/P24/low power clock output/PWM4/QSPI_CLK
  2, //D8/P2/ADC5/I2S Clock
  26,//D9/P26/IrDA input/PWM5/QSPI_CS
  20,//D10/P20/JTAG TCK/QSPI IO3 Flash download port clock
  21,//D11/P21/JTAG TMS/QSPI IO2 Flash download chip enable signal at download mode
  22,//D12/P22/JTAG TDI/CLK_26M/QSPI IO1 FLASH download data input at download mode
  23,//D13/P23/JTAG TDO/ADC/QSPI IO0 FLASH download data output at download mode

  12,//D14/A0/P12/ADC6/UART1 CTS
  13,//D15/A1/P13/ADC7/UART1 RTS
  5, //D16/A2/P5/ADC2/I2S DOUT
  3, //D17/A3/P3/ADC4/I2S SYNC
  4, //D18/A4/P4/ADC1/I2S DIN

  0, //D19/P0/SCL 2/UART2 TX
  1, //D20/P1/SDA 2/UART2 RX

  14,//D21/P14/SPI SCK/SDIO_CLK/QSPI_CLK
  16,//D22/P16/SPI MOSI/SDIO_CMD/QSPI_IO0
  17,//D23/P17/SPI MISO/SDIO_D0/QSPI_IO1

  25,//D24/P25/USBDP/TX_EN
  28,//D25/P28/USBDN/RX_EN
  
  18,//D26/P18/SD_D2/QSPI_IO2/SDIO D2/QSPI IO2
  19,//D27/P19/SDIO_D1/QSPI_IO3
  
  34,//D28/P34/SD_CLK/DVP D2
  35,//D29/P35/SD_CMD/DVP D3
  36,//D30/P36/SD_D0/DVP D4
};
