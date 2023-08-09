/*
 * Summary: openMV + esp32 based autonomous racer
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
#ifndef CAM_H
#define CAM_H

#include <Arduino.h>
#include "config.h"

#define CAM_CMD_MODE_IDLE 1
#define CAM_CMD_MODE_BLOBS 2
#define CAM_CMD_MODE_REGRESSION_LINE 3
#define CAM_CMD_SEND_PIC 4
#define CAM_CMD_MODE_GRAYSCALE 5
#define CAM_CMD_DRIVE_ON 6
#define CAM_CMD_DRIVE_OFF 7
#define CAM_CMD_BLOB_SET_SEED_LOC 8
#define CAM_CMD_BLOB_SET_FLOATING_THRESH 9
#define CAM_CMD_BLOB_SET_SEED_THRESH 10
#define CAM_CMD_BLOB_ROI_WEIGHT_T 11
#define CAM_CMD_BLOB_ROI_WEIGHT_M 12
#define CAM_CMD_BLOB_ROI_WEIGHT_B 13
#define CAM_CMD_BLOB_ROI_POSITION_T 14
#define CAM_CMD_BLOB_ROI_POSITION_M 15
#define CAM_CMD_BLOB_ROI_POSITION_B 16
#define CAM_CMD_PID_SET_KP 17
#define CAM_CMD_PID_SET_KI 18
#define CAM_CMD_PID_SET_KD 19
#define CAM_CMD_PID_SET_STEERING_GAIN 20      // deprecated
#define CAM_CMD_PID_SET_STEERING_DIRECTION 21 // deprecated
#define CAM_CMD_CAM_SET_PERSPECTIVE_FACTOR 22
#define CAM_CMD_CAM_PERSPECTIVE_ON 23
#define CAM_CMD_CAM_PERSPECTIVE_OFF 24
#define CAM_CMD_MODE_LANE_LINES 25
#define CAM_CMD_BLOB_SET_LUMI_LOW 26
#define CAM_CMD_BLOB_SET_LUMI_HIGH 27
#define CAM_CMD_HISTEQ_WANTED 28
#define CAM_CMD_NEGATE_WANTED 29

void cam_init(void);
void cam_loop(void);
void cam_send_cmd(uint8_t cmd);
void cam_send_cmd(uint8_t cmd, int param1);
void cam_send_cmd(uint8_t cmd, int param1, int param2);
void cam_send_cmd(uint8_t cmd, float param1);
void cam_timeout_check(void);
void cam_preset_paremeters();
void cam_enter_preferred_mode();

bool cam_append_pic(String &pagebuf);
void cam_request_pic();
char* cam_return_pic();
bool cam_check_cam_image_readiness();

#endif  /* CAM_H */
