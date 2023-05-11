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
#include <Adafruit_NeoPixel.h>
#include "neo_functions.h"../*
 * neopixel reference:
 * https://learn.adafruit.com/adafruit-neopixel-uberguide
 */

#define PIN_STRIP A3

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(24, PIN_STRIP, NEO_GRB + NEO_KHZ800);


/*
 * private variables
 */
int rainbow_color_index;
int blink_duration, blink_downcounter;  
bool blink_status;

int  current_mode;        // current display mode
int  color_background, color_foreground;  // index into array of colors
int  window_width, window_center, last_window_width, last_window_center;

uint32_t colors[16] = { COLOR_BLACK, COLOR_WHITE, COLOR_RED, COLOR_YELLOW, COLOR_GREEN,
  COLOR_CYAN, COLOR_BLUE, COLOR_PURPLE, COLOR_ORANGE, COLOR_GRAY };

/*
 * private function templates
 */
uint32_t Wheel(byte WheelPos);
uint32_t WheelBlue(byte WheelPos);
uint32_t WheelGray(byte WheelPos);
void neo_fill(int color_index);

/*
 * public functions
 */

void neo_init() {  
  strip.begin();
  strip.setBrightness(50);
  strip.show(); // Initialize all pixels to 'off'  
  
  //current_mode = MODE_OFF;
  current_mode = MODE_RAINBOW_GRAY;    // for testing
  color_background = colors[0]; // black   NOTE THIS IS ACTUAL RGB value, not index
  color_foreground = colors[1]; // white   NOTE THIS IS ACTUAL RGB value, not index
  window_width = 8;
  last_window_width = 0;
  window_center = 11;
  last_window_center = 0;
  blink_duration = 50;    // 50 x 10mS = 500 mS
  blink_downcounter = 0;
  blink_status = false;
}

int neo_get_mode() {
  return current_mode;
}

void neo_set_mode(int mode) {
  current_mode = mode;
  if (mode == MODE_WINDOWED) {    
    last_window_width = 0;
    last_window_center = 0;
  }  
  if (mode == MODE_FLASHING) {
    blink_downcounter = -1;   // make it toggle at next loop
    blink_status = false;     // and pretend LEDs are currently off, so they'll turn on
  }
  if ((mode == MODE_RAINBOW) || (mode == MODE_RAINBOW_BLUE) || (mode == MODE_RAINBOW_GRAY)) {
    rainbow_color_index = 0;
  }
  if (mode == MODE_SOLID) {    
    neo_fill_background();
  }
  if (mode == MODE_OFF) {    
    neo_fill(0);
  }
}

void neo_fill_background() {
  strip.fill(color_background);
  strip.show();
}
void neo_set_background(int color_index) {
  color_background = colors[color_index];
}

void neo_fill_foreground() {
  strip.fill(color_foreground);
  strip.show();
}
void neo_set_foreground(int color_index) {
  color_foreground = colors[color_index];
}

void neo_set_win_width(int width) {
  window_width = width;
}

void neo_set_win_center(int center) {
  window_center = center;
}

void neo_fill(int color_index) {
  strip.fill(colors[color_index]);
  strip.show();
}

// cycle and display flash status if appropriate
void neo_handle_flash() {  
  blink_downcounter -= 1;
  if (blink_downcounter < 0) {
    if (blink_status) {
      blink_status = false;
      strip.fill(color_background);
      strip.show();
    } else {
      blink_status = true;
      strip.fill(color_foreground);
      strip.show();          
    }
    blink_downcounter = blink_duration;
  }
}

// adjust window display, if needed
void neo_handle_window() {
  if ((last_window_width != window_width) || (last_window_center != window_center)) {
    strip.fill(color_background);
    strip.fill(color_foreground, (window_center - (window_width/2)), window_width);
    strip.show();
    last_window_width = window_width;
    last_window_center = window_center;
  }
}

// this cycles a rainbow in multitasking environment
// it should be called periodically to update the display.  approx 10mS is good
void neo_cycle_rainbow() {
  int i;
  
  rainbow_color_index++;
  if (rainbow_color_index > 255) {
    rainbow_color_index = 0;
  }
  for(i=0; i< strip.numPixels(); i++) {
    if (current_mode == MODE_RAINBOW) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + rainbow_color_index) & 255));
    } else if (current_mode == MODE_RAINBOW_BLUE) {        
      strip.setPixelColor(i, WheelBlue(((i * 256 / strip.numPixels()) + rainbow_color_index) & 255));
    } else if (current_mode == MODE_RAINBOW_GRAY) {
      strip.setPixelColor(i, WheelGray(((i * 256 / strip.numPixels()) + rainbow_color_index) & 255));
    }
  }
  strip.show();
}

/*
 * private functions
 */

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.

uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

uint32_t WheelBlue(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(0, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(0, 255 - WheelPos * 3, 0);
}

uint32_t WheelGray(byte WheelPos) {
  WheelPos = 255 - WheelPos;
    return strip.Color(WheelPos, WheelPos, WheelPos);
}
