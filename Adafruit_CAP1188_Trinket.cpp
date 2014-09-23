/*************************************************** 
  This is a library for the CAP1188 I2C/SPI 8-chan Capacitive Sensor

  Designed specifically to work with the CAP1188 sensor from Adafruit
  ----> https://www.adafruit.com/products/1602

  These sensors use I2C/SPI to communicate, 2+ pins are required to  
  interface
  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#include "Adafruit_CAP1188_Trinket.h"
#include "TinyWireM.h"

uint8_t mySPCR, SPCRback;

Adafruit_CAP1188::Adafruit_CAP1188(int8_t resetpin) {
  // I2C
  _resetpin = resetpin;
  _i2c = true;
}

boolean Adafruit_CAP1188::begin(uint8_t i2caddr) {
  if (_i2c) {
    TinyWireM.begin();
    
    _i2caddr = i2caddr;
  }

  if (_resetpin != -1) {
    pinMode(_resetpin, OUTPUT);
    digitalWrite(_resetpin, LOW);
    delay(100);
    digitalWrite(_resetpin, HIGH);
    delay(100);
    digitalWrite(_resetpin, LOW);
    delay(100);
  }

  readRegister(CAP1188_PRODID);
  

  if ( (readRegister(CAP1188_PRODID) != 0x50) ||
       (readRegister(CAP1188_MANUID) != 0x5D) ||
       (readRegister(CAP1188_REV) != 0x83)) {
    return false;
  }
  // allow multiple touches
  writeRegister(CAP1188_MTBLK, 0); 
  // Have LEDs follow touches
  writeRegister(CAP1188_LEDLINK, 0xFF);
  // speed up a bit
  writeRegister(CAP1188_STANDBYCFG, 0x30);
  return true;
}

uint8_t  Adafruit_CAP1188::touched(void) {
  uint8_t t = readRegister(CAP1188_SENINPUTSTATUS);
  if (t) {
    writeRegister(CAP1188_MAIN, readRegister(CAP1188_MAIN) & ~CAP1188_MAIN_INT);
  }
  return t;
}

void Adafruit_CAP1188::LEDpolarity(uint8_t x) {
  writeRegister(CAP1188_LEDPOL, x);
}

/*********************************************************************/

/**************************************************************************/
/*!
    @brief  Abstract away platform differences in Arduino wire library
*/
/**************************************************************************/
static uint8_t i2cread(void) {
  return TinyWireM.read();
}

/**************************************************************************/
/*!
    @brief  Abstract away platform differences in Arduino wire library
*/
/**************************************************************************/
static void i2cwrite(uint8_t x) {
  TinyWireM.write((uint8_t)x);
}

/**************************************************************************/
/*!
    @brief  Reads 8-bits from the specified register
*/
/**************************************************************************/

uint8_t Adafruit_CAP1188::readRegister(uint8_t reg) {
  if (_i2c) {
    TinyWireM.beginTransmission(_i2caddr);
    i2cwrite(reg);
    TinyWireM.endTransmission();
    TinyWireM.requestFrom(_i2caddr, 1);
    return (i2cread());
  } 
}


/**************************************************************************/
/*!
    @brief  Writes 8-bits to the specified destination register
*/
/**************************************************************************/
void Adafruit_CAP1188::writeRegister(uint8_t reg, uint8_t value) {
  if (_i2c) {
    TinyWireM.beginTransmission(_i2caddr);
    i2cwrite((uint8_t)reg);
    i2cwrite((uint8_t)(value));
    TinyWireM.endTransmission();
  }
}
