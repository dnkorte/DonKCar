/*
 * Summary: this package is a newpixel strip display driver for robots/racers
 *    it runs on an Adafruit QTPy ESP32-S2 board and drives a 24 element 
 *    NeoPixel Strip.  It receives commands over i2c from the robot main controller
 *    and generates approprate displays on the strip
 * 
 * Author(s):  Don Korte
 * Repository: https://github.com/dnkorte/DonKCar
 *
 * MIT License
 * Copyright (c) 2023 Don Korte
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * 0000000
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE. * 
 */
#ifndef I2C_H
#define I2C_H
#include <Arduino.h>

#include "config.h"

/*
 * i2c interface for a "slave" device
 */

/*
 * index to array of bytes that represent the device "registers"
 */
#define REG_MODULE_ID1 0
#define REG_STATUS 1               // 1 byte BUSY/1 or DONE/0 status
#define REG_READREG 2                // 1 byte; register address for pending read

#define NUM_REGISTERS 3



/*
 * public functions relative to communication with host (i2c)
 */

void i2c_init(uint8_t addr);
bool i2c_stream_isEmpty();
uint8_t i2c_stream_pull();

/*
 * public functions relative to register-basted storage
 */

void i2c_register_set_8(int register_index, uint8_t value);
void i2c_register_set_16(int register_index, int value);
void i2c_register_set_32(int register_index, unsigned long value);

uint8_t i2c_register_get_8(int register_index);
int i2c_register_get_16(int register_index);
unsigned long i2c_register_get_32(int register_index);

#endif  /* I2C_H */
