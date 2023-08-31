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
 
#ifndef CONFIG_H
#define CONFIG_H

/*
 * *****************************************************************
 * macro to support debugging
 * *****************************************************************
 */

#ifdef DEBUG
  #define DEBUG_PRINTLN(x) Serial.println(x)
  #define DEBUG_PRINT(x)  Serial.print(x)
#else
  #define DEBUG_PRINTLN(x)
  #define DEBUG_PRINT(x)
#endif

#define WANT_TFT    /* use TFT screen instead of OLED */

#define DEVICE_NAME "controller2"

#define JOY_CENTER 128

#define JOY_X_MAX 225 // 96
#define JOY_X_MIN 30  // -100
#define JOY_Y_MAX 226 // 97 
#define JOY_Y_MIN 39  // -88

#define JOY_DEADBAND 20

#define PIN_BUTTON_0 0  
#define PIN_BUTTON_1 1 
#define PIN_BUTTON_2 2   

// comment out whichever one is not used...
#define CONTROLLER_TYPE_NUNCHUK
//#define CONTROLLER_TYPE_WIICLASSIC

#endif /* CONFIG_H */
