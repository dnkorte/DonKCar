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
 *
 * *************************************************************************************
 *
 * Adafruit GFX library     https://learn.adafruit.com/adafruit-gfx-graphics-library/overview
 * Adafruit ST7789 library  https://github.com/adafruit/Adafruit-ST7735-Library
 * for 240x240 1.54in TFT   https://www.adafruit.com/product/3787
 */
 
#include <Arduino.h>
#include "config.h"

#ifdef WANT_OLED

#include "screen_oled.h"
#include <Adafruit_GFX.h>

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

//int row_tops[] = { 1, 15 };
int row_tops[] = { 1, 9, 17, 25 };

bool screen_initialized;

/*
 * *************************************************
 * private functions 
 * *************************************************
*/

void screen_clearLine_raw(int y) {
  display.fillRect(0, y, SCREEN_WIDTH, CELL_HEIGHT, COLOR_BACKGROUND);
}

// this displays text (array of chars), horizontally centered
// @param int y     is y index of top of chars (0=top of screen, SCREEN_HEIGHT is bottom of screen)
// @param int scale is scale of text (1, 2, 3, ...)
void screen_centerText_raw(int y, char myText[], int color) {  
  screen_clearLine_raw(y);
  display.setTextSize(DEFAULT_SCALE);
  int lengthOfText = (CELL_WIDTH) * strlen(myText);
  int thisX = (SCREEN_WIDTH / 2) - (lengthOfText / 2);
  if (thisX < 0) {
    thisX = 0;
  }
  display.setTextColor(color);
  display.setCursor(thisX, y);
  display.print(myText);
  display.setTextSize(DEFAULT_SCALE);
}

// @param  x, y   coordinates of top left in pixels
// @param  nc     width of field as number of characters
// note that the height of the cleared block is enough to contain 1 row of text at specified scale
void screen_writeText_raw(int x, int y,  int width_px, char myText[], int color) {
  int mywidth = width_px; 
  display.fillRect(x, y, mywidth, CELL_HEIGHT, COLOR_BACKGROUND);
  
  display.setTextColor(color);
  display.setTextSize(DEFAULT_SCALE);
  display.setCursor(x,y);
  display.print(myText);
  display.setTextSize(DEFAULT_SCALE);
}

void screen_writeString_raw(int x, int y, int nc, String myString, int color) {
  char myText[81]; 
  myString.toCharArray(myText, 80);
  screen_writeText_raw(x, y, nc, myText, color);
}

/*
 * *************************************************
 * public functions 
 * *************************************************
*/

void screen_init() {  
  screen_initialized = true;
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    DEBUG_PRINTLN(F("SSD1306 allocation failed"));
    //for(;;); // Don't proceed, loop forever
    screen_initialized = false;
  }

  if (screen_initialized) {
    display.fillScreen(COLOR_BLACK);
    
    // note the BLACK background color is needed so that previous contents are covered
    display.setTextColor(COLOR_FOREGROUND, COLOR_BACKGROUND);   
    display.setRotation(0);
    display.setTextSize(DEFAULT_SCALE);
    screen_show();
  }
}

void screen_show() {
  if (!screen_initialized) {
    return;
  }
  display.display();
}

void screen_clear() { 
  if (!screen_initialized) {
    return;
  }
  display.fillScreen(SSD1306_BLACK);
}

void screen_clearLine(int rowindex) {
  int myrow = constrain(rowindex, 0, (NUM_ROWS-1)); 
  if (!screen_initialized) {
    return;
  }
  screen_clearLine_raw(row_tops[myrow]);
}

// @param nc  width of field as number of characters
void screen_writeText_colrow(int col, int rowindex, int nc, char myText[], int color) {
  int myrow = constrain(rowindex, 0, (NUM_ROWS-1));
  int mywidth = nc * CELL_WIDTH;
  int myX = col * CELL_WIDTH; 
  if (!screen_initialized) {
    return;
  }
  screen_writeText_raw(myX, row_tops[myrow], mywidth, myText, color);
}

// @param nc  width of field as number of characters
void screen_writeText_xrow(int x, int rowindex, int nc, char myText[], int color) {
  int myrow = constrain(rowindex, 0, (NUM_ROWS-1));
  int mywidth = nc * CELL_WIDTH; 
  if (!screen_initialized) {
    return;
  }
  screen_writeText_raw(x, row_tops[myrow], mywidth, myText, color);
}

void screen_writeString_colrow(int col, int rowindex, int nc, String myString, int color) {
  char myText[81];
  if (!screen_initialized) {
    return;
  }
  myString.toCharArray(myText, 80); 
  screen_writeText_colrow(col, rowindex, nc, myText, color);
}

void screen_writeString_xrow(int x, int rowindex, int nc, String myString, int color) {
  char myText[81];
  if (!screen_initialized) {
    return;
  }
  myString.toCharArray(myText, 80); 
  screen_writeText_xrow(x, rowindex, nc, myText, color);
}

// this displays text (array of chars),  horizontally centered
// @param int y     is y index of top of chars (0=top of screen, SCREEN_HEIGHT is bottom of screen)
void screen_centerText(int rowindex, char myText[], int color) {
  int myrow = constrain(rowindex, 0, (NUM_ROWS-1));  
  if (!screen_initialized) {
    return;
  }
  screen_centerText_raw(row_tops[myrow], myText, color);
}

void screen_dot_blink(int ulx, int uly, int color) {
  // do nothing 
}

int screen_get_pixel_x(int col) {
  int myX = col * CELL_WIDTH;
  return myX;
}

int screen_get_pixel_y(int rowindex) {
  int myrow = constrain(rowindex, 0, (NUM_ROWS-1));  
  int myY = row_tops[myrow];
  return myY;
}

int screen_get_last_col_index() {
  int lastcol = SCREEN_WIDTH / CELL_WIDTH;
  return lastcol;
}

#endif  /* WANT_OLED */
