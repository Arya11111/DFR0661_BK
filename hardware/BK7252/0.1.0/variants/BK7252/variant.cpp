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

/*
 *
 * + Pin number +  ZERO Board pin  |  PIN   | Label/Name      | Comments (* is for default peripheral in use)
 * +------------+------------------+--------+-----------------+------------------------------
 * |            | Digital Low      |        |                 | 
 * +------------+------------------+--------+-----------------+------------------------------
 * | 0          | RXD              |  P10   |                 | EIC/UART1	[11] ADC/AIN[19] PTC/X[3] *SERCOM0/PAD[3] SERCOM2/PAD[3] TCC1/WO[1] TCC0/WO[3]
 * | 1          | TXD              |  P11   |                 | EIC/EXTINT[10] ADC/AIN[18] PTC/X[2] *SERCOM0/PAD[2] TCC1/WO[0] TCC0/WO[2]
 * | 2          | ~2               |  P20   |                 | EIC/NMI ADC/AIN[16] PTC/X[0] SERCOM0/PAD[0] SERCOM2/PAD[0] *TCC0/WO[0] TCC1/WO[2]
 * | 3          | ~3               |  P21   |                 | EIC/EXTINT[9] ADC/AIN[17] PTC/X[1] SERCOM0/PAD[1] SERCOM2/PAD[1] *TCC0/WO[1] TCC1/WO[3]
 * | 4          |                  |        |                 | NC
 * | 5          | ~5               |  P22   |                 | EIC/EXTINT[15] SERCOM2/PAD[3] SERCOM4/PAD[3] TC3/WO[1] *TCC0/WO[5]
 * | 6          | ~6               |  P20   |                 | EIC/EXTINT[4] PTC/X[8] SERCOM5/PAD[2] SERCOM3/PAD[2] TC7/WO[0] *TCC0/WO[6]
 * | 7          | ~7               |  P21   |                 | EIC/EXTINT[5] PTC/X[9] SERCOM5/PAD[3] SERCOM3/PAD[3] TC7/WO[1] *TCC0/WO[7]
 * +------------+------------------+--------+-----------------+------------------------------
 * |            | Digital High     |        |                 |
 * +------------+------------------+--------+-----------------+------------------------------
 * | 8          | ~8               |  PA06  |                 | EIC/EXTINT[6] PTC/Y[4] ADC/AIN[6] AC/AIN[2] SERCOM0/PAD[2] *TCC1/WO[0]
 * | 9          | ~9               |  PA07  |                 | EIC/EXTINT[7] PTC/Y[5] DC/AIN[7] AC/AIN[3] SERCOM0/PAD[3] *TCC1/WO[1]
 * | 10         | ~10              |  PA18  |                 | EIC/EXTINT[2] PTC/X[6] SERCOM1/PAD[2] SERCOM3/PAD[2] *TC3/WO[0] TCC0/WO[2]
 * | 11         | ~11              |  PA16  |                 | EIC/EXTINT[0] PTC/X[4] SERCOM1/PAD[0] SERCOM3/PAD[0] *TCC2/WO[0] TCC0/WO[6]
 * | 12         | ~12              |  PA19  |                 |
 * | 15         | AREF             |  PA03  |                 | *DAC/VREFP PTC/Y[1]
 * | 16         | SDA              |  PA22  |                 | EIC/EXTINT[6] PTC/X[10] *SERCOM3/PAD[0] SERCOM5/PAD[0] TC4/WO[0] TCC0/WO[4]
 * | 17         | SCL              |  PA23  |                 | EIC/EXTINT[7] PTC/X[11] *SERCOM3/PAD[1] SERCOM5/PAD[1] TC4/WO[1] TCC0/WO[5]
 * +------------+------------------+--------+-----------------+------------------------------
 * |            |SPI (Legacy ICSP) |        |                 |
 * +------------+------------------+--------+-----------------+------------------------------
 * | 18         | 1                |  PA12  | MISO            | EIC/EXTINT[12] SERCOM2/PAD[0] *SERCOM4/PAD[0] TCC2/WO[0] TCC0/WO[6]
 * | 19         | 2                |        | 5V0             |
 * | 20         | 3                |  PB11  | SCK             | EIC/EXTINT[11]                *SERCOM4/PAD[3] TC5/WO[1] TCC0/WO[5]
 * | 21         | 4                |  PB10  | MOSI            | EIC/EXTINT[10]                *SERCOM4/PAD[2] TC5/WO[0] TCC0/WO[4]
 * | 22         | 5                |        | RESET           |
 * | 23         | 6                |        | GND             |
 * +------------+------------------+--------+-----------------+------------------------------
 * |            | Analog Connector |        |                 |
 * +------------+------------------+--------+-----------------+------------------------------
 * | 24         | A0               |  PA02  |                 | EIC/EXTINT[2] *ADC/AIN[0] PTC/Y[0] DAC/VOUT
 * | 25         | A1               |  PB08  |                 | EIC/EXTINT[8] *ADC/AIN[2] PTC/Y[14] SERCOM4/PAD[0] TC4/WO[0]
 * | 26         | A2               |  PB09  |                 | EIC/EXTINT[9] *ADC/AIN[3] PTC/Y[15] SERCOM4/PAD[1] TC4/WO[1]
 * | 27         | A3               |  PA04  |                 | EIC/EXTINT[4] *ADC/AIN[4] AC/AIN[0] PTC/Y[2] SERCOM0/PAD[0] TCC0/WO[0]
 * | 28         | A4               |  PA05  |                 | EIC/EXTINT[5] *ADC/AIN[5] AC/AIN[1] PTC/Y[5] SERCOM0/PAD[1] TCC0/WO[1]
 * | 29         | A5               |  PB02  |                 | EIC/EXTINT[2] *ADC/AIN[10] PTC/Y[8] SERCOM5/PAD[0] TC6/WO[0]
 * +------------+------------------+--------+-----------------+------------------------------
 * |            | LEDs             |        |                 |
 * +------------+------------------+--------+-----------------+------------------------------
 * | 30         |                  |  PB03  | RX              |
 * | 31         |                  |  PA27  | TX              |
 * +------------+------------------+--------+-----------------+------------------------------
 * |            | USB              |        |                 |
 * +------------+------------------+--------+-----------------+------------------------------
 * | 32         |                  |  PA28  | USB HOST ENABLE |
 * | 33         |                  |  PA24  | USB_NEGATIVE    | USB/DM
 * | 34         |                  |  PA25  | USB_POSITIVE    | USB/DP
 * +------------+------------------+--------+-----------------+------------------------------
 * |            | EDBG             |        |                 |
 * +------------+------------------+--------+-----------------+------------------------------
 * | 35         |                  |  PB22  | EDBG_UART TX    | SERCOM5/PAD[2]
 * | 36         |                  |  PB23  | EDBG_UART RX    | SERCOM5/PAD[3]
 * +------------+------------------+--------+-----------------+------------------------------
 * | 37         |                  |  PA22  | EDBG_SDA        | SERCOM3/PAD[0]
 * | 38         |                  |  PA23  | EDBG_SCL        | SERCOM3/PAD[1]
 * +------------+------------------+--------+-----------------+------------------------------
 * | 39         |                  |  PA19  | EDBG_MISO       | SERCOM1/PAD[3]
 * | 40         |                  |  PA16  | EDBG_MOSI       | SERCOM1/PAD[0]
 * | 41         |                  |  PA18  | EDBG_SS         | SERCOM1/PAD[2]
 * | 42         |                  |  PA17  | EDBG_SCK        | SERCOM1/PAD[1]
 * +------------+------------------+--------+-----------------+------------------------------
 * | 43         |                  |  PA13  | EDBG_GPIO0      | EIC/EXTINT[13] *TCC2/WO[1] TCC0/WO[7]
 * | 44         |                  |  PA21  | EDBG_GPIO1      | Pin 7
 * | 45         |                  |  PA06  | EDBG_GPIO2      | Pin 8
 * | 46         |                  |  PA07  | EDBG_GPIO3      | Pin 9
 * +------------+------------------+--------+-----------------+------------------------------
 * |            |32.768KHz Crystal |        |                 |
 * +------------+------------------+--------+-----------------+------------------------------
 * |            |                  |  PA00  | XIN32           | EXTINT[0] SERCOM1/PAD[0] TCC2/WO[0]
 * |            |                  |  PA01  | XOUT32          | EXTINT[1] SERCOM1/PAD[1] TCC2/WO[1]
 * +------------+------------------+--------+-----------------+------------------------------
 */

