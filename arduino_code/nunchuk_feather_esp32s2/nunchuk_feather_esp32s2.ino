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
#include "battery.h"

#include <Adafruit_NeoPixel.h>
#define NUMPIXELS 1
Adafruit_NeoPixel pixels(NUMPIXELS, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);

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

Nunchuk nchuk;

/*
 * private function declarations
 */
void neopixel_flash(int color);
void page_clear_status_area();

/* 
 * Setup
 */

void setup() {

  // turn on the Neopixel power supply
  // https://learn.adafruit.com/esp32-s2-reverse-tft-feather/pinouts
  pinMode(NEOPIXEL_POWER, OUTPUT);
  digitalWrite(NEOPIXEL_POWER, HIGH);
  delay(10);
  pixels.begin();
  pixels.setBrightness(20);

  /*
   * note we're goint to flash the on-board neopixel a few times
   * here to give progress indication of startup.  but there's really
   * a dual purpose -- it takes OLED and/or Nunchuk board a little while
   * to wake up after powerup, and the delays in these display functions
   * give them time to do that.  if left out, then substitute a delay 
   * early in startup routine to give equivalent time
   */
  neopixel_flash(0xFFFFFF);

  // note it takes a while for Serial to start; this empty loop waits for it
  // ref   https://forum.arduino.cc/t/cant-view-serial-print-from-setup/167916
  #ifdef DEBUG
    Serial.begin(115200); 
    while (!Serial) ;
  #endif
  
  screen_init();  
  screen_centerText(ROW_M_STAT1, "Screen Initialized", ST77XX_BLUE);
  screen_show();  
  neopixel_flash(0xFF0000);  // RED indicates past serial
  
	nchuk.begin();
  DEBUG_PRINTLN("nunchuk initizialized");

  nunchukConnected = false;
	while (!nchuk.connect()) {
		DEBUG_PRINTLN("Nunchuk not detected!");
    pixels.fill(0xc41a00);  // ORANGE indicates error nunchuk
    pixels.show();   
    screen_centerText(ROW_M_STAT1, "Please Plug in", ST77XX_RED);
    screen_centerText(ROW_M_STAT2, "the Nunchuk Device", ST77XX_RED);
    screen_show();
		delay(1000);
	} 
  nunchukConnected = true;
	page_clear_status_area();   
  screen_centerText(ROW_M_STAT1, "Nunchuk Initialized", ST77XX_GREEN);
  screen_show();
  neopixel_flash(0x00FF00);  // GREEN indicates past nunchuk

  lastJoyY = 0;
  lastJoyX = 0;
  lastButtonC = false;
  lastButtonZ = false;
  lastButton_0_State = 1;
  lastButton_1_State = 1;
  lastButton_2_State = 1;
    
  communic_init();  
  screen_centerText(ROW_M_STAT1, "Radio Initialized", ST77XX_YELLOW);
  screen_show(); 
  neopixel_flash(0xFFFF00);  // YELLLOW indicates past communic

  pinMode(PIN_BUTTON_0, INPUT_PULLUP);
  pinMode(PIN_BUTTON_1, INPUT_PULLUP);
  pinMode(PIN_BUTTON_2, INPUT_PULLUP);
  
  nextHeartbeat = millis() + HEARTBEAT_INTERVAL;
  nextBattDispDue = millis() + 2000;
  
  batt_init();
  page_main_structure();    
  screen_centerText(ROW_M_STAT1, "Ready for Use", ST77XX_WHITE);
  screen_show(); 
}

