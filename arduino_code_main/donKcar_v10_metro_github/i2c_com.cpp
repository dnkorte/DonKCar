/*
 * Summary: openMV + esp32 based autonomous racer
 * 
 * Author(s):  Don Korte
 * Repository: https://github.com/dnkorte/DonKCar
 *
 * MIT License
 * Copyright (c) 2020 Don Korte
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
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
#include <Wire.h>
#include "i2c_com.h"

/*
 * i2c interface for a "master" device
 */

void i2c_init(void) {
  // if this was using Wire1, then you would have to  explicitly set pins for secondary 
  // i2c port on ESP32 (the stemma connector); since using Wire this is not strictly necessary
  Wire.setPins(SDA, SCL);
  Wire.begin();
}

void i2c_send_cmd_and_int(int i2c_addr, char cmd, short value) {
  byte param_byte;  // 8 bit throttle byte sent to motor controller
  Wire.beginTransmission(i2c_addr);
  Wire.write(cmd);
  param_byte = value & 0xFF;
  Wire.write(param_byte);    
  param_byte = (value >> 8) & 0xFF;
  Wire.write(param_byte);
  Wire.endTransmission();
}

void i2c_send_cmd_and_byte(int i2c_addr, char cmd, byte value) {
  byte param_byte;  // 8 bit throttle byte sent to motor controller
  Wire.beginTransmission(i2c_addr);
  Wire.write(cmd);
  param_byte = value & 0xFF;
  Wire.write(param_byte);
  Wire.endTransmission();
}

void i2c_send_cmd(int i2c_addr, char cmd) {
  Wire.beginTransmission(i2c_addr);
  Wire.write(cmd);
  Wire.endTransmission();
}
