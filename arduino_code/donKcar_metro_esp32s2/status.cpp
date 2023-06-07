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
#include "i2c_com.h"
#include "nunchuk.h"


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
int  current_screen;

void status_init() {
  screen_init();
  current_screen = STATUS_SCREEN_MAIN;
  screen_centerString(ROW_STAT4, "Initializing", COLOR_CYAN);
  
  lastJoyX = 0;
  lastJoyY = 0;
  neo_background_color = NEO_COLOR_BLACK;
  neo_foreground_color = NEO_COLOR_WHITE;
  screen_centerString(ROW_STAT4, "Initializing", COLOR_CYAN);
}

/*
 * *****************************************
 * TFT screen related functions
 * *****************************************
 */
 
void status_disp_menu_msg(String message, char colorcode) {
  char tmpBuf[22];
  if (current_screen == STATUS_SCREEN_MAIN) {
    screen_centerString(ROW_STAT4, message, ccToRGB(colorcode));
  }
  if (nunchuk_is_available()) {
    message.toCharArray(tmpBuf,21);
    nunchuk_send_text(0, colorcode, tmpBuf);
  }
}

void status_disp_info_msgs(String message1, String message2, String message3, char colorcode) {
  char tmpBuf[22];
  if (current_screen == STATUS_SCREEN_MAIN) {
    screen_centerString(ROW_STAT1, message1, ccToRGB(colorcode));
    screen_centerString(ROW_STAT2, message2, ccToRGB(colorcode));
    screen_centerString(ROW_STAT3, message3, ccToRGB(colorcode));
  }
  if (nunchuk_is_available()) {
    message1.toCharArray(tmpBuf,21);
    nunchuk_send_text(1, colorcode, tmpBuf);
    message2.toCharArray(tmpBuf,21);
    nunchuk_send_text(2, colorcode, tmpBuf);
    message3.toCharArray(tmpBuf,21);
    nunchuk_send_text(3, colorcode, tmpBuf);
  }
}

/* 
 *  displays message to ONLY the center status line on display
 *  and to ESP-NOW
 */
void status_disp_simple_msg(String message, char colorcode) {
  char tmpBuf[22];
  String emptiness = "";
  if (current_screen == STATUS_SCREEN_MAIN) {
    screen_centerString(ROW_STAT2, message, ccToRGB(colorcode));
    screen_clearLine(ROW_STAT1);
    screen_clearLine(ROW_STAT3);
  }
  if (nunchuk_is_available()) {
    emptiness.toCharArray(tmpBuf,21);
    nunchuk_send_text(1, colorcode, tmpBuf);
    message.toCharArray(tmpBuf,21);
    nunchuk_send_text(2, colorcode, tmpBuf);
    emptiness.toCharArray(tmpBuf,21);
    nunchuk_send_text(3, colorcode, tmpBuf);
  }
}

void status_disp_clear_status_area() {
  if (current_screen == STATUS_SCREEN_MAIN) {
    screen_clearLine(ROW_STAT1);
    screen_clearLine(ROW_STAT2);
    screen_clearLine(ROW_STAT3);  
  }
}

/*
 * displays racer name centered on appropriate line
 * note that it gets name from config info and the name
 * is horizontally centered on the full line.
 * 
 * this will erase any throttle info that is diaplayed
 * to left or right of the racername
 */
void status_disp_racername_msg(void) {  
  if (current_screen == STATUS_SCREEN_MAIN) {
    screen_centerText(ROW_RACERNAME, config.robot_name, ccToRGB('C') ); 
  }
}

void status_disp_throt_value(char dir, int value, char colorcode) {
  char myBuf[6];
  if (current_screen == STATUS_SCREEN_MAIN) { 
    if ((dir == 'Y') || (dir == 'L')) {
      itoa(value, myBuf,10);
      screen_writeText_colrow(COL_THROT_R, ROW_THROT, 5, myBuf, ccToRGB(colorcode)); 
    } else {
      itoa(value, myBuf,10);
      screen_writeText_colrow(COL_THROT_L, ROW_THROT, 5, myBuf, ccToRGB(colorcode));
    }
  }
}