void loop() {
  int rawJoyX, rawJoyY, centeredJoyX, centeredJoyY, scaledJoyX, scaledJoyY;
	boolean success;
  char tmpBuf[12];

  current_time = millis();
  success = nchuk.update();  // Get new data from the controller
  
	if (!success) {  // Ruh roh
		DEBUG_PRINTLN("Controller disconnected!"); 
    screen_centerText(ROW_M_STAT1, "Please Plug in", ST77XX_RED);
    screen_centerText(ROW_M_STAT2, "the Nunchuk Device", ST77XX_RED);
    screen_show();    
    nunchukConnected = false;
		delay(1000);
    nchuk.connect();
	}
	else {
    if (!nunchukConnected) {      
      nunchukConnected = true;
      page_clear_status_area();
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
    
		// Print all the values!
		//nchuk.printDebug();
   
    if (zButton != lastButtonZ) {
      if (zButton) {
        //communic_send_message("zbutn=1");
        communic_send_message('Z', 1, MSG_NULL_FLOAT);
        screen_writeText_colrow(COL_M_NUN_BTNS_LAB+0, ROW_M_NUNCHUK_INFO, 1, "Z", COLOR_GREEN);
      } else {
        //communic_send_message("zbutn=0");  
        communic_send_message('Z', 0, MSG_NULL_FLOAT);
        screen_writeText_colrow(COL_M_NUN_BTNS_LAB+0, ROW_M_NUNCHUK_INFO, 1, " ", COLOR_FOREGROUND);      
      }
      screen_show();
    }
   
    if (cButton != lastButtonC) {
      if (cButton) {
        //communic_send_message("cbutn=1");
        communic_send_message('C', 1, MSG_NULL_FLOAT);
        screen_writeText_colrow(COL_M_NUN_BTNS_LAB+1, ROW_M_NUNCHUK_INFO, 1, "C", COLOR_RED);
      } else {
        //communic_send_message("cbutn=0");
        communic_send_message('C', 0, MSG_NULL_FLOAT);
        screen_writeText_colrow(COL_M_NUN_BTNS_LAB+1, ROW_M_NUNCHUK_INFO, 1, " ", COLOR_FOREGROUND);        
      }
      screen_show();
    }
   
    if (abs(scaledJoyX - lastJoyX) > 1) {
      //communic_send_message("joyX="+String(scaledJoyX)); 
      communic_send_message('X', scaledJoyX, MSG_NULL_FLOAT);
      screen_writeText_colrow(COL_M_NUN_X_LAB+2, ROW_M_NUNCHUK_INFO, 4, itoa(scaledJoyX, tmpBuf, 10), COLOR_FOREGROUND);
      screen_show();
    }
   
    if (abs(scaledJoyY - lastJoyY) > 1) {
      //communic_send_message("joyY="+String(scaledJoyY)); 
      communic_send_message('Y', scaledJoyY, MSG_NULL_FLOAT);
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
      screen_writeText_colrow(COL_M_NUN_BTNS_LAB+2, ROW_M_NUNCHUK_INFO, 1, "0", COLOR_ORANGE);;
      screen_show();
    } else {    
      screen_writeText_colrow(COL_M_NUN_BTNS_LAB+2, ROW_M_NUNCHUK_INFO, 1, " ", COLOR_FOREGROUND);
      screen_show();
    }
    lastButton_0_State = button_state;
  }
 
  button_state = digitalRead(PIN_BUTTON_1);
  if (button_state != lastButton_1_State) {
    if (button_state == 0) {  // pressed      
      screen_writeText_colrow(COL_M_NUN_BTNS_LAB+3, ROW_M_NUNCHUK_INFO, 1, "1", COLOR_ORANGE);;
      screen_show();
    } else {    
      screen_writeText_colrow(COL_M_NUN_BTNS_LAB+3, ROW_M_NUNCHUK_INFO, 1, " ", COLOR_FOREGROUND);
      screen_show();
    }
    lastButton_1_State = button_state;
  }
 
  button_state = digitalRead(PIN_BUTTON_2);
  if (button_state != lastButton_2_State) {
    if (button_state == 0) {  // pressed      
      screen_writeText_colrow(COL_M_NUN_BTNS_LAB+4, ROW_M_NUNCHUK_INFO, 1, "2", COLOR_ORANGE);;
      screen_show();
    } else {    
      screen_writeText_colrow(COL_M_NUN_BTNS_LAB+4, ROW_M_NUNCHUK_INFO, 1, " ", COLOR_FOREGROUND);
      screen_show();
    }
    lastButton_2_State = button_state;
  }

  
  if (current_time > nextHeartbeat) {
    //communic_send_message("imalive=1");
    communic_send_message('H', 1, MSG_NULL_FLOAT);
    nextHeartbeat = millis() + HEARTBEAT_INTERVAL;
  }
  
  if (current_time > nextBattDispDue) {
    nextBattDispDue = current_time + 60000;
    display_mybatt();
  }

  DEBUG_PRINTLN("end of loop");
  delay(50);
}

void neopixel_flash(int color) {
  pixels.fill(color);  // RED indicates past serial
  pixels.show(); 
  delay(500);
  pixels.fill(0x000000);
  pixels.show(); 
  delay(500);
}

void page_main_structure() {
  screen_writeText_colrow(COL_M_BATT_LAB_E, ROW_M_BATT1, 2, "E:", ST77XX_CYAN);
  screen_writeText_colrow(COL_M_BATT_LAB_M, ROW_M_BATT1, 2, "M:", ST77XX_CYAN);
  screen_writeText_colrow(COL_M_BATT_LAB_N, ROW_M_BATT1, 2, "N:", ST77XX_CYAN);
  
  screen_writeText_colrow(COL_M_BATT_LAB_E, ROW_M_BATT2, 2, "E:", ST77XX_CYAN);
  screen_writeText_colrow(COL_M_BATT_LAB_M, ROW_M_BATT2, 2, "M:", ST77XX_CYAN);
  screen_writeText_colrow(COL_M_BATT_LAB_N, ROW_M_BATT2, 2, "N:", ST77XX_CYAN);
  
  screen_writeText_colrow(COL_M_NUN_X_LAB, ROW_M_NUNCHUK_INFO, 2, "Y:", ST77XX_CYAN);
  screen_writeText_colrow(COL_M_NUN_Y_LAB, ROW_M_NUNCHUK_INFO, 2, "X:", ST77XX_CYAN);
  screen_show();
}

void page_clear_status_area() {
  screen_clearLine(ROW_M_STAT1);
  screen_clearLine(ROW_M_STAT2);
  screen_show();
}
