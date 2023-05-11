/*
 * Summary: remote controller for robots/racers uses nunchuk as actuator
 *    communicates to robot/racer using ESP-NOW protocol; this version
 *    runs on an Adafruit QTPy ESP32-S2 and has an oled display
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
#ifndef SCREEN_H
#define SCREEN_H

#include <Arduino.h>
#include "config.h"

#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define SCREEN_ADDRESS 0x3C // See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

#define DEFAULT_SCALE 1
#define CHAR_WIDTH  (5*DEFAULT_SCALE)
#define CHAR_HEIGHT (7*DEFAULT_SCALE)
#define CELL_WIDTH  (CHAR_WIDTH+DEFAULT_SCALE)
#define CELL_HEIGHT (CHAR_HEIGHT+DEFAULT_SCALE)
#define CHARS_PER_LINE (SCREEN_WIDTH / CELL_WIDTH)
#define RIGHTMOST_COLUMN (CHARS_PER_LINE - 1)

#define NUM_ROWS 4

// screen locations for text
#define ROW_SSID 0
#define ROW_IP 1
#define ROW_RSSI 2
#define ROW_STAT1 3
#define ROW_BATT 0
#define ROW_MAC 1

#define COL_LEFTEDGE 0
#define COL_DATA 5
#define WIDTH_HEADER 5
#define WIDTH_FULL CHARS_PER_LINE 
#define WIDTH_DATA (CHARS_PER_LINE - WIDTH_HEADER)

#define COL_LABEL_Y 0
#define COL_VALUE_Y 2
#define COL_LABEL_X 7
#define COL_VALUE_X 9
#define COL_BTN_C 14
#define COL_BTN_Z 15
#define COL_BTN_B 13
#define COL_COMMSTAT 17
#define WIDTH_COMMSTAT 4

#define COLOR_FOREGROUND SSD1306_WHITE
#define COLOR_BACKGROUND SSD1306_BLACK
#define COLOR_HEADER SSD1306_WHITE
#define COLOR_WHITE SSD1306_WHITE
#define COLOR_BLACK SSD1306_BLACK


void screen_init();
void screen_show();
void screen_clear();
void screen_clearLine(int rowindex);
void screen_dot_blink(int ulx, int uly, int color );
int screen_get_pixel_x(int col);
int screen_get_pixel_y(int rowindex);
int screen_get_last_col_index();

void screen_writeText_xrow(int x, int rowindex, int w, char myText[], int color );
void screen_writeText_colrow(int col, int rowindex, int nc, char myText[], int color );
void screen_writeString_xrow(int x, int rowindex, int w, String myString, int color );
void screen_writeString_colrow(int col, int rowindex, int nc, String myString, int color );

void screen_centerText(int rowindex, char myText[], int color);

#endif  /* SCREEN_H */
