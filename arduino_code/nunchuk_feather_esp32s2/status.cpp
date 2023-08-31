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

#include "status.h"
#include "communic.h"
#include "screen_tft.h"

#include <Adafruit_NeoPixel.h>
#define NUMPIXELS 1
Adafruit_NeoPixel pixels(NUMPIXELS, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);

#define MESSAGE_TIMER_PRESET 300  // 300 intervals of 100ms = 30 sec

String StatMsg1, StatMsg2, StatMsg3, MenuMsg;
char   StatCC1, StatCC2, StatCC3, MenuCC;
bool   StatNew1, StatNew2, StatNew3, MenuNew;
int    status_message_timer;  // status messages disappear after 30 sec

int  current_screen;

void status_init() {
  screen_init();  
  current_screen = STATUS_SCREEN_MAIN; 
  StatMsg1 = ""; 
  StatMsg2 = ""; 
  StatMsg3 = "";
  MenuMsg = "";
  StatCC1  = 'W';
  StatCC2  = 'W';
  StatCC3  = 'W';
  MenuCC  = 'W';
  StatNew1 = false;
  StatNew2 = false;
  StatNew3 = false;
  MenuNew = false;
  status_message_timer = -1;  // no message timer active
}

// -- make many of these "save" message.   then have a single "display" function that is called from main loop and actually shows anything with a changee

void status_clear_status_area() {
  if (current_screen == STATUS_SCREEN_MAIN) {
    screen_clearLine(ROW_M_STAT1);
    screen_clearLine(ROW_M_STAT2);
    screen_clearLine(ROW_M_STAT3);
    status_message_timer = -1;   // (nothing counting, right now)
    screen_show();
  }
}

void status_save_menu_msg(String message, char colorcode) {
  MenuMsg = message;
  MenuCC  = colorcode;
  MenuNew = true;
}

void status_save_info_msgs(String message1, String message2, String message3, char colorcode) {
  StatMsg1 = message1;
  StatCC1  = colorcode;
  StatNew1 = true;
  
  StatMsg2 = message2;
  StatCC2  = colorcode;
  StatNew2 = true;
  
  StatMsg3 = message3;
  StatCC3  = colorcode;
  StatNew3 = true;  
}
/* 
 *  displays message to one STATx line, as indicated by msg_num
 *  and to ESP-NOW
 */
void status_save_msg(int msg_num, String message, char colorcode) {
  if (current_screen == STATUS_SCREEN_MAIN) {
    if (msg_num == 1) {
      StatMsg1 = message;
      StatCC1  = colorcode;
      StatNew1 = true;
    } else if (msg_num == 2) {
      StatMsg2 = message;
      StatCC2  = colorcode;
      StatNew2 = true;
    } else if (msg_num == 3) {  
      StatMsg3 = message;
      StatCC3  = colorcode;
      StatNew3 = true;  
    }
  }
}

/* 
 *  displays message to ONLY the center status line on display
 *  and to ESP-NOW
 */
void status_save_simple_msg(String message, char colorcode) {
  if (current_screen == STATUS_SCREEN_MAIN) {
  StatMsg1 = "";
  StatCC1  = colorcode;
  StatNew1 = true;
  
  StatMsg2 = message;
  StatCC2  = colorcode;
  StatNew2 = true;
  
  StatMsg3 = "";
  StatCC3  = colorcode;
  StatNew3 = true;  
  }
}

void status_display_mainscreen_messages() {
  if (current_screen == STATUS_SCREEN_MAIN) {
    if (StatNew1) {
      screen_centerString(ROW_M_STAT1, StatMsg1, ccToRGB(StatCC1));
      StatNew1 = false;
      status_message_timer = MESSAGE_TIMER_PRESET;
    }
    if (StatNew2) {
      screen_centerString(ROW_M_STAT2, StatMsg2, ccToRGB(StatCC2));
      StatNew2 = false;
      status_message_timer = MESSAGE_TIMER_PRESET;
    }
    if (StatNew3) {
      screen_centerString(ROW_M_STAT3, StatMsg3, ccToRGB(StatCC3));
      StatNew3 = false;
      status_message_timer = MESSAGE_TIMER_PRESET;
    }
    if (MenuNew) {
      screen_centerString(ROW_M_MENU_INFO, MenuMsg, ccToRGB(MenuCC));
      MenuNew = false;
    }
  }  
}

