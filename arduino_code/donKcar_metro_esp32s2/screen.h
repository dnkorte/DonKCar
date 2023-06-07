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
#ifndef SCREEN_H
#define SCREEN_H

#include <Arduino.h>
#include "config.h"

//#include <Adafruit_SSD1306.h>
//#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>

// screen locations for text
#define ROW_BATT_E 1
#define ROW_BATT_M 2
#define ROW_MAC 3
#define ROW_STAT1 4
#define ROW_STAT2 5   // note this is used for bootup progress indications (status_set_info_msg)
#define ROW_STAT3 6
#define ROW_STAT4 7
#define ROW_THROT 8
#define ROW_RACERNAME 8
#define ROW_STAT4 9   // note this is used for menu mode names, etc (bottom row) (status_set_status_msg)

#define WIDTH_HEADER 5
#define WIDTH_FULL 20

#define COL_LEFTEDGE 0
#define COL_DATA 5
#define WIDTH_DATA 15
#define COL_BATTV 5
#define COL_BATTV_LABEL 10
#define COL_THROT_L 0
#define COL_THROT_R (WIDTH_FULL-4)


//#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789

// RGB888 to 565 converter  https://trolsoft.ru/en/articles/rgb565-color-picker
// 565 color picker https://chrishewett.com/blog/true-rgb565-colour-picker/
// 565 color picker http://www.barth-dev.de/online/rgb565-color-picker/
// https://trolsoft.ru/en/articles/rgb565-color-picker  
// http://www.barth-dev.de/online/rgb565-color-picker/ 
// http://drakker.org/convert_rgb565.html

// note that the colors defined here are for standard RGR565 TFT
// and probably works with most any RGB565 type device, but could
// be changed if an OLED or an 8 bit RGB display is used

#define COLOR_FOREGROUND ST77XX_WHITE
#define COLOR_BACKGROUND ST77XX_BLACK
#define COLOR_HEADER ST77XX_BLUE
#define COLOR_RED ST77XX_RED
#define COLOR_MAGENTA ST77XX_MAGENTA
#define COLOR_ORANGE ST77XX_ORANGE
#define COLOR_YELLOW ST77XX_YELLOW
#define COLOR_GREEN ST77XX_GREEN
#define COLOR_BLUE ST77XX_BLUE
#define COLOR_CYAN ST77XX_CYAN
#define COLOR_WHITE ST77XX_WHITE
#define COLOR_BLACK ST77XX_BLACK
#define COLOR_PURPLE 0xB814

/*
 * NOTE these colors are meaningless here, just to save
 * them for future builds.  they represent full 8 bit RGB colors
 * 
 * the actual color definitopms ised are int eh status.h file
 * and must be adjusted if the display used is not a standard
 * RGB565 display
 */
#define COLOR_GRAY 0x8430           // 0x888888
#define COLOR_LIGHT_GRAY 0xe75c     // 0xEEEEEE
#define COLOR_MED_GRAY 0xBDF7       // 0xC0C0C0
#define COLOR_DARK_GRAY 0x4228      // 0x484848
#define COLOR_ORANGE 0xFCE0         //  0xFFa000
#define COLOR_DARK_ORANGE  0xEC20   // 0xF18701
#define COLOR_LIGHT_BLUE  0x5C99    // 0x5C92D1
#define COLOR_PASTEL_GREEN  0x1EEF  // 0x19DF82
#define COLOR_SMOKY_GREEN 0x042D    // 0x03876D
#define COLOR_DARK_GREEN  0x04E0    // 0x00a000
#define COLOR_MELLOW_YELLOW 0xEF40  // 0xF4ED06
#define COLOR_LIGHT_YELLOW  0xFEE0  // 0xFFE000
#define COLOR_DARK_RED 0xD040       // 0xDE0A07
#define COLOR_PINK  0xFDF8          // 0xFFC0CB
#define COLOR_DEEP_PURPLE 0x8046    // 0x890C32
#define COLOR_VIOLET  0xD819        // 0xE300D2
#define COLOR_BROWN  0x91E4         // 0x9B3E25

void screen_init();
void screen_show();
void screen_clear();
void screen_clearLine(int rowindex);
int ccToRGB(char colorcode);
//void screen_dot_blink(int ulx, int uly, int color );
//int screen_get_pixel_x(int col);
//int screen_get_pixel_y(int rowindex);
//int screen_get_last_col_index();

void screen_writeText_xrow(int x, int rowindex, int w, char myText[], int color );
void screen_writeText_colrow(int col, int rowindex, int nc, char myText[], int color );
void screen_writeString_xrow(int x, int rowindex, int w, String myString, int color );
void screen_writeString_colrow(int col, int rowindex, int nc, String myString, int color );

void screen_centerText(int rowindex, char myText[], int color);
void screen_centerString(int rowindex, String myString, int color);

#endif  /* SCREEN_H */
