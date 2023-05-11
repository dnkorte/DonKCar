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
 * SOFTWARE. 
 *
 * *************************************************************************************
 * *************************************************************************************
 * *************************************************************************************
 *  support functions
 * *************************************************************************************
 * *************************************************************************************
 * *************************************************************************************
 */

 #include "general_support_lib.h"
 
int sign(int x) {
  if (x < 0) {
    return -1;
  }
  return 1;
}

/*
 * "fixes" a 1-byte truncated integer as sent by python over i2c
 * to a signed number.  (note that if python program truncates
 * a signed integer to a single (lower) byte, then negative numbers
 * look like positive numbers > 127)
 */
int fix_to_signed_int(uint8_t param) {  
  int rtnval;
  
  if (param < 128) {
    rtnval = param;
  } else {
    rtnval =  (int) (256 - param) * (-1);
  }
  return rtnval;
}
