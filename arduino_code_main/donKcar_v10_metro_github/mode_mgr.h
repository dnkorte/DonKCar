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
#ifndef MODE_H
#define MODE_H

#include <Arduino.h>
#include "config.h"

#define MODE_INITIALIZING 0
#define MODE_IDLE         1
#define MODE_MANUAL1      2   // Throt/Steer
#define MODE_MANUAL2      3   // Steer Only
#define MODE_AUTO         4
#define MODE_CONFIGURING  5
#define MODE_LEARNING     6
#define MODE_MENU         7
#define MODE_ERROR_BATT   8
#define MODE_ERROR_HBEAT  9
#define MODE_WAITING_CNX  10

void mode_init(void);
void mode_notice_heartbeat(void);
void mode_check_heartbeat();
void mode_check_menu_timeout();
void mode_set_mode(int newMode);
void mode_set_substatus(int newStatus);
bool mode_motion_permitted();
void mode_z_button_event(int action);
void mode_c_button_event(int action);
void mode_joyX_event(int myValue);
void mode_joyY_event(int myValue);
void mode_menu_indexer();
void mode_menu_itemselect();
void mode_check_webap_heartbeat();
void mode_notice_webap_heartbeat(void);
void mode_init_webap_heartbeat();

#endif   /* MODE_H */ 
