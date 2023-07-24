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
 *
 * **********************************************************************************
 * **********************************************************************************
 * this project utilizes the Nintendo Extension Controller Library by David Madison
 *  @link       github.com/dmadison/NintendoExtensionCtrl
 *  @license    LGPLv3 - Copyright (c) 2018 David Madison
 *  https://github.com/dmadison/NintendoExtensionCtrl  
 *  
 *  Note this works with either the literal Woo "Nunchuk" device or the Wii Classic Controller
 *  Nunchuk Controller: https://www.amazon.com/Nunchuck-Controller-Joystick-Gamepad-Console/dp/B07RLBN9Z2
 *  Nunchuk COntroller: https://www.amazon.com/Nunchuck-Controller-Joystick-Gamepad-Console/dp/B083TRZZK7/
 *  Classic Controller: https://www.amazon.com/JYELUK-Classic-Controller-Compatible-White/dp/B0BZV1RR6Z
 *  Classic Controller: https://www.amazon.com/Zettaguard-Controller-classic-Console-Gampad-Nintendo/dp/B00XH1FV3
 *  Connector Breakout: https://www.adafruit.com/product/4836
 *
 * **********************************************************************************
 * **********************************************************************************            
*/

#include "config.h"

#include <NintendoExtensionCtrl.h>
#include "communic.h"
#include "screen_tft.h"
#include "status.h"
#include "battery.h"
#include <Wire.h>

#define HEARTBEAT_INTERVAL 1000

String jsonBuffer;
int lastJoyY, lastJoyX;
bool lastButtonC, lastButtonZ;
int button_state, lastButton_0_State, lastButton_1_State, lastButton_2_State;
bool nunchukConnected;
long current_time;
long lastHeartbeatMsg;  
long nextBattDispDue;
long nextHeartbeat;
long nextStatusMessageClearCheck;

Nunchuk nchuk;

/*
 * private function declarations
 */

/* 
 * Setup
 */

void setup() {

  status_neopixels_init();

  /*
   * note we're goint to flash the on-board neopixel a few times
   * here to give progress indication of startup.  but there's really
   * a dual purpose -- it takes OLED and/or Nunchuk board a little while
   * to wake up after powerup, and the delays in these display functions
   * give them time to do that.  if left out, then substitute a delay 
   * early in startup routine to give equivalent time
   */
  status_neopixel_flash(0xFFFFFF);

  // note it takes a while for Serial to start; this empty loop waits for it
  // ref   https://forum.arduino.cc/t/cant-view-serial-print-from-setup/167916
  #ifdef DEBUG
    Serial.begin(115200); 
    while (!Serial) ;
  #endif

  status_init(); 
  screen_centerText(ROW_M_STAT2, "Screen Initialized", ST77XX_BLUE);
  screen_show();  
  status_neopixel_flash(0x0000FF);  // BLUE indicates past screen
  
	nchuk.begin();
  nunchukConnected = false;
	while (!nchuk.connect()) {
    // try to reconnect, but due to lack of i2c hot-plugging capability
    // it will probably need a physical reboot
    status_neopixel_flash(0xc41a00);  // ORANGE indicates error nunchuk
    screen_centerText(ROW_M_STAT1, "Please Plug in", ST77XX_RED);
    screen_centerText(ROW_M_STAT2, "the Nunchuk device", ST77XX_RED);
    screen_centerText(ROW_M_STAT3, "then reboot", ST77XX_RED);
    screen_show();
		delay(1000);
	} 
  nunchukConnected = true;
	status_clear_status_area();   
  screen_centerText(ROW_M_STAT2, "Nunchuk Initialized", ST77XX_GREEN);
  screen_show();
  status_neopixel_flash(0x00FF00);  // GREEN indicates past nunchuk

  lastJoyY = 0;
  lastJoyX = 0;
  lastButtonC = false;
  lastButtonZ = false;
  lastButton_0_State = 1;
  lastButton_1_State = 1;
  lastButton_2_State = 1;
    
  communic_init();  
  screen_centerText(ROW_M_STAT2, "Radio Initialized", ST77XX_YELLOW);
  screen_show(); 
  status_neopixel_flash(0xFFFF00);  // YELLLOW indicates past communic

  pinMode(PIN_BUTTON_0, INPUT_PULLUP);
  pinMode(PIN_BUTTON_1, INPUT_PULLUP);
  pinMode(PIN_BUTTON_2, INPUT_PULLUP);
  
  nextHeartbeat = millis() + HEARTBEAT_INTERVAL;
  nextBattDispDue = millis() + 20;
  nextStatusMessageClearCheck = millis() + 263;
  
  batt_init();
  status_main_structure();    
  screen_centerText(ROW_M_STAT2, "Ready for Use", ST77XX_WHITE);
  screen_centerString(ROW_M_STAT3, communic_connected_device(), ST77XX_WHITE);
  screen_show(); 
}

