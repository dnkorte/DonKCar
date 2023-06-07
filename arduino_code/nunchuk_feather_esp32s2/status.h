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
#ifndef STATUS_H
#define STATUS_H

#include <Arduino.h>
#include "config.h"


#define STATUS_SCREEN_MAIN      0
#define STATUS_SCREEN_NODISP    1
#define STATUS_SCREEN_SPECIAL   2

void status_init();
void status_clear_status_area();
void status_save_menu_msg(String message, char colorcode);
void status_save_info_msgs(String message1, String message2, String message3, char colorcode);
void status_save_simple_msg(String message, char colorcode);
void status_save_msg(int msg_num, String message, char colorcode);
void status_display_mainscreen_messages();
void status_disp_batt_volts(char battcode, float battvolts, float cellvolts, char colorcode);
void status_main_structure();

void status_neopixels_init();
void status_neopixel_flash(int color);

#endif  // STATUS_H
