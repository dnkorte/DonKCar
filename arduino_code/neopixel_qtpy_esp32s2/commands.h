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
#ifndef COMMANDS_H
#define COMMANDS_H

#include <Arduino.h>
#include "config.h"

typedef void (*MyCallbackFunction) ();

/*
 * command codes (these are sent by master controller
 */
#define NEO_CMD_CHECK_STATUS 0
#define NEO_CMD_SET_READ_REGISTER 8  // set read register 
#define NEO_CMD_WRITE_REGISTER2 9     // write register 3 bytes -- 1/register index, 2/3 are 2-byte register value (low, high)
#define NEO_CMD_WRITE_REGISTER1 10     // write register 2 bytes -- 1/register index, 2/ 1-byte register value (low, high)

#define NEO_CMD_SETMODE         7
#define NEO_CMD_SETBACKGROUND   6
#define NEO_CMD_SETFOREGROUND   5
#define NEO_CMD_SET_WIN_CTR     4
#define NEO_CMD_SET_WIN_WIDTH   3

#define NEO_MODE_DISPLAY_OFF    0
#define NEO_MODE_RAINBOW_FULL   1
#define NEO_MODE_RAINBOW_BLUE   2 
#define NEO_MODE_RAINBOW_GRAY   3
#define NEO_MODE_SOLID          4
#define NEO_MODE_FLASHING       5
#define NEO_MODE_WINDOWED       6


/*
 * * public functions related to commands processor
 */
 
void commands_init();
void commands_execute_cmd_if_avail();

#endif  /* COMMANDS_H */
