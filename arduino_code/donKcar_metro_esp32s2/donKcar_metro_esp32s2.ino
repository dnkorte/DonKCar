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
#include "status.h"
#include "wifi.h"
#include "webap_core.h"
#include "i2c_com.h"
#include "mode_mgr.h"
#include "drivetrain.h"
#include "nunchuk.h"
#include "battery.h"
#include "serial_com_esp32.h"

long nextBattDispDue_E, nextBattDispDue_M;
long nextBattSendDue_E, nextBattSendDue_M;
long nextHeartbeatCheckDue, nextMenuCheckDue;
long nextCam1QueryDue;

long nextTestDue;
int testangle, testFlavor;

int  last_joyXR, last_joyYR, last_joyXL, last_joy_YL;

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
  status_init();    // this initializes screen and neopixel interfaces
  mode_init();      // note this also performs status_init
  batt_init();
 
  status_disp_mainpage_skeleton();
    
  wifi_init();
  drivetrain_init();    // this also initializes motors  
    
  nextBattDispDue_E = millis() + 50;
  nextBattDispDue_M = millis() + 2050;
  nextBattSendDue_E = millis() + 70;
  nextBattSendDue_M = millis() + 2070;
  nextCam1QueryDue = millis() + 315;
  nextHeartbeatCheckDue = 0;
  nextMenuCheckDue = 0;
  
  nextTestDue = millis() + 230;
  testFlavor = 0;   // currently no camera polls
  sercom1_init();
  
  mode_set_mode(MODE_IDLE); 
}

void loop() {
  char tmpBuf[36];
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
 
  if (current_time > nextBattDispDue_E) {
    nextBattDispDue_E = current_time + 60000;
    if (batt_read('E')) {
      mode_set_mode(MODE_ERROR_BATT);
    }
    batt_display('E');
  }
  
  if (current_time > nextBattDispDue_M) {
    nextBattDispDue_M = current_time + 60000;
    if (batt_read('M')) {
      mode_set_mode(MODE_ERROR_BATT);
    }
    batt_display('M');
  }
 
  if (current_time > nextBattSendDue_E) {
    nextBattSendDue_E = current_time + 60000;
    batt_send('E');
  }
 
  if (current_time > nextBattSendDue_M) {
    nextBattSendDue_M = current_time + 60000;
    batt_send('M');
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
      status_disp_simple_msg("Sending Cam Test", 'G');
    } else {
      testFlavor = 0;
      status_disp_simple_msg("Not Sending Cam", 'Y');
    }
    nextTestDue = millis() + 10000;
  }
  
  //delay(5);
}