void status_disp_throt_value(char dir, char * text, char colorcode) {
  if (current_screen == STATUS_SCREEN_MAIN) { 
    if ((dir == 'Y') || (dir == 'R')) {
      screen_writeText_colrow(COL_THROT_R, ROW_THROT, 5, text, ccToRGB(colorcode));
    } else {
      screen_writeText_colrow(COL_THROT_L, ROW_THROT, 5, text, ccToRGB(colorcode));
    }
  }
}

void status_disp_batt_volts(char battcode, float battvolts, float cellvolts, char colorcode) {
  char tmpBuf[12];
  
  if (current_screen == STATUS_SCREEN_MAIN) { 
    if (battcode == 'E') {
      dtostrf(battvolts, 5, 2, tmpBuf);
      screen_writeText_colrow(COL_DATA, ROW_BATT_E, 5, tmpBuf, ccToRGB(colorcode)); 
       
      dtostrf(cellvolts, 5, 2, tmpBuf);
      screen_writeText_colrow(COL_DATA+7, ROW_BATT_E, 1, "(", ccToRGB('W'));
      screen_writeText_colrow(COL_DATA+8, ROW_BATT_E, 5, tmpBuf, ccToRGB(colorcode));
      screen_writeText_colrow(COL_DATA+14, ROW_BATT_E, 2, ")", ccToRGB('W')); 
    } else {
      dtostrf(battvolts, 5, 2, tmpBuf);
      screen_writeText_colrow(COL_DATA, ROW_BATT_M, 5, tmpBuf, ccToRGB(colorcode)); 
       
      dtostrf(cellvolts, 5, 2, tmpBuf);
      screen_writeText_colrow(COL_DATA+7, ROW_BATT_M, 1, "(", ccToRGB('W'));
      screen_writeText_colrow(COL_DATA+8, ROW_BATT_M, 5, tmpBuf, ccToRGB(colorcode));
      screen_writeText_colrow(COL_DATA+14, ROW_BATT_M, 2, ")", ccToRGB('W')); 
    }
  }
}

void status_disp_IP_or_MAC(String address, char flavor) {  
  if (current_screen == STATUS_SCREEN_MAIN) {
    screen_clearLine(ROW_MAC);
    if (flavor == 'M') {
      screen_writeText_colrow(COL_LEFTEDGE, ROW_MAC, WIDTH_HEADER, "M:", ccToRGB('H') );
      screen_writeString_colrow(COL_LEFTEDGE+3, ROW_MAC, WIDTH_FULL-3, address, ccToRGB('C') );
    } else {
      screen_writeText_colrow(COL_LEFTEDGE, ROW_MAC, WIDTH_HEADER, "IP:", ccToRGB('H') );
      screen_writeString_colrow(COL_DATA, ROW_MAC, WIDTH_DATA, address, ccToRGB('C') );
    }  
  }
}

/*
 * note each tick represents 0.5 seconds
 */
void status_disp_webconnect_downcounter(int ticks_left) {
  char tmpBuf[6];
  
  if (current_screen == STATUS_SCREEN_MAIN) {
    itoa(ticks_left/2, tmpBuf,10);
    screen_centerText(ROW_STAT3, tmpBuf, COLOR_YELLOW);
    // in last 15 seconds of "wait for connection" mode, we change neopixels to flash yellow and purple
    if (ticks_left == 30) {
      status_neo_send(NEO_CMD_SETFOREGROUND,NEO_COLOR_ORANGE);      
    }
  }
}

void status_disp_mainpage_skeleton(void) {
  if (current_screen == STATUS_SCREEN_MAIN) {
    status_disp_racername_msg();
    screen_writeText_colrow(COL_LEFTEDGE, ROW_BATT_E, WIDTH_FULL, "Bat E:", ccToRGB('H'));
    screen_writeText_colrow(COL_LEFTEDGE, ROW_BATT_M, WIDTH_FULL, "Bat M:", ccToRGB('H'));
  }
}

/*
 * *****************************************
 * NeoPixel related functions
 * *****************************************
 */

void status_neo_send(int cmd, int param) {
  uint8_t data;

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