#include "variant.h"

/* Types used for the table below */
//typedef struct _PinDescription
//{
//  uint32_t        ulPin ;
//  EPioType        ulPinType ;//默认功能
//  uint32_t        ulPinAttribute ;
//  EAnalogChannel  ulADCChannelNumber ; /* ADC Channel number in the SAM device */
//  EPWMChannel     ulPWMChannel ;
//  ETCChannel      ulTCChannel ;
//  EExt_Interrupts ulExtInt ;
//} PinDescription ;
//
//const PinDescription g_APinDescription[]={
//  // 0/1 - UART(Serial1)
//  {10, PIO_UART, (PIN_ATTR_DIGITAL|PIN_ATTR_EXTINT), No_ADC_Channel, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_11},//0
//};

const sPinDescription_t g_APinDescription[]=
{
  //0/1-UART (Serial1)
  {10, PIO_INPUT_OUTPUT, PIN_ATTR_NONE, NO_ADC_CHANNEL, NOT_ON_PWM},//rx //D0/P10/UART RX
  {11, PIO_INPUT_OUTPUT, PIN_ATTR_NONE, NO_ADC_CHANNEL, NOT_ON_PWM},//tx //D1/P11/UART TX

  //2..7
  {20, PIO_INPUT_OUTPUT, PIN_ATTR_DIGITAL, NO_ADC_CHANNEL, NOT_ON_PWM},//D2//P20/JTAG TCK/QSPI IO3 Flash download port clock
  {21, PIO_INPUT_OUTPUT, PIN_ATTR_DIGITAL, NO_ADC_CHANNEL, NOT_ON_PWM},
  {5,  PIO_INPUT_OUTPUT, PIN_ATTR_DIGITAL, ADC_CHANNEL2, NOT_ON_PWM},
  {22, PIO_INPUT_OUTPUT, PIN_ATTR_DIGITAL, NO_ADC_CHANNEL, NOT_ON_PWM},
  {23, PIO_INPUT_OUTPUT, PIN_ATTR_DIGITAL, ADC_CHANNEL3, NOT_ON_PWM},
  {6,  PIO_INPUT_OUTPUT, (PIN_ATTR_DIGITAL|PIO_PWM), NO_ADC_CHANNEL, PWM0_CHANNEL},
  
  //8
  {0,  PIO_NOT_A_PIN, PIN_ATTR_NONE, NO_ADC_CHANNEL, NOT_ON_PWM},
  //9..13
  {7,  PIO_INPUT_OUTPUT, (PIN_ATTR_DIGITAL|PIO_PWM), NO_ADC_CHANNEL, PWM1_CHANNEL},
  {36, PIO_INPUT_OUTPUT, PIN_ATTR_DIGITAL, NO_ADC_CHANNEL, NOT_ON_PWM},
  {37, PIO_INPUT_OUTPUT, PIN_ATTR_DIGITAL, NO_ADC_CHANNEL, NOT_ON_PWM},
  {24, PIO_INPUT_OUTPUT, (PIN_ATTR_DIGITAL|PIO_PWM), NO_ADC_CHANNEL, PWM4_CHANNEL},
  {26, PIO_INPUT_OUTPUT, (PIN_ATTR_DIGITAL|PIO_PWM), NO_ADC_CHANNEL, PWM5_CHANNEL},
  
  //14..18 A0..A4
  {12, PIO_ANALOG, (PIN_ATTR_DIGITAL|PIN_ATTR_ANALOG), ADC_CHANNEL6, NOT_ON_PWM},
  {13, PIO_ANALOG, (PIN_ATTR_DIGITAL|PIN_ATTR_ANALOG), ADC_CHANNEL7, NOT_ON_PWM},
  {2,  PIO_ANALOG, (PIN_ATTR_DIGITAL|PIN_ATTR_ANALOG), ADC_CHANNEL2, NOT_ON_PWM},
  {3,  PIO_ANALOG, (PIN_ATTR_DIGITAL|PIN_ATTR_ANALOG), ADC_CHANNEL4, NOT_ON_PWM},
  {4,  PIO_ANALOG, (PIN_ATTR_DIGITAL|PIN_ATTR_ANALOG), ADC_CHANNEL1, NOT_ON_PWM},
  
  //19/20-IIC (Wire)
  {0, PIO_INPUT_OUTPUT, PIN_ATTR_NONE, NO_ADC_CHANNEL, NOT_ON_PWM},//scl
  {1, PIO_INPUT_OUTPUT, PIN_ATTR_NONE, NO_ADC_CHANNEL, NOT_ON_PWM},//sda
  
  //21/22/23-SPI
  {30, PIO_SERCOM, PIN_ATTR_DIGITAL, NO_ADC_CHANNEL, NOT_ON_PWM},//sck
  {32, PIO_SERCOM, PIN_ATTR_DIGITAL, NO_ADC_CHANNEL, NOT_ON_PWM},//mosi
  {33, PIO_SERCOM, PIN_ATTR_DIGITAL, NO_ADC_CHANNEL, NOT_ON_PWM},//miso
  
  //24/25 usb cdc
  {25, PIO_SERCOM, PIN_ATTR_NONE, NO_ADC_CHANNEL, NOT_ON_PWM},//D24/P25/USBDP/TX_EN
  {28, PIO_SERCOM, PIN_ATTR_NONE, NO_ADC_CHANNEL, NOT_ON_PWM},//D25/P28/USBDN/RX_EN
};

