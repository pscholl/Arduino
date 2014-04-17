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

#ifndef _MPL115A2_H_
#define _MPL115A2_H_

#include "I2Cdev.h"

#define MPL115A2_DEFAULT_ADDRESS   0x60

#define MPL115A2_RA_PADC           0x00
#define MPL115A2_RA_TADC           0x02
#define MPL115A2_RA_A0             0x04
#define MPL115A2_RA_B1             0x06
#define MPL115A2_RA_B2             0x08
#define MPL115A2_RA_C12            0x0a
#define MPL115A2_RA_C11            0x0c
#define MPL115A2_RA_C22            0x0e
#define MPL115A2_RA_CONVERT        0x12

#define MPL115A2_PADC_LENGTH           10
#define MPL115A2_TADC_LENGTH           10
#define MPL115A2_A0_LENGTH             16
#define MPL115A2_B1_LENGTH             16
#define MPL115A2_B2_LENGTH             16
#define MPL115A2_C12_LENGTH            14

class MPL115A2 {
    public:
        MPL115A2();
        MPL115A2(uint8_t address);

        void initialize();
        bool testConnection();

        // MEASURE1 register, read-only
        float getTemperature();
        float getPressure();

    private:
        void startConversion();
        int16_t a0, b1, b2, c12, c11, c22;
        uint8_t devAddr;
        uint16_t adc[2];
};

#endif /* _MPL115A2_H_ */
