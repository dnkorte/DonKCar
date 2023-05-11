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
#ifndef STATUS_H
#define STATUS_H

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

#include <Arduino.h>
#include "config.h"

#define STATUS_INITIALIZING     0
#define STATUS_IDLE             1
#define STATUS_MANUAL_WEB       2
#define STATUS_MANUAL_NUNCHUK   3
#define STATUS_AUTO_N           4
#define STATUS_AUTO_N_SEEKING   5
#define STATUS_AUTO_W           6
#define STATUS_AUTO_W_SEEKING   7
#define STATUS_CONFIGURING      8
#define STATUS_LEARNING         9

#define NEO_CMD_SETMODE         7
#define NEO_CMD_SETBACKGROUND   6
#define NEO_CMD_SETFOREGROUND   5
#define NEO_CMD_SET_WIN_CTR     4
#define NEO_CMD_SET_WIN_WIDTH   3

#define NEO_MODE_DISPLAY_OFF    0
#define NEO_MODE_RAINBOW_FULL   1
#define NEO_MODE_RAINBOW_BLUE   2 
#define NEO_MODE_RAINBOW_GRAY   3
#define NEO_MODE_SOLID          4
#define NEO_MODE_FLASHING       5
#define NEO_MODE_WINDOWED       6

#define NEO_COLOR_BLACK         0
#define NEO_COLOR_WHITE         1
#define NEO_COLOR_RED           2
#define NEO_COLOR_YELLOW        3
#define NEO_COLOR_GREEN         4
#define NEO_COLOR_CYAN          5
#define NEO_COLOR_BLUE          6
#define NEO_COLOR_PURPLE        7
#define NEO_COLOR_ORANGE        8
#define NEO_COLOR_GRAY          9

void status_init();
void status_set_menu_msg(String message, int color);
void status_set_info_msg(String message, int color);

void status_neo_send(int cmd, int param);
void status_neo_show_steering_info(int cmd_joyX);
void status_neo_show_steering_info(int cmd_joyX, bool forcedisplay);
void status_neo_show_movement_info(int cmd_joyY, int cmd_joyX);
void status_neo_show_movement_info(int cmd_joyY, int cmd_joyX, bool forcedisplay);
void status_neo_show_menu_psn6(int slot, int color);
void status_neo_show_menu_psn5(int slot, int color);

#endif  // STATUS_H
