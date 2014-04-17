// I2Cdev library collection - MPL115A2 I2C device class header file
// Based on Freescale MPL115A2 datasheet, [02/2013]
// 2014-02-11 by Philipp M. Scholl <scholl@ess.tu-darmstadt.de>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// Changelog:
//     [2014-02-11] - initial release

/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2011-2014 Philipp M. Scholl, Jeff Rowberg

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
===============================================
*/


#include "MPL115A2.h"

/** Default constructor, uses default I2C address.
 * @see MPL115A2_DEFAULT_ADDRESS
 */
MPL115A2::MPL115A2() {
    devAddr = MPL115A2_DEFAULT_ADDRESS;
}

/** Specific address constructor.
 * @param address I2C address
 * @see MPL115A2_DEFAULT_ADDRESS
 * @see MPL115A2_ADDRESS
 */
MPL115A2::MPL115A2(uint8_t address) {
    devAddr = address;
}

/** Power on and prepare for general usage.
 */
void MPL115A2::initialize() {
  if (!testConnection())
    return;

  /* read correction coefficients */
  I2Cdev::readWord(devAddr, MPL115A2_RA_A0, (uint16_t*) &a0);
  I2Cdev::readWord(devAddr, MPL115A2_RA_B1, (uint16_t*) &b1);
  I2Cdev::readWord(devAddr, MPL115A2_RA_B2, (uint16_t*) &b2);
  I2Cdev::readWord(devAddr, MPL115A2_RA_C12,(uint16_t*) &c12);
  I2Cdev::readWord(devAddr, MPL115A2_RA_C11,(uint16_t*) &c11);
  I2Cdev::readWord(devAddr, MPL115A2_RA_C22,(uint16_t*) &c22);
}

/** Verify the I2C connection.
 * Make sure the device is connected and responds as expected.
 * @return True if connection is valid, false otherwise
 */
bool MPL115A2::testConnection() {
  uint8_t tmp;
  return I2Cdev::readByte(devAddr, MPL115A2_RA_PADC, &tmp);
}

float MPL115A2::getTemperature() {
  startConversion();
  return 25 + ((adc[0] - 498) / -5.35);
}

float MPL115A2::getPressure() {
  /** copied from https://github.com/misenso/MPL115A2-Arduino-Library/blob/master/MPL115A2.cpp */
  signed int siPcomp;
  signed long lt1, lt2, lt3, si_c11x1, si_a11, si_c12x2;
  signed long si_a1, si_c22x2, si_a2, si_a1x1, si_y1, si_a2x2;

  // Step 1 c11x1 = c11 * Padc
  lt1 = (signed long) c11;
  lt2 = (signed long) adc[1];
  lt3 = lt1*lt2;
  si_c11x1 = (signed long) lt3;

  // Step 2 a11 = b1 + c11x1
  lt1 = ((signed long)b1)<<14;
  lt2 = (signed long) si_c11x1;
  lt3 = lt1 + lt2;
  si_a11 = (signed long)(lt3>>14);

  // Step 3 c12x2 = c12 * Tadc
  lt1 = (signed long) c12;
  lt2 = (signed long) adc[0];
  lt3 = lt1*lt2;
  si_c12x2 = (signed long)lt3;

  // Step 4 a1 = a11 + c12x2
  lt1 = ((signed long)si_a11<<11);
  lt2 = (signed long)si_c12x2;
  lt3 = lt1 + lt2;
  si_a1 = (signed long) lt3>>11;

  // Step 5 c22x2 = c22*Tadc
  lt1 = (signed long)c22;
  lt2 = (signed long)adc[0];
  lt3 = lt1 * lt2;
  si_c22x2 = (signed long)(lt3);

  // Step 6 a2 = b2 + c22x2
  lt1 = ((signed long)b2<<15);
  lt2 = ((signed long)si_c22x2>1);
  lt3 = lt1+lt2;
  si_a2 = ((signed long)lt3>>16);

  // Step 7 a1x1 = a1 * Padc
  lt1 = (signed long)si_a1;
  lt2 = (signed long)adc[1];
  lt3 = lt1*lt2;
  si_a1x1 = (signed long)(lt3);

  // Step 8 y1 = a0 + a1x1
  lt1 = ((signed long)a0<<10);
  lt2 = (signed long)si_a1x1;
  lt3 = lt1+lt2;
  si_y1 = ((signed long)lt3>>10);

  // Step 9 a2x2 = a2 * Tadc
  lt1 = (signed long)si_a2;
  lt2 = (signed long)adc[0];
  lt3 = lt1*lt2;
  si_a2x2 = (signed long)(lt3);

  // Step 10 pComp = y1 + a2x2
  lt1 = ((signed long)si_y1<<10);
  lt2 = (signed long)si_a2x2;
  lt3 = lt1+lt2;

  // Fixed point result with rounding
  siPcomp = ((signed long)lt3>>13);

  // decPcomp is defined as a floating point number
  // Conversion to decimal value from 1023 ADC count value
  // ADC counts are 0 to 1023, pressure is 50 to 115kPa respectively
  return (((65.0/1023.0)*siPcomp)+50);
}

void MPL115A2::startConversion() {
  /* start a conversion for both temperature and pressure */
  I2Cdev::writeByte(devAddr, MPL115A2_RA_CONVERT, 1);

  /* TODO: we need to make sure to delay the next conversion by this time */
  delay(6);

  /* a new temperature should be sampled after the conversion completed */
  I2Cdev::readWords(devAddr, MPL115A2_RA_TADC, 2, adc);

  adc[0] = adc[0] >> 6;
  adc[1] = adc[1] >> 6;
}