void loop() {
  int rawJoyX, rawJoyY, centeredJoyX, centeredJoyY, scaledJoyX, scaledJoyY;
	boolean success;
  char tmpBuf[12];

  current_time = millis();
  success = nchuk.update();  // Get new data from the controller
  
	if (!success) {  // Ruh roh
    // try to reconnect, but due to lack of i2c hot-plugging capability
    // it will probably need a physical reboot
    screen_centerText(ROW_M_STAT1, "Please Plug in", ST77XX_RED);
    screen_centerText(ROW_M_STAT2, "the Nunchuk device", ST77XX_RED);
    screen_centerText(ROW_M_STAT3, "then reboot", ST77XX_RED);
    screen_show();    
    nunchukConnected = false;
		delay(1000);
    nchuk.connect();
	}
	else {
    if (!nunchukConnected) {      
      nunchukConnected = true;
      status_clear_status_area();
    }
    
		// Read a button (on/off, C and Z)
		boolean zButton = nchuk.buttonZ();
    boolean cButton = nchuk.buttonC();
    rawJoyX = nchuk.joyX();
    rawJoyY = nchuk.joyY();

    centeredJoyX = rawJoyX;
    centeredJoyY = rawJoyY;

    // then centralize both joysticks to zero center
    centeredJoyX = centeredJoyX - JOY_CENTER;
    centeredJoyY = centeredJoyY - JOY_CENTER;

    // finally scale joystick values to represent -255 to +255
    scaledJoyX = map(centeredJoyX, JOY_X_MIN - JOY_CENTER, JOY_X_MAX - JOY_CENTER, -255, 255);
    scaledJoyY = map(centeredJoyY, JOY_Y_MIN - JOY_CENTER, JOY_Y_MAX - JOY_CENTER, -255, 255);

    if (scaledJoyX > 255) {
      scaledJoyX = 255;
    }
    if (scaledJoyX < -255) {
      scaledJoyX = -255;
    }
    
    if (scaledJoyY > 255) {
      scaledJoyY = 255;
    }
    if (scaledJoyY < -255) {
      scaledJoyY = -255;
    }
    
    // make a deadband around CENTER for both joysticks
    if ((scaledJoyX < (JOY_DEADBAND)) && (scaledJoyX > (-JOY_DEADBAND))) {
      scaledJoyX = 0;
    }
    if ((scaledJoyY < (JOY_DEADBAND)) && (scaledJoyY > (-JOY_DEADBAND))) {
      scaledJoyY = 0;
    }
    
    if (zButton != lastButtonZ) {
      if (zButton) {
        communic_send_message('Z', 1);
        //screen_writeText_colrow(COL_M_NUN_BTNS_LAB+0, ROW_M_NUNCHUK_INFO, 1, "Z", COLOR_GREEN);
      } else {
        //communic_send_message("zbutn=0");  
        communic_send_message('Z', 0);
        //screen_writeText_colrow(COL_M_NUN_BTNS_LAB+0, ROW_M_NUNCHUK_INFO, 1, " ", COLOR_FOREGROUND);      
      }
      //screen_show();
    }
   
    if (cButton != lastButtonC) {
      if (cButton) {
        communic_send_message('C', 1);
        //screen_writeText_colrow(COL_M_NUN_BTNS_LAB+1, ROW_M_NUNCHUK_INFO, 1, "C", COLOR_RED);
      } else {
        //communic_send_message("cbutn=0");
        communic_send_message('C', 0);
        //screen_writeText_colrow(COL_M_NUN_BTNS_LAB+1, ROW_M_NUNCHUK_INFO, 1, " ", COLOR_FOREGROUND);        
      }
      //screen_show();
    }
   
    if (abs(scaledJoyX - lastJoyX) > 1) { 
      communic_send_message('X', scaledJoyX);
      screen_writeText_colrow(COL_M_NUN_X_LAB+2, ROW_M_NUNCHUK_INFO, 4, itoa(scaledJoyX, tmpBuf, 10), COLOR_FOREGROUND);
      screen_show();
    }
   
    if (abs(scaledJoyY - lastJoyY) > 1) {; 
      communic_send_message('Y', scaledJoyY);
      screen_writeText_colrow(COL_M_NUN_Y_LAB+2, ROW_M_NUNCHUK_INFO, 4, itoa(scaledJoyY, tmpBuf, 10), COLOR_FOREGROUND);
      screen_show();
    }
     
    lastJoyY = scaledJoyY;
    lastJoyX = scaledJoyX;
    lastButtonC = cButton;
    lastButtonZ = zButton;
	}
 
  button_state = digitalRead(PIN_BUTTON_0);
  if (button_state != lastButton_0_State) {
    if (button_state == 0) {  // pressed      
      //screen_writeText_colrow(COL_M_NUN_BTNS_LAB+2, ROW_M_NUNCHUK_INFO, 1, "0", COLOR_ORANGE);;
      //screen_show();
    } else {    
      //screen_writeText_colrow(COL_M_NUN_BTNS_LAB+2, ROW_M_NUNCHUK_INFO, 1, " ", COLOR_FOREGROUND);
      //screen_show();
    }
    lastButton_0_State = button_state;
  }
 
  button_state = digitalRead(PIN_BUTTON_1);
  if (button_state != lastButton_1_State) {
    if (button_state == 0) {  // pressed      
      //screen_writeText_colrow(COL_M_NUN_BTNS_LAB+3, ROW_M_NUNCHUK_INFO, 1, "1", COLOR_ORANGE);;
      //screen_show();
    } else {    
      //screen_writeText_colrow(COL_M_NUN_BTNS_LAB+3, ROW_M_NUNCHUK_INFO, 1, " ", COLOR_FOREGROUND);
      //screen_show();
    }
    lastButton_1_State = button_state;
  }
 
  button_state = digitalRead(PIN_BUTTON_2);
  if (button_state != lastButton_2_State) {
    if (button_state == 0) {  // pressed      
      //screen_writeText_colrow(COL_M_NUN_BTNS_LAB+4, ROW_M_NUNCHUK_INFO, 1, "2", COLOR_ORANGE);;
      //screen_show();
    } else {    
      //screen_writeText_colrow(COL_M_NUN_BTNS_LAB+4, ROW_M_NUNCHUK_INFO, 1, " ", COLOR_FOREGROUND);
      //screen_show();
    }
    lastButton_2_State = button_state;
  }

  
  if (current_time > nextHeartbeat) {
    communic_send_message('H', 1);
    nextHeartbeat = millis() + HEARTBEAT_INTERVAL;
  }
  
  if (current_time > nextBattDispDue) {
    nextBattDispDue = current_time + 60000;
    display_my_mybatt();
  }
  
  if (batt_has_new_data_ready('E')) {
    display_remote_mybatts();
  }
  if (batt_has_new_data_ready('M')) {
    display_remote_mybatts();
  }

  // display any new status messages or menu lines that came in over espnow
  status_display_mainscreen_messages();
  

  /* 
   *  implement "clear status message" process after appropriate timeouts
   *  (note messages time out after 30 seconds)
   */
  if (current_time > nextStatusMessageClearCheck) {
    nextStatusMessageClearCheck = current_time + 100;
    status_message_area_clear_check();   // check for menu timeouts
  }

  delay(50);
}
