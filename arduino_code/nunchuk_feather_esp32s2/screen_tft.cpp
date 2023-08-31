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
 *
 * ***********************************************************************************
 *
 * Adafruit GFX library     https://learn.adafruit.com/adafruit-gfx-graphics-library/overview
 * Adafruit ST7789 library  https://github.com/adafruit/Adafruit-ST7735-Library
 * for 240x240 1.54in TFT   https://www.adafruit.com/product/3787
 */
 
#include <Arduino.h>
#include "config.h"

#ifdef WANT_TFT

#include <Adafruit_GFX.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSansBold12pt7b.h>
#include "screen_tft.h"

// For 1.14", 1.3", 1.54", 1.69", and 2.0" TFT with ST7789:
//Adafruit_ST7789 display = Adafruit_ST7789(PIN_TFT_CS, PIN_TFT_DC, PIN_TFT_RST); 

// this one works for Feather Reverse TFT
Adafruit_ST7789 display = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);


//int row_tops[] = { 2, 26, 50, 74, 98, 122 };
//int row_tops[] = { 2, 20, 38, 56, 74, 92, 110 };
int row_tops[] = { 2, 24, 46, 68, 90, 112 };



/*
 * *************************************************
 * private function templates
 * *************************************************
*/

void screen_clearLine_raw(int y);
void screen_centerText_raw(int y, char myText[], int color);
void screen_writeText_raw(int x, int y,  int width_px, char myText[], int color);
void screen_writeString_raw(int x, int y, int nc, String myString, int color);

/*
 * *************************************************
 * public functions 
 * *************************************************
*/


/*
 * *************************************************
 * basic utility functions
 * *************************************************
 */
void screen_init() {
  
  // turn on backlite
  pinMode(TFT_BACKLITE, OUTPUT);
  digitalWrite(TFT_BACKLITE, HIGH);

  // turn on the TFT / I2C power supply
  pinMode(TFT_I2C_POWER, OUTPUT);
  digitalWrite(TFT_I2C_POWER, HIGH);
  delay(10);   
  
  display.init(SCREEN_HEIGHT, SCREEN_WIDTH);
  
  display.fillScreen(ST77XX_BLACK);
  
  // note the BLACK background color is needed so that previous contents are covered
  //display.setFont(&FreeSansBold12pt7b);
  display.setTextColor(COLOR_FOREGROUND, COLOR_BACKGROUND);   
  display.setRotation(3);
  display.setTextSize(DEFAULT_SCALE);
  //display.display();
}

void screen_show() {
  //display.display();
}

void screen_clear() { 
  display.fillScreen(ST77XX_BLACK);
}

void screen_clearLine(int rowindex) {
  int myrow = constrain(rowindex, 0, (NUM_ROWS-1)); 
  screen_clearLine_raw(row_tops[myrow]);
}

void screen_clearField(int col, int rowindex, int nc) {
  int myrow = constrain(rowindex, 0, (NUM_ROWS-1)); 
  int mywidth = nc * CELL_WIDTH;
  int myX = col * CELL_WIDTH; 
  int myY = row_tops[myrow];  
  display.fillRect(myX, myY, mywidth, CELL_HEIGHT, COLOR_BACKGROUND);
}


/*
 * *************************************************
 * write text from char array
 * *************************************************
 */

// @param nc  width of field as number of characters

void screen_writeText_colrow(int col, int rowindex, int nc, char myText[], int color) {
  int myrow = constrain(rowindex, 0, (NUM_ROWS-1));
  int mywidth = nc * CELL_WIDTH;
  int myX = col * CELL_WIDTH; 
  screen_writeText_raw(myX, row_tops[myrow], mywidth, myText, color);
}



// this displays text (array of chars),  horizontally centered
// @param int y     is y index of top of chars (0=top of screen, SCREEN_HEIGHT is bottom of screen)

void screen_centerText(int rowindex, char myText[], int color) {
  int myrow = constrain(rowindex, 0, (NUM_ROWS-1));  
  screen_centerText_raw(row_tops[myrow], myText, color);
}


// @param nc  width of field as number of characters

void screen_writeText_xrow(int x, int rowindex, int nc, char myText[], int color) {
  int myrow = constrain(rowindex, 0, (NUM_ROWS-1));
  int mywidth = nc * CELL_WIDTH; 
  screen_writeText_raw(x, row_tops[myrow], mywidth, myText, color);
}




/*
 * *************************************************
 * write text from Arduino String
 * *************************************************
 */


void screen_writeString_colrow(int col, int rowindex, int nc, String myString, int color) {
  char myText[81];
  myString.toCharArray(myText, 80); 
  screen_writeText_colrow(col, rowindex, nc, myText, color);
}


// this displays text (String),  horizontally centered
// @param int y     is y index of top of chars (0=top of screen, SCREEN_HEIGHT is bottom of screen)

void screen_centerString(int rowindex, String myString, int color) {
  char myText[81];
  myString.toCharArray(myText, 80); 
  screen_centerText(rowindex, myText, color);
}


void screen_writeString_xrow(int x, int rowindex, int nc, String myString, int color) {
  char myText[81];
  myString.toCharArray(myText, 80); 
  screen_writeText_xrow(x, rowindex, nc, myText, color);
}



/*
 * *************************************************
 * public low level functions
 * *************************************************
 */


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
/*
 * colorcode char for messages is as follows:
 *  G - Green
 *  Y - Yellow
 *  O - Orange
 *  R - Red
 *  W - White
 *  X - Black (background)
 *  B - Blue
 *  C - Cyan
 *  P - Purple
 *  Z - Medium Gray
 *  H - Header
 */
 
int ccToRGB(char colorcode) {
  switch(colorcode) {
    case 'G':
      return COLOR_GREEN;
    case 'Y':
      return ST77XX_YELLOW;
    case 'O':
      return COLOR_ORANGE;
    case 'R':
      return COLOR_RED;
    case 'W':
      return COLOR_WHITE;
    case 'X':
      return COLOR_BLACK;
    case 'B':
      return COLOR_BLUE;
    case 'C':
      return COLOR_CYAN;
    case 'P':
      return COLOR_PURPLE;
    case 'Z':
      return COLOR_MED_GRAY;
    case 'H':
      return COLOR_HEADER;
    default:
      return COLOR_WHITE;
  }
}

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

#endif  /* WANT_TFT */
