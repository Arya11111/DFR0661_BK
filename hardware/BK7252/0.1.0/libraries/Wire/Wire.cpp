/*
 * TWI/I2C library for nRF5x
 * Copyright (c) 2015 Arduino LLC. All rights reserved.
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

extern "C" {
#include <string.h>
#include "i2c.h"
}


#include <Arduino.h>
#include <wiring_private.h>

#include "Wire.h"

uint8_t TwoWire::rxBuffer[BUFFER_LENGTH];
uint8_t TwoWire::rxBufferIndex = 0;
uint8_t TwoWire::rxBufferLength = 0;

uint8_t TwoWire::txAddress = 0;
uint8_t TwoWire::txBuffer[BUFFER_LENGTH];
uint8_t TwoWire::txBufferIndex = 0;
uint8_t TwoWire::txBufferLength = 0;

uint8_t TwoWire::transmitting = 0;
void (*TwoWire::user_onRequest)(void);
void (*TwoWire::user_onReceive)(int);

TwoWire::TwoWire():_i2cDev(NULL){}

void TwoWire::begin(void)
{
  rxBufferIndex = 0;
  rxBufferLength = 0;
  txBufferIndex = 0;
  txBufferLength = 0;
  uint32_t param;
  if(!(_i2cDev = rt_device_find("i2c2"))) {
	   rt_kprintf("search device failed123\r\n");
  }else{
	  rt_kprintf("open sucess!\r\n");
  }
  rt_i2c_twi_init(_i2cDev);
  rt_device_open(_i2cDev, RT_DEVICE_OFLAG_RDWR);
  //twi_init();
}
/*
void TwoWire::begin(uint8_t address)
{
  begin();
  rt_i2c_twi_setAddress(_i2cDev, address);
}

void TwoWire::begin(int address)
{
  begin((uint8_t)address);
}*/

void TwoWire::end(void)
{
  rt_i2c_twi_disable(_i2cDev);
  //twi_disable();
}

void TwoWire::setClock(uint32_t clock)
{
  //twi_setFrequency(clock);
  rt_i2c_twi_setFrequency(_i2cDev, clock);
}

uint8_t TwoWire::requestFrom(uint8_t address, uint8_t quantity, uint32_t iaddress, uint8_t isize, uint8_t sendStop)
{
  if (isize > 0) {
  beginTransmission(address);

  if (isize > 3){
    isize = 3;
  }

  while (isize-- > 0)
    write((uint8_t)(iaddress >> (isize*8)));
    endTransmission(false);
  }

  if(quantity > BUFFER_LENGTH){
    quantity = BUFFER_LENGTH;
  }
  uint8_t read = rt_i2c_twi_readFrom(_i2cDev,address, rxBuffer, quantity, sendStop);//twi_readFrom(address, rxBuffer, quantity, sendStop);
  rxBufferIndex = 0;
  rxBufferLength = read;

  return read;
}

uint8_t TwoWire::requestFrom(uint8_t address, uint8_t quantity, uint8_t sendStop) {
  return requestFrom((uint8_t)address, (uint8_t)quantity, (uint32_t)0, (uint8_t)0, (uint8_t)sendStop);
}

uint8_t TwoWire::requestFrom(uint8_t address, uint8_t quantity)
{
  return requestFrom((uint8_t)address, (uint8_t)quantity, (uint8_t)true);
}

uint8_t TwoWire::requestFrom(int address, int quantity)
{
  return requestFrom((uint8_t)address, (uint8_t)quantity, (uint8_t)true);
}

uint8_t TwoWire::requestFrom(int address, int quantity, int sendStop)
{
  return requestFrom((uint8_t)address, (uint8_t)quantity, (uint8_t)sendStop);
}

void TwoWire::beginTransmission(uint8_t address)
{
  transmitting = 1;
  txAddress = address;
  txBufferIndex = 0;
  txBufferLength = 0;
}

void TwoWire::beginTransmission(int address)
{
  beginTransmission((uint8_t)address);
}

uint8_t TwoWire::endTransmission(uint8_t sendStop)
{
  uint8_t ret = rt_i2c_twi_writeTo(_i2cDev,txAddress, txBuffer, txBufferLength, 1, sendStop);//twi_writeTo(txAddress, txBuffer, txBufferLength, 1, sendStop);
  txBufferIndex = 0;
  txBufferLength = 0;
  transmitting = 0;
  return ret;
}

uint8_t TwoWire::endTransmission(void)
{
  return endTransmission(true);
}

size_t TwoWire::write(uint8_t data)
{
  if(transmitting){
    if(txBufferLength >= BUFFER_LENGTH){
      //setWriteError();
      return 0;
    }
    // put byte in tx buffer
    txBuffer[txBufferIndex] = data;
    ++txBufferIndex;
    // update amount in buffer   
    txBufferLength = txBufferIndex;
  }else{
    //twi_transmit(&data, 1);
	rt_i2c_twi_transmit(_i2cDev,&data, 1);
  }
  return 1;
}

size_t TwoWire::write(const uint8_t *data, size_t quantity)
{
  if(transmitting){
    for(size_t i = 0; i < quantity; ++i){
      write(data[i]);
    }
  }else{
    //twi_transmit(data, quantity);
	rt_i2c_twi_transmit(_i2cDev,data, quantity);
  }
  return quantity;
}

int TwoWire::available(void)
{
  return rxBufferLength - rxBufferIndex;
}

int TwoWire::read(void)
{
  int value = -1;
  if(rxBufferIndex < rxBufferLength){
    value = rxBuffer[rxBufferIndex];
    ++rxBufferIndex;
  }
  return value;
}

int TwoWire::peek(void)
{
  int value = -1;
  
  if(rxBufferIndex < rxBufferLength){
    value = rxBuffer[rxBufferIndex];
  }
  return value;
}

void TwoWire::flush(void)
{
  // XXX: to be implemented.
}

// behind the scenes function that is called when data is received
void TwoWire::onReceiveService(uint8_t* inBytes, int numBytes)
{
  if(!user_onReceive){
    return;
  }

  if(rxBufferIndex < rxBufferLength){
    return;
  }

  for(uint8_t i = 0; i < numBytes; ++i){
    rxBuffer[i] = inBytes[i];    
  }
  // set rx iterator vars
  rxBufferIndex = 0;
  rxBufferLength = numBytes;
  user_onReceive(numBytes);
}

void TwoWire::onRequestService(void)
{
  if(!user_onRequest){
    return;
  }
  txBufferIndex = 0;
  txBufferLength = 0;
  user_onRequest();
}

void TwoWire::onReceive( void (*function)(int) )
{
  user_onReceive = function;
}

// sets function called on slave read
void TwoWire::onRequest( void (*function)(void) )
{
  user_onRequest = function;
}

TwoWire Wire = TwoWire();