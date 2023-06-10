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
 *
 * *****************************************************************
 * OpenMV Autonomous Racers  configuration     
 * *****************************************************************
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

/*
 * ***************************************************
 * public functions for config
 * ***************************************************
 */

void cfg_init(void); 
void cfg_save(void);
String cfg_showfile();

/*
 * ***************************************************
 * public data for config
 * ***************************************************
 */

struct Config {
  char robot_name[10];  
  float batt_E_scale;
  int batt_E_numcells;
  bool batt_E_used;
  bool batt_E_shutdown_on_red;
  float batt_E_min_green;
  float batt_E_min_yellow;
  float batt_E_min_orange;   
  float batt_M_scale;
  int batt_M_numcells;
  bool batt_M_used;
  bool batt_M_shutdown_on_red;
  float batt_M_min_green;
  float batt_M_min_yellow;
  float batt_M_min_orange;   
  int steer_center_us;
  int steer_left_us;
  int steer_right_us;
  float steering_fraction;
  int manual_speed_creep;     
  int manual_speed_normal;
  int manual_speed_turns;
  int manual_turn_threshold;
  int auto_speed_normal;
  int auto_speed_turns;
  int throt_start_pwm;
  int throt_start_duration;
  int accel_limit;
  
  int esc_arm_signal;
  int esc_arm_time;
  int esc_min_throttle;
  int esc_max_throttle;
  int esc_reverse_throttle;  
  int readconfirm;
};

//struct Globals {
//  bool batt_show_raw;
//};

extern Config config;  
//extern Globals globals; 

/*
 * ************************************************************************
 * Fundamental Switch for 2-wheel differential drive vs RS PWM drive
 * (note that all RC flavor systems are build on METRO ESP32-S2 platform
 * ************************************************************************
 */

#define FLAVOR_RC
//#define FLAVOR_DIFFERENTIAL

/*
 * *******************************************************************
 * ******************************************************************* 
 * ******************************************************************* 
 * Hardware PINS and "board usage" configuration
 * the items below are compiled-in constants that are "static" 
 * in the sense that they define hardware pins that are not 
 * typically changed "on the fly" so do not need to be in the user
 * changeable configuration flash
 * *******************************************************************
 * ******************************************************************* 
 * *******************************************************************
 */
 
/*
 * *******************************************************************
 * ******************************************************************* 
 *   RC Chassis configuration - using PWM signals for throt & steer
 * ******************************************************************* 
 * *******************************************************************
 */

#ifdef FLAVOR_RC
  #define PIN_SD_CS   14
  
  #define PIN_TFT_CS       11
  #define PIN_TFT_RST      12 // Or set to -1 and connect to Arduino RESET pin  (can probably do this)
  #define PIN_TFT_DC       13 // (was 36)
  
  #define PIN_MOSI      35
  #define PIN_MISO      37
  #define PIN_SCLK      36

  #define I2C_NEOPIXEL  0x32

  // may set pins to 0 if sercom2 is not used, else to pin numbers
  #define PIN_RX_SERCOM2 15       // sercom2 is used for neopixel driver board
  #define PIN_TX_SERCOM2 16
  #define BAUD_RATE_SERCOM2 9600
  
  // may set pins to 0 if sercom1 is not used, else to pin numbers
  #define PIN_RX_SERCOM1 9
  #define PIN_TX_SERCOM1 10
  #define BAUD_RATE_SERCOM1 57600

  // note A0 and A1 are on ADC2 and cannot be used when wifi/bluetooth is used
  // A2-A5 are on ADC1 and can be freely used
  // on Adafruit Metro ESP32-S2  A0=io17, A1=io18, A2=io1, A3=io2, A4=io3, A5=io4
  #define PIN_VBAT_E_CHK 1   // A2 (note this is on ADC1, so it can be used freely
  #define PIN_VBAT_M_CHK 2   // A3 (note this is on ADC1, so it can be used freely
  #define PIN_SERVO_STEER 5  // 42
  #define PIN_SERVO_THROT 21  // 21
   
  #define SERVO_PCA9685
  //#define SERVO_NATIVE_ESP32S2 
  
  #define SERVO_MIN_MICROS      500  // was 800
  #define SERVO_MAX_MICROS      2700 // was 2450
#endif  // FLAVOR_RC

 
/*
 * *******************************************************************
 * ******************************************************************* 
 *   Differential steering configuration; 2 DC motors to drive 
 * ******************************************************************* 
 * *******************************************************************
 */
 
#ifdef FLAVOR_DIFFERENTIAL
  #define PIN_SD_CS   21
  #define PIN_CAM_CS  5
  
  #define PIN_TFT_CS       14
  #define PIN_TFT_RST      4 
  #define PIN_TFT_DC       22 
  
  #define PIN_MOSI      23
  #define PIN_MISO      19
  #define PIN_SCLK      18
  
  #define PIN_MOTOR_PWMA 15  // right motor
  #define PIN_MOTOR_AIN2 33
  #define PIN_MOTOR_AIN1 25
  
  #define PIN_MOTOR_PWMB 2 // left motor
  #define PIN_MOTOR_BIN1 26
  #define PIN_MOTOR_BIN2 27
  
  #define PIN_VBAT_E_CHK 34
  
  // may set pins to 0 if sercom2 is not used, else to pin numbers
  #define PIN_RX_SERCOM2 16       // sercom2 is used for neopixel driver board
  #define PIN_TX_SERCOM2 17
  #define BAUD_RATE_SERCOM2 9600
  
  // may set pins to 0 if sercom1 is not used, else to pin numbers
  #define PIN_RX_SERCOM1 35       
  #define PIN_TX_SERCOM1 32
  #define BAUD_RATE_SERCOM1 9600  
#endif  // FLAVOR_DIFFERENTIAL


#endif /* CONFIG_H */