extern "C" {
    unsigned int PINCOUNT_fn() {
        return (sizeof(g_APinDescription) / sizeof(g_APinDescription[0]));
    }
}
/*
const uint32_t g_ADigitalPinMap[] = {
  10,//D0/P10/UART RX
  11,//D1/P11/UART TX
  20,//D2//P20/JTAG TCK/QSPI IO3 Flash download port clock
  21,//D3/P21/JTAG TMS/QSPI IO2 Flash download chip enable signal at download mode
  5, //D4/P5
  22,//D5/P22/JTAG TDI/CLK_26M/QSPI IO1 FLASH download data input at download mode
  23,//D6/P23/JTAG TDO/ADC/QSPI IO0 FLASH download data output at download mode
  6, //D7/P6/13MHz PWM0
  0, //D8
  
  7, //D9/P7/WiFi active outputor PWM1
  36,//D10/P36/SD_D0/DVP D4
  37,//D11
  24,//D12/P24/low power clock output/PWM4/QSPI_CLK
  26,//D13/P26/IrDA input/PWM5/QSPI_CS
  
  12,//D14/A0/P12/ADC6/UART1 CTS
  13,//D15/A1/P13/ADC7/UART1 RTS
  2, //D16/A2/P2/ADC2/I2S DOUT
  3, //D17/A3/P3/ADC4/I2S SYNC
  4, //D18/A4/P4/ADC1/I2S DIN
  
  0, //D19/P0/SCL 2/UART2 TX
  1, //D20/P1/SDA 2/UART2 RX

  30,//D21/P30/SPI SCK
  32,//D22/P32/SPI MOSI
  33,//D23/P33/SPI MISO

  25,//D24/P25/USBDP/TX_EN
  28,//D25/P28/USBDN/RX_EN

};

*/