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
#include "status.h"
#include "screen.h"
//#include "serial_com_esp32.h"
#include "i2c_com.h"

/*
 * ***************************************************************
 * the status module manages the neopixel display, and the central
 * "status" rows on the TFT display, as well as the bottom-row
 * menu line.  
 * 
 * messages to other areas of the TFT are driven by talking
 * directly to the screen module
 * ***************************************************************
 */

int  lastJoyX, lastJoyY;
int  neo_background_color, neo_foreground_color;


void status_init() {
  lastJoyX = 0;
  lastJoyY = 0;
  neo_background_color = NEO_COLOR_BLACK;
  neo_foreground_color = NEO_COLOR_WHITE;
  //curStatus = STATUS_INITIALIZING;
  screen_centerString(ROW_STAT4, "Initializing", COLOR_CYAN);
}

void status_set_menu_msg(String message, int color) {
    screen_centerString(ROW_STAT4, message, color);
}

void status_set_info_msg(String message, int color) {
    screen_centerString(ROW_STAT2, message, color);
}



void status_neo_send(int cmd, int param) {
  uint8_t data;
  
  //Serial.print(cmd);
  //Serial.print(">");
  //Serial.println(param);

  data = ((cmd << 5) & 0xE0) | (param & 0x1f);
  //sercom2_sendchar(data);
  //i2c_send_cmd(I2C_NEOPIXEL, data);
  i2c_send_cmd_and_byte(I2C_NEOPIXEL, cmd, param);
}


void status_neo_show_steering_info(int cmd_joyX) {
  int scaledX = cmd_joyX / 24;
  
  if (scaledX != lastJoyX) {
    int window_width = 5;     // just make it always 5
    int window_center = 11 + scaledX;
    //status_neo_send(NEO_CMD_SETMODE,NEO_MODE_WINDOWED);
    status_neo_send(NEO_CMD_SET_WIN_CTR, window_center);
    status_neo_send(NEO_CMD_SET_WIN_WIDTH, window_width);
    lastJoyX = scaledX;
  }
}

void status_neo_show_steering_info(int cmd_joyY, int cmd_joyX, bool forcedisplay) {
  int scaledX = cmd_joyX / 24;
  int scaledY = cmd_joyY / 36;
  
  if ((scaledX != lastJoyX) || (forcedisplay)) {
    int window_width = 5;     // just make it always 5
    int window_center = 11 + scaledX;
    status_neo_send(NEO_CMD_SETMODE,NEO_MODE_WINDOWED);
    status_neo_send(NEO_CMD_SET_WIN_CTR, window_center);
    status_neo_send(NEO_CMD_SET_WIN_WIDTH, window_width);
    lastJoyX = scaledX;
  }
}
void status_neo_show_movement_info(int cmd_joyY, int cmd_joyX) {
  int scaledX = cmd_joyX / 24;
  int scaledY = cmd_joyY / 36;
  
  if ((scaledY != lastJoyY) || (scaledX != lastJoyX)) {
    int window_width = abs(scaledY);
    if (window_width < 1) {
      window_width = 1;
    }
    int window_center = 11 + scaledX;
    //status_neo_send(NEO_CMD_SETMODE,NEO_MODE_WINDOWED);
    status_neo_send(NEO_CMD_SET_WIN_CTR, window_center);
    status_neo_send(NEO_CMD_SET_WIN_WIDTH, window_width);
    lastJoyY = scaledY;
    lastJoyX = scaledX;
  }
}

void status_neo_show_movement_info(int cmd_joyY, int cmd_joyX, bool forcedisplay) {
  int scaledX = cmd_joyX / 24;
  int scaledY = cmd_joyY / 36;
  
  if ((scaledY != lastJoyY) || (scaledX != lastJoyX) || (forcedisplay)) {
    int window_width = abs(scaledY);
    if (window_width < 1) {
      window_width = 1;
    }
    int window_center = 11 + scaledX;
    status_neo_send(NEO_CMD_SETMODE,NEO_MODE_WINDOWED);
    status_neo_send(NEO_CMD_SET_WIN_CTR, window_center);
    status_neo_send(NEO_CMD_SET_WIN_WIDTH, window_width);
    lastJoyY = scaledY;
    lastJoyX = scaledX;
  }
}

/*
 * @param slot    0, 1, 2, 3, 4, 5 describes position
 * @param color   is neopixel color index
 */
void status_neo_show_menu_psn6(int slot, int color) {
    if ((slot < 0) || (slot > 5)) {
      slot = 0;
    }
    int center = 2 + (slot * 4);
    if (center > 21) {
      center = 21;
    }
    status_neo_send(NEO_CMD_SETBACKGROUND,NEO_COLOR_GRAY);        // gray backgound
    status_neo_send(NEO_CMD_SETFOREGROUND,color);    // foreground as requested
    status_neo_send(NEO_CMD_SETMODE,NEO_MODE_WINDOWED);        // set windowed display
    status_neo_send(NEO_CMD_SET_WIN_CTR, center);  // set center point
    status_neo_send(NEO_CMD_SET_WIN_WIDTH, 5);       // set window width to 5
}

/*
 * @param slot    0, 1, 2, 3, 4 describes position
 * @param color   is neopixel color index
 */
void status_neo_show_menu_psn5(int slot, int color) {
    if ((slot < 0) || (slot > 4)) {
      slot = 0;
    }
    int center = 2 + (slot * 5);
    if (center > 21) {
      center = 21;
    }
    status_neo_send(NEO_CMD_SETBACKGROUND,NEO_COLOR_GRAY);        // gray backgound
    status_neo_send(NEO_CMD_SETFOREGROUND,color);    // foreground as requested
    status_neo_send(NEO_CMD_SETMODE,NEO_MODE_WINDOWED);        // set windowed display
    status_neo_send(NEO_CMD_SET_WIN_CTR, center);  // set center point
    status_neo_send(NEO_CMD_SET_WIN_WIDTH, 5);       // set window width to 5
}
