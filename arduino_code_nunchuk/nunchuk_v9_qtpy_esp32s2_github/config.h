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
  #define DEBUG_PRINTLN(x)  Serial.println(x)
  #define DEBUG_PRINT(x)  Serial.print(x)
#else
  #define DEBUG_PRINTLN(x)
  #define DEBUG_PRINT(x)
#endif

#define DEVICE_NAME "nunchuk1"
//#define TARGET_DEVICE "http://192.168.86.71"    // differential racer
//#define TARGET_DEVICE "http://192.168.86.76"    // racer5
#define TARGET_DEVICE "http://192.168.86.73"    // racer3
#//define TARGET_DEVICE "http://racer2.local"

#define JOY_CENTER 128

#define JOY_X_MAX 225 // 96
#define JOY_X_MIN 30  // -100
#define JOY_Y_MAX 226 // 97 
#define JOY_Y_MIN 39  // -88

#define JOY_DEADBAND 20

/*
 * battery read and scale config
 * to generate scaling constant measure voltage incoming from batt
 * and also display the raw reading from a/d (as a raw integer)
 * divide voltage from voltmeter (ie 3.628) by raw a/d value (ie 1328)
 * to get scaling constant (ie 2.732)
 */
#define PIN_VBAT_CHK 9  // according to adafruit pin map, A2 is GPIO 9 (for QTPy ESP32-S2)
#define VBAT_SCALING_CONSTANT 2.732   // this times raw a/d reading = mV

#define PIN_BUTTON 5    // panel pushbutton attached to QTPy TX pin (GPIO 5)

#define COMMUNIC_MODE_ESPNOW 0
#define COMMUNIC_MODE_HTML 1

#endif /* CONFIG_H */