void status_disp_batt_volts(char battcode, float battvolts, float cellvolts, char colorcode) {
  char tmpBuf[12];
  
  if (current_screen == STATUS_SCREEN_MAIN) { 
    if (battcode == 'E') {
      dtostrf(battvolts, 4, 1, tmpBuf);
      screen_writeText_colrow(COL_M_BATT_LAB_E+2, ROW_M_BATT1, 5, tmpBuf, ccToRGB(colorcode));
      
      //dtostrf(cellvolts, 4, 1, tmpBuf);
      //screen_writeText_colrow(COL_M_BATT_LAB_E+2, ROW_M_BATT2, 5, tmpBuf, ccToRGB(colorcode));
    }
    if (battcode == 'M') {
      dtostrf(battvolts, 4, 1, tmpBuf);
      screen_writeText_colrow(COL_M_BATT_LAB_M+2, ROW_M_BATT1, 5, tmpBuf, ccToRGB(colorcode));
      
      //dtostrf(cellvolts, 4, 1, tmpBuf);
      //screen_writeText_colrow(COL_M_BATT_LAB_M+2, ROW_M_BATT2, 5, tmpBuf, ccToRGB(colorcode));
    }
    if (battcode == 'C') {
      dtostrf(battvolts, 4, 1, tmpBuf);
      screen_writeText_colrow(COL_M_BATT_LAB_C+2, ROW_M_BATT1, 5, tmpBuf, ccToRGB(colorcode));
    }
  }
}

void status_message_area_clear_check() {
  if (status_message_timer > -1) {
    status_message_timer--;
    if (status_message_timer <= 0) {
      status_clear_status_area();
      status_message_timer = -1;
    }
  }
}

void status_neopixel_flash(int color) {
  pixels.fill(color);  // RED indicates past serial
  pixels.show(); 
  delay(500);
  pixels.fill(0x000000);
  pixels.show(); 
  delay(500);
}

void status_main_structure() {
  if (current_screen == STATUS_SCREEN_MAIN) {
    screen_writeText_colrow(COL_M_BATT_LAB_E, ROW_M_BATT1, 2, "E:", ccToRGB('C'));
    screen_writeText_colrow(COL_M_BATT_LAB_M, ROW_M_BATT1, 2, "M:", ccToRGB('C'));
    screen_writeText_colrow(COL_M_BATT_LAB_C, ROW_M_BATT1, 2, "C:", ccToRGB('C'));
    
    //screen_writeText_colrow(COL_M_BATT_LAB_E, ROW_M_BATT2, 2, "E:", ccToRGB('C'));
    //screen_writeText_colrow(COL_M_BATT_LAB_M, ROW_M_BATT2, 2, "M:", ccToRGB('C'));
    //screen_writeText_colrow(COL_M_BATT_LAB_C, ROW_M_BATT2, 2, "C:", ccToRGB('C'));
    
    //screen_writeText_colrow(COL_M_NUN_X_LAB, ROW_M_NUNCHUK_INFO, 2, "y", ccToRGB('C'));
    //screen_writeText_colrow(COL_M_NUN_Y_LAB, ROW_M_NUNCHUK_INFO, 2, "x", ccToRGB('C'));
    screen_show();
  }
}

void status_neopixels_init() {
  // turn on the Neopixel power supply
  // https://learn.adafruit.com/esp32-s2-reverse-tft-feather/pinouts
  pinMode(NEOPIXEL_POWER, OUTPUT);
  digitalWrite(NEOPIXEL_POWER, HIGH);
  delay(10);
  pixels.begin();
  pixels.setBrightness(20);
}
