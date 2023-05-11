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
#ifndef NEO_FUNC_H
#define NEO_FUNC_H
#include <Arduino.h>

#define MODE_OFF          0
#define MODE_RAINBOW      1
#define MODE_RAINBOW_BLUE 2
#define MODE_RAINBOW_GRAY 3
#define MODE_SOLID        4
#define MODE_FLASHING     5
#define MODE_WINDOWED     6

#define COLOR_BLACK       0X000000
#define COLOR_WHITE       0xFFFFFF
#define COLOR_RED         0xFF0000
#define COLOR_YELLOW      0xFF9600 
#define COLOR_GREEN       0x00FF00
#define COLOR_CYAN        0x00FFFF
#define COLOR_BLUE        0x0000FF
#define COLOR_PURPLE      0xB400FF
#define COLOR_ORANGE      0x731900
#define COLOR_GRAY        0x101010
#define NUM_OF_COLORS 10

void neo_init();
int neo_get_mode();
void neo_set_mode(int mode);
void neo_fill_background();
void neo_set_background(int color_index);
void neo_fill_foreground();
void neo_set_foreground(int color_index);
void neo_fill_color(int color_index);
void neo_set_win_width(int width);
void neo_set_win_center(int ctr);
void neo_handle_flash();
void neo_handle_window();
void neo_cycle_rainbow();

#endif  /* NEO_FUNC_H */
