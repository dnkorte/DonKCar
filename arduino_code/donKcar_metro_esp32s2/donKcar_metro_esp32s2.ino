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
#include "config.h"
#include "util.h";
#include "screen.h"
#include "status.h"
#include "wifi.h"
#include "webap_core.h"
#include "i2c_com.h"
#include "mode_mgr.h"
#include "drivetrain.h"
#include "nunchuk.h"
#include "serial_com_esp32.h"

/* 
 *  Note later setup motors.h and screen.h as standard arduino libraries so that
 *     they are independent of this program (could be used by other versions of it)
 *  Note later setup communic.h and dispatcher.h as independent libraries so that
 *      they could be replaced by modules relevant to BLE
 *      (bleuart is bidirectional;  https://learn.adafruit.com/circuitpython-ble-libraries-on-any-computer/ble-uart-example)
 *  
 *  note for PS4-esp32 library, download zip from https://github.com/aed3/PS4-esp32
 */

long nextBattDispDue_E, nextBattDispDue_M;
long nextHeartbeatCheckDue, nextMenuCheckDue;
long nextCam1QueryDue;

long nextTestDue;
int testangle, testFlavor;

int  last_joyXR, last_joyYR, last_joyXL, last_joy_YL;
float batt_volts_E, cell_volts_E;   // batt for electronics
float batt_volts_M, cell_volts_M;   // batt for motors
float batt_volts_C;
int   num_red_E, num_red_M;         // if 3 checks in a row are red then we go to MODE_ERROR_BATT 

void setup() {
  // note it takes a while for Serial to start; this empty loop waits for it
  // ref   https://forum.arduino.cc/t/cant-view-serial-print-from-setup/167916
  #ifdef DEBUG
    Serial.begin(115200); 
    while (!Serial) ;
  #endif
  
  pinMode(PIN_TFT_CS, OUTPUT);    
  pinMode(PIN_SD_CS, OUTPUT);

  // give other boards (neopixel controller, etc) time to wake up before we start talking to them
  delay(500);    

  cfg_init();
  i2c_init();       // initialize communication to neopixel
  screen_init(); 
  mode_init();    // note this also performs status_init
 
  screen_centerText(ROW_RACERNAME, config.robot_name, COLOR_CYAN ); 
  screen_writeText_colrow(COL_LEFTEDGE, ROW_BATT_E, WIDTH_FULL, "Bat E:", COLOR_HEADER);
  screen_writeText_colrow(COL_LEFTEDGE, ROW_BATT_M, WIDTH_FULL, "Bat M:", COLOR_HEADER);
    
  wifi_init();
  drivetrain_init();    // this also initializes motors  
  
  batt_volts_E = 0.0;
  batt_volts_M = 0.0;
  cell_volts_E = 0.0;
  cell_volts_M = 0.0;
  num_red_E = 0;
  num_red_M = 0;
  
  nextBattDispDue_E = millis() + 50;
  nextBattDispDue_M = millis() + 3050;
  nextCam1QueryDue = millis() + 315;
  nextHeartbeatCheckDue = 0;
  nextMenuCheckDue = 0;
  
  nextTestDue = millis() + 230;
  testFlavor = 0;   // currently no camera polls
  sercom1_init();
  
  analogReadResolution(12);
  mode_set_mode(MODE_IDLE); 
}

