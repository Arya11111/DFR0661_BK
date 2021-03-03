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

#ifndef _VARIANT_BBC_MICROBIT_
#define _VARIANT_BBC_MICROBIT_

/** Master clock frequency */
#define VARIANT_MCK       (16000000ul)

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include "WVariant.h"


#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus
#include "pin.h"
#include "drv_gpio.h"

#ifdef __cplusplus
extern "C" unsigned int PINCOUNT_fn();
#endif

// Number of pins defined in PinDescription array
#define PINS_COUNT           (PINCOUNT_fn())
#define NUM_DIGITAL_PINS     (20u)
#define NUM_ANALOG_INPUTS    (6u)
#define NUM_ANALOG_OUTPUTS   (0u)

// LEDs
#define PIN_LED                 (13)
#define LED_BUILTIN             PIN_LED
// #define

/*
// Buttons
#define PIN_BUTTON_A            (5)
#define PIN_BUTTON_B            (11)
*/

/*
 * Analog pins
 */
#define PIN_A0               (14)
#define PIN_A1               (15)
#define PIN_A2               (16)
#define PIN_A3               (17)
#define PIN_A4               (18)
//#define PIN_A5               (10)

#include "pins_arduino.h"
//static const uint8_t A0  = PIN_A0 ;
//static const uint8_t A1  = PIN_A1 ;
//static const uint8_t A2  = PIN_A2 ;
//static const uint8_t A3  = PIN_A3 ;
//static const uint8_t A4  = PIN_A4 ;
//static const uint8_t A5  = PIN_A5 ;
//#define ADC_RESOLUTION    10

/*
 * Serial interfaces
 */
// Serial
#define UART_INTERFACES_COUNT 1

#define PIN_SERIAL_RX       (21)
#define PIN_SERIAL_TX       (22)

/*
 * SPI Interfaces
 */
#define SPI_INTERFACES_COUNT 1

#define PIN_SPI_MISO         (23)
#define PIN_SPI_MOSI         (22)
#define PIN_SPI_SCK          (21)


static const uint8_t SS   = 16 ;
static const uint8_t MOSI = PIN_SPI_MOSI ;
static const uint8_t MISO = PIN_SPI_MISO ;
static const uint8_t SCK  = PIN_SPI_SCK ;

/*
 * Wire Interfaces
 */
#define WIRE_INTERFACES_COUNT 1

#define PIN_WIRE_SDA         (20u)
#define PIN_WIRE_SCL         (19u)

static const uint8_t SDA = PIN_WIRE_SDA;
static const uint8_t SCL = PIN_WIRE_SCL;
#define GDL_PIN_INTERFACE

#ifdef GDL_PIN_INTERFACE
#define GPIO_PIN_NONE 255

#define GDL_PIN_BLK      9
#define GDL_PIN_SCLK     PIN_SPI_MISO
#define GDL_PIN_MOSI     PIN_SPI_MOSI
#define GDL_PIN_MISO     PIN_SPI_MISO
#define GDL_PIN_DC       7
#define GDL_PIN_RES      6
#define GDL_PIN_CS       5
#define GDL_PIN_SDCS     3
#define GDL_PIN_FCS      A4
#define GDL_PIN_TCS      A3
#define GDL_PIN_SCL      PIN_WIRE_SCL
#define GDL_PIN_SDA      PIN_WIRE_SDA
#define GDL_PIN_INT      2
#define GDL_PIN_BUSY_TE  10
#define GDL_PIN_X1   GPIO_PIN_NONE
#define GDL_PIN_X2   GPIO_PIN_NONE
#endif

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
unsigned int PINCOUNT_fn();
#ifdef __cplusplus
}
#endif

#endif
