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
 * **********************************************************************************
 * **********************************************************************************            
*/


#include <NintendoExtensionCtrl.h>
#include "config.h"
#include "communic.h"
#include "screen_oled.h"
#include <Adafruit_NeoPixel.h>

#define HEARTBEAT_INTERVAL 1000
#define NUMPIXELS 1
Adafruit_NeoPixel pixels(NUMPIXELS, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);

String jsonBuffer;
int lastJoyY, lastJoyX;
bool lastButtonC, lastButtonZ;
int button_state, lastButtonState;
long current_time;
long lastHeartbeatMsg;  
long nextBattDispDue;
long nextHeartbeat;

Nunchuk nchuk;

/*
 * private function declarations
 */
float read_batt();
void neopixel_flash(int color);
void display_mybatt();

void setup() {

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
  
  neopixel_flash(0xFF0000);  // RED indicates past serial

  DEBUG_PRINTLN("started");
	nchuk.begin();
  DEBUG_PRINTLN("nunchuk initizialized");
   

	while (!nchuk.connect()) {
		DEBUG_PRINTLN("Nunchuk not detected!");
    pixels.fill(0xc41a00);  // ORANGE indicates error nunchuk
    pixels.show(); 
		delay(1000);
	} 

  DEBUG_PRINTLN("nunchuk connected");
  
  neopixel_flash(0x00FF00);  // GREEN indicates past nunchuk

  lastJoyY = 0;
  lastJoyX = 0;
  lastButtonC = false;
  lastButtonZ = false;
  lastButtonState = 1;
  
  screen_init();  
  neopixel_flash(0x0000FF);  // BLUE indicates past screen  
  communic_init();  
  neopixel_flash(0xFFFF00);  // YELLLOW indicates past communic

  screen_writeText_colrow(COL_LABEL_Y, ROW_STAT1, 2, "Y:", COLOR_FOREGROUND);
  screen_writeText_colrow(COL_LABEL_X, ROW_STAT1, 2, "X:", COLOR_FOREGROUND);
  screen_show();

  pinMode(PIN_BUTTON, INPUT_PULLUP);
  
  nextHeartbeat = millis() + HEARTBEAT_INTERVAL;
  nextBattDispDue = millis() + 2000;
  analogReadResolution(12);
}

void loop() {
  int rawJoyX, rawJoyY, centeredJoyX, centeredJoyY, scaledJoyX, scaledJoyY;
	boolean success;
  char tmpBuf[12];

  current_time = millis();
  success = nchuk.update();  // Get new data from the controller
  
	if (!success) {  // Ruh roh
		DEBUG_PRINTLN("Controller disconnected!");
		delay(1000);
	}
	else {
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
        screen_writeText_colrow(COL_BTN_Z, ROW_STAT1, 1, "Z", COLOR_FOREGROUND);
      } else {
        //communic_send_message("zbutn=0");  
        communic_send_message('Z', 0, MSG_NULL_FLOAT);
        screen_writeText_colrow(COL_BTN_Z, ROW_STAT1, 1, " ", COLOR_FOREGROUND);      
      }
      screen_show();
    }
   
    if (cButton != lastButtonC) {
      if (cButton) {
        //communic_send_message("cbutn=1");
        communic_send_message('C', 1, MSG_NULL_FLOAT);
        screen_writeText_colrow(COL_BTN_C, ROW_STAT1, 1, "C", COLOR_FOREGROUND);
      } else {
        //communic_send_message("cbutn=0");
        communic_send_message('C', 0, MSG_NULL_FLOAT);
        screen_writeText_colrow(COL_BTN_C, ROW_STAT1, 1, " ", COLOR_FOREGROUND);        
      }
      screen_show();
    }
   
    if (abs(scaledJoyX - lastJoyX) > 1) {
      //communic_send_message("joyX="+String(scaledJoyX)); 
      communic_send_message('X', scaledJoyX, MSG_NULL_FLOAT);
      screen_writeText_colrow(COL_VALUE_X, ROW_STAT1, 4, itoa(scaledJoyX, tmpBuf, 10), COLOR_FOREGROUND);
      screen_show();
    }
   
    if (abs(scaledJoyY - lastJoyY) > 1) {
      //communic_send_message("joyY="+String(scaledJoyY)); 
      communic_send_message('Y', scaledJoyY, MSG_NULL_FLOAT);
      screen_writeText_colrow(COL_VALUE_Y, ROW_STAT1, 4, itoa(scaledJoyY, tmpBuf, 10), COLOR_FOREGROUND);
      screen_show();
    }
     
    lastJoyY = scaledJoyY;
    lastJoyX = scaledJoyX;
    lastButtonC = cButton;
    lastButtonZ = zButton;
	}
	
  button_state = digitalRead(PIN_BUTTON);
  if (button_state != lastButtonState) {
    if (button_state == 0) {  // pressed      
      screen_writeText_colrow(COL_BTN_B, ROW_STAT1, 1, "B", COLOR_FOREGROUND);;
      screen_show();
    } else {    
      screen_writeText_colrow(COL_BTN_B, ROW_STAT1, 1, " ", COLOR_FOREGROUND);
      screen_show();
    }
    lastButtonState = button_state;
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

/* 
 * read battery voltage.  note that battery is divided by resistive 
 * divider with 4.7k and 1k, such that nominal 3.6v (3 NiMH AA) should
 * yield approx 0.63v at pin A2
 * https://docs.espressif.com/projects/esp-idf/en/v4.4.1/esp32s2/api-reference/peripherals/adc.html
 * https://docs.espressif.com/projects/esp-idf/en/v4.4.1/esp32s2/api-reference/peripherals/adc.html#_CPPv414adc1_channel_t
 * attenuation ADC_ATTEN_DB_0 allows to read 0 - 750 mV
 * Pin A2 (GPIO9) is on ADC1 which is the one that is ok to use even if using WiFi
 * per typedefs, ADC1_CHANNEL_8  is GPIO 9, which is probably A2 on QTPy
 * per typedefs, ADC_ATTEN_0db   is 0 atten, good for 100 - 750 mV range
 */
float read_batt() {
  int vbat_raw;
  float temp, batt_volts;
    
    vbat_raw = analogRead(PIN_VBAT_CHK);
    batt_volts = (VBAT_SCALING_CONSTANT * float(vbat_raw)) / 1000.0;
    return batt_volts;
}

 void display_mybatt() {  
    float batt_volts;
    char tmpBuf[12];

    batt_volts = read_batt();
    screen_writeText_colrow(COL_LEFTEDGE, ROW_BATT, 5, "Batt:", COLOR_FOREGROUND);
    dtostrf(batt_volts, 5, 2, tmpBuf);
    screen_writeText_colrow(COL_DATA, ROW_BATT, 5, tmpBuf, COLOR_FOREGROUND);
    screen_writeText_colrow(COL_DATA+5, ROW_BATT, 2, " v", COLOR_FOREGROUND);
    screen_show();
 }
