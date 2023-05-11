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
#include "i2c_com.h"
#include "commands.h"
#include "neo_functions.h"

#define MY_I2C_ADDR 0x32
#define RAINBOW_CYCLE_DELAY 10    // number of mS between color change

long nextDisplayUpdate;   // millis() to change color

void setup() {
  // give it a couple milliseconds for everything to stabilize before we get started
  delay(30);

  neo_init();
  i2c_init(MY_I2C_ADDR);

  nextDisplayUpdate = millis() + RAINBOW_CYCLE_DELAY;
}

void loop() {
  commands_execute_cmd_if_avail();
  
  int curNeoMode = neo_get_mode();
  
  if (millis() >  nextDisplayUpdate) {   
    if ((curNeoMode == MODE_RAINBOW) || (curNeoMode == MODE_RAINBOW_BLUE) || (curNeoMode == MODE_RAINBOW_GRAY))  {
      neo_cycle_rainbow();
    }
    
    if (curNeoMode == MODE_FLASHING) {
      neo_handle_flash();
    }

    if (curNeoMode == MODE_WINDOWED) {
      neo_handle_window();
    }
    
    nextDisplayUpdate = millis() + RAINBOW_CYCLE_DELAY;
  }  
  
  delay(1);
}