void loop() {
  char tmpBuf[36];
  int  battColor;
  int  vbat_raw;
  long current_time;
  
  current_time = millis();

  /*
   * handle requests generated in nunchuk module
   * these cannot be handled in the nunchuk module which is
   * called in the wifi callback for ESPNOW because the callback
   * is an interrupt handler and some events (principally those
   * that generate PWM) get confused when called from an ISR
   * (note flagXX vars are extern globals declared in nunchuk module)
   */
  if (flagC) {
    mode_c_button_event(cmdC);
    flagC = false;
  }
  if (flagZ) {
    mode_z_button_event(cmdZ);
    flagZ = false;
  }
  if (flagX) {
    mode_joyX_event(cmdX);
    flagX = false;
  }
  if (flagY) {
    mode_joyY_event(cmdY);
    flagY = false;
  }

  if (webap_getWebMode()) {
    webap_process();
  }

  if (webap_getWebEndRequest()) {
    // note this delay  is ok because robot isn't doing anything else when it (was) in web mode
    delay(100);         
    webap_deinit(0);    // terminated due to user request
  }
 
  /* 
   * read battery voltage.  note that battery is divided by resistive 
   * divider with 10k and 1k, such that nominal 7.4v (LiPo 2S) should
   * yield approx 0.67v at pin A2 (BATT_E) or A3 (BATT_M)
   * https://docs.espressif.com/projects/esp-idf/en/v4.4.1/esp32s2/api-reference/peripherals/adc.html
   * https://docs.espressif.com/projects/esp-idf/en/v4.4.1/esp32s2/api-reference/peripherals/adc.html#_CPPv414adc1_channel_t
   * attenuation ADC_ATTEN_DB_0 allows to read 0 - 750 mV
   * Pin A2 (GPIO1) is on ADC1 which is the one that is ok to use even if using WiFi
   * Pin A3 (GPIO2) is on ADC1 which is the one that is ok to use even if using WiFi
   */
 
  if ((current_time > nextBattDispDue_E) && config.batt_E_used) {
    nextBattDispDue_E = current_time + 60000;

    vbat_raw = readAnalog5xAveraged(PIN_VBAT_E_CHK);
    batt_volts_E = (config.batt_E_scale * vbat_raw) / 1000.0;  
    cell_volts_E = batt_volts_E / config.batt_E_numcells;
    
    if (cell_volts_E > config.batt_E_min_green) {
      battColor = COLOR_GREEN;
    } else if (cell_volts_E > config.batt_E_min_yellow) {
      battColor = COLOR_YELLOW; 
    } else if (cell_volts_E > config.batt_E_min_orange) {
      battColor = COLOR_ORANGE;  
    } else {
      battColor = COLOR_RED;
    }     
    
    dtostrf(batt_volts_E, 5, 2, tmpBuf);
    screen_writeText_colrow(COL_DATA, ROW_BATT_E, 5, tmpBuf, battColor);
    screen_writeText_colrow(COL_DATA+5, ROW_BATT_E, 1, "v", COLOR_FOREGROUND); 
     
    //if (globals.batt_show_raw) {
    if (0==1) {
      itoa(vbat_raw, tmpBuf,10);
      screen_writeText_colrow(COL_DATA + 7, ROW_BATT_E, 6, tmpBuf, COLOR_CYAN);
      screen_writeText_colrow(COL_DATA + 12, ROW_BATT_E, 3, "raw", COLOR_CYAN);
    } else {
      dtostrf(cell_volts_E, 5, 2, tmpBuf);
      screen_writeText_colrow(COL_DATA+7, ROW_BATT_E, 1, "(", COLOR_FOREGROUND);
      screen_writeText_colrow(COL_DATA+8, ROW_BATT_E, 5, tmpBuf, battColor);
      screen_writeText_colrow(COL_DATA+14, ROW_BATT_E, 2, ")", COLOR_FOREGROUND);    
    }      
    screen_show();
    
    if ((battColor == COLOR_RED) && config.batt_E_shutdown_on_red) {
      num_red_E++;
      if (num_red_E > 3) {
        mode_set_mode(MODE_ERROR_BATT);
      }
    } else {
      num_red_E = 0;
    }
  }

  if ((current_time > nextBattDispDue_M) && config.batt_M_used) {
    nextBattDispDue_M = current_time + 60000;

    //vbat_raw = analogRead(PIN_VBAT_M_CHK);
    vbat_raw = readAnalog5xAveraged(PIN_VBAT_M_CHK);
    batt_volts_M = (config.batt_M_scale * vbat_raw) / 1000.0;  
    cell_volts_M = batt_volts_M / config.batt_M_numcells;
    
    if (cell_volts_M > config.batt_M_min_green) {
      battColor = COLOR_GREEN;
    } else if (cell_volts_M > config.batt_M_min_yellow) {
      battColor = COLOR_YELLOW; 
    } else if (cell_volts_M > config.batt_M_min_orange) {
      battColor = COLOR_ORANGE;  
    } else {
      battColor = COLOR_RED;
    }     
    
    dtostrf(batt_volts_M, 5, 2, tmpBuf);
    screen_writeText_colrow(COL_DATA, ROW_BATT_M, 5, tmpBuf, battColor);
    screen_writeText_colrow(COL_DATA+5, ROW_BATT_M, 1, "v", COLOR_FOREGROUND); 
     
    //if (globals.batt_show_raw) {
    if (0 == 1) {
      itoa(vbat_raw, tmpBuf,10);
      screen_writeText_colrow(COL_DATA + 7, ROW_BATT_M, 6, tmpBuf, COLOR_CYAN);
      screen_writeText_colrow(COL_DATA + 12, ROW_BATT_M, 3, "raw", COLOR_CYAN);
    } else {
      dtostrf(cell_volts_M, 5, 2, tmpBuf);
      screen_writeText_colrow(COL_DATA+7, ROW_BATT_M, 1, "(", COLOR_FOREGROUND);
      screen_writeText_colrow(COL_DATA+8, ROW_BATT_M, 5, tmpBuf, battColor);
      screen_writeText_colrow(COL_DATA+14, ROW_BATT_M, 2, ")", COLOR_FOREGROUND);    
    }      
    screen_show();
    
    if ((battColor == COLOR_RED) && config.batt_M_shutdown_on_red) {
      num_red_M++;
      if (num_red_M > 3) {
        mode_set_mode(MODE_ERROR_BATT);
      }
    } else {
      num_red_M = 0;
    }
  }
  
  if (current_time > nextHeartbeatCheckDue) {
    nextHeartbeatCheckDue = current_time + 500;
    mode_check_heartbeat();   // check for heartbeat timeouts
    mode_check_webap_heartbeat();
  }

  if (current_time > nextMenuCheckDue) {
    nextMenuCheckDue = current_time + 1000;
    mode_check_menu_timeout();   // check for menu timeouts
  }

  if (current_time > nextCam1QueryDue)  {
    if (testFlavor == 1) {
      for (int ii=0; ii<10; ii++) {
        sercom1_sendchar('c');
      }
    }
    nextCam1QueryDue = millis() + 250;
  }
  
  if (current_time > nextTestDue) {
    if (testFlavor == 0) {
      testFlavor = 1;
      screen_centerText(ROW_STAT3, "Sending Cam Test", COLOR_GREEN );
    } else {
      testFlavor = 0;
      screen_centerText(ROW_STAT3, "Not Sending Cam", COLOR_YELLOW );
    }
    nextTestDue = millis() + 10000;
  }

  // FOR TESTING   TODO **********************************************************
  //if (current_time > nextTestDue) {
  //  servo_set_steering_angle(testangle);
  //  //DEBUG_PRINT("Servo:");
  //  //DEBUG_PRINTLN(testangle);
  //  testangle += 1;
  //  if (testangle > 255) {
  //    testangle = -255;
  //  }
  //  nextTestDue = current_time + 20;
  //}


  //delay(5);
}
